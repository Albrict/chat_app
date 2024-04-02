#include "server.hpp"
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#include <string>

using namespace Chat;

namespace {
    [[nodiscard]] int getListenSocket(const char *port, const addrinfo *hints = nullptr);
    void sigchild_handler(const int s);
    void *get_in_addr(sockaddr *sa);
    [[nodiscard]] inline pollfd createPollfd(const int fd, const short events); 
}

Server::Server(const char *port)
{
    m_listen_fd = getListenSocket(port);
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
    m_buffer.resize(256);
}

Server::~Server()
{
    for (const auto &sockets : m_pollfds)
        close(sockets.fd);
}

int Server::run()
{
    m_pollfds.emplace_back(createPollfd(m_listen_fd, POLLIN));
    
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
                auto data = handleDataFromClient(m_pollfds[i]); 
                if (data == nullptr) {
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
        m_pollfds.push_back(createPollfd(m_connection_fd, POLLIN));
        std::cout << connection_message;
        std::cout << m_address << '\n';
        send(m_connection_fd, "Hello, world!", 13, 0);
        return true;
    }
}

const char *Server::handleDataFromClient(const pollfd &pollfd_connect)
{
    const int n_bytes   = recv(pollfd_connect.fd, m_buffer.data(), m_buffer.size(), 0); 
    
    if (n_bytes <= 0) {
        // Got connection error or closed by client
        if (n_bytes == 0)
            printf("connection closed on socket:%d\n", pollfd_connect.fd);
        else 
            perror("recv");
        return nullptr;
    } else {
        // Received some data
        printf("Received data: %s\n", m_buffer.c_str());
        return m_buffer.c_str();
    }
}

namespace {
    [[nodiscard]] int getListenSocket(const char *port, const addrinfo *hints)
    {
        int      listen_fd = 0;
        int      yes       = 1;
        int      result    = 0;
        addrinfo *servinfo = nullptr; 

        if (!hints) {
            addrinfo internal_hints {0};

            internal_hints.ai_family   = AF_UNSPEC;   // Any IP (IPv4 or IPv6)
            internal_hints.ai_socktype = SOCK_STREAM; // Stream connection
            internal_hints.ai_flags    = AI_PASSIVE;  // Use server IP
            
            hints = &internal_hints; 
        }

        result = getaddrinfo(nullptr, port, hints, &servinfo); // Get address info of host
                                                               //
        if (result != 0 || servinfo == nullptr) {
            std::string error = "Error in getaddrinfo:";
            std::cerr << error << '\n';
            std::cerr << gai_strerror(result) << '\n';
            return -1;
        }

        auto tmp = servinfo;
        while (tmp) {
            listen_fd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
            if (listen_fd == -1) {
                const char *error = "server: socket";
                perror(error);
                tmp = tmp->ai_next; // Get to the next node
                continue;
            }

            result = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
            if (result == -1) {
                const char *error = "setsockopt";
                perror(error);
                return -1;
            }

            result = bind(listen_fd, tmp->ai_addr, tmp->ai_addrlen);
            if (result == -1) {
                const char *error = "server: bind";
                close(listen_fd);
                perror(error);
                tmp = tmp->ai_next; // Get to the next node
                continue;
            }
            break;
        }
        freeaddrinfo(servinfo);
        servinfo = nullptr;
        tmp      = nullptr;
        return listen_fd;
    }

    void sigchild_handler(const int s)
    {
        const int saved_errno = errno;
        while (waitpid(-1, nullptr, WNOHANG) > 0);
        errno = saved_errno;
    }

    void *get_in_addr(sockaddr *sa)
    {
        if (sa->sa_family == AF_INET)
            return &reinterpret_cast<sockaddr_in*>(sa)->sin_addr;
        else
            return &reinterpret_cast<sockaddr_in6*>(sa)->sin6_addr;
    }

    [[nodiscard]] inline pollfd createPollfd(const int fd, const short events)
    {
        return (pollfd){ .fd = fd, .events = events }; 
    }
}


