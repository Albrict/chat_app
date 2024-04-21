#include "server.hpp"
#include <boost/serialization/access.hpp>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <string>
#include <chrono>

#include "../utils/network_utils.hpp"
#include "sha.hpp"

namespace {
    [[nodiscard]] std::string getSalt();
    [[nodiscard]] bool sendNotificationPacket(const NetworkUtils::Packet::Type type, const int sender_fd);

}

using namespace Chat;

Server::Server(const char *port, std::unique_ptr<Database> database)
{
    m_database  = std::move(database);
    m_listen_fd = NetworkUtils::getListenSocket(port);
    if (m_listen_fd < 0) {
        const char *err = "Fatal error: can't get listen socket";
        throw std::runtime_error(err);
    }

    const int result = listen(m_listen_fd, SOMAXCONN); 
    if (result < 0) {
        const char *err = "listen";
        perror(err);
        throw std::runtime_error(err);
    }
    m_address.resize(INET6_ADDRSTRLEN);
}

Server::~Server()
{
    for (const auto &sockets : m_pollfds)
        close(sockets.fd);
}

int Server::run()
{
    m_pollfds.emplace_back(NetworkUtils::createPollfd(m_listen_fd, POLLIN));
    
    printf("server: waiting for connections...\n");
    while (true) {
        int poll_count = poll(m_pollfds.data(), m_pollfds.size(), -1);

        if (poll_count < 0) {
            const char *error = "poll";
            perror(error);
            return 1;
        }
        checkConnections();
    }
    return 0;
}

void Server::checkConnections()
{
    for (size_t i = 0; i < m_pollfds.size(); ++i) {
        if (m_pollfds[i].revents & POLLIN) { 
            if (m_pollfds[i].fd == m_listen_fd) {
                handleConnectionRequest();
            } else  {
                NetworkUtils::Packet packet(m_pollfds[i].fd); 
                if (packet.isEmpty()) {
                    close(m_pollfds[i].fd);
                    m_pollfds.erase(m_pollfds.begin() + i);
                } else {
                    handlePacket(packet, m_pollfds[i].fd);
                }
            }
        } else { 
            ;
        }
    }
}

bool Server::handleConnectionRequest()
{
    auto sockaddr_connect = reinterpret_cast<sockaddr*>(&m_connection_info);
    m_address_length = sizeof(m_connection_info);   
    m_connection_fd  = accept(m_listen_fd, sockaddr_connect, &m_address_length);
    if (m_connection_fd < 0) {
        const char *error = "accept";
        perror(error);
        return false;
    } else {
        const char *connection_message = "pollserver: got connection from:\n";
        inet_ntop(m_connection_info.ss_family, sockaddr_connect, m_address.data(), INET6_ADDRSTRLEN);
        m_pollfds.push_back(NetworkUtils::createPollfd(m_connection_fd, POLLIN));
        std::cout << connection_message;
        std::cout << m_address << '\n';
        return true;
    }
}

void Server::handlePacket(const NetworkUtils::Packet &packet, const int sender_fd)
{
    if (packet.type() == NetworkUtils::Packet::Type::SERVER_REGISTRATION) {
        const char *err_msg = "Can't send a notification to client!\n";
        NetworkUtils::LoginPacket login_packet(packet);

        if (m_database->isUserExists(login_packet.nickname().c_str())) {
            if (!sendNotificationPacket(NetworkUtils::Packet::Type::SERVER_REGISTRATION_ALREADY_EXISTS, sender_fd))
                std::cerr << err_msg;
        } else { 
            auto nickname = login_packet.nickname();
            auto password = login_packet.password();
            auto salt     = getSalt();
            password += salt;
            SHA1 checksum;
            checksum.update(password);

            auto hashed_password = checksum.final();
            if (m_database->addUser(nickname, hashed_password, salt)) {
                if (!sendNotificationPacket(NetworkUtils::Packet::Type::SERVER_REGISTRATION_OK, sender_fd))
                    std::cerr << err_msg;
            }
        } 
    }

    if (packet.type() != NetworkUtils::Packet::Type::SERVER) {
        std::cout << "Packet size: " << packet.size() << '\n';
        std::cout << "Packet type: " << packet.type() << '\n';
        std::cout << "Packet data: " << packet.asChars() << '\n';
        for (const auto &clients : m_pollfds) {
            if (clients.fd != m_listen_fd && clients.fd != sender_fd) {
                NetworkUtils::Packet send_packet(std::move(packet));
                send_packet.send(clients.fd);
            }        
        }
    }
}


namespace {
    std::string getSalt()
    {
        auto time          = std::chrono::system_clock::now();  
        auto s_time        = std::chrono::system_clock::to_time_t(time);
        auto time_str      = std::ctime(&s_time);
        return time_str;
    }

    bool sendNotificationPacket(const NetworkUtils::Packet::Type type, const int sender_fd)
    {
        auto message            = NetworkUtils::Packet::messages[type]; 
        auto message_as_bytes   = reinterpret_cast<std::byte*>(const_cast<char*>(message));
        auto packet_data        = std::vector<std::byte>(message_as_bytes, message_as_bytes + strlen(message) + 1);

        NetworkUtils::Packet packet(packet_data, type); 
        return packet.send(sender_fd); 
    }
}
