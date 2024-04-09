#include "server.hpp"
#include <boost/serialization/access.hpp>
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

#include "../utils/network_utils.hpp"

using namespace Chat;

Server::Server(const char *port)
{
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
        auto result = NetworkUtils::sendAllData(m_connection_fd, "Hello, world!");
        return true;
    }
}
