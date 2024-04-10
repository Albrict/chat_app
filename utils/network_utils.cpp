#include "network_utils.hpp"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <array>

int NetworkUtils::getListenSocket(const char *port, const addrinfo *hints) noexcept
{
    int      listen_fd = 0;
    int      yes       = 1;
    int      result    = 0;
    addrinfo *servinfo = nullptr; 
    addrinfo internal_hints {};

    if (!hints) {
        internal_hints.ai_family   = AF_UNSPEC;   // Any IP (IPv4 or IPv6)
        internal_hints.ai_socktype = SOCK_STREAM; // Stream connection
        internal_hints.ai_flags    = AI_PASSIVE;  // Use server IP
        result = getaddrinfo(nullptr, port, &internal_hints, &servinfo); // Get address info of host
    } else {
        result = getaddrinfo(nullptr, port, hints, &servinfo); // Get address info of host
    }

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

int NetworkUtils::getConnectionSocket(const char *host, const char *port, const addrinfo *hints) noexcept
{
    int  connect_fd       = 0; 
    int  result           = 0;
    bool connect_success  = false;

    struct addrinfo internal_hints {}; 
    struct addrinfo *servinfo       = nullptr;
    
    if (!hints) {
        internal_hints.ai_family = AF_UNSPEC;
        internal_hints.ai_socktype = SOCK_STREAM;
        result = getaddrinfo(host, port, &internal_hints, &servinfo);
    } else {
        result = getaddrinfo(host, port, hints, &servinfo);
    }
    
    if (result < 0 || servinfo == nullptr) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(result));
        return -1;
    }

    // loop through all the results and connect to the first we can
    auto tmp = servinfo;
    while(tmp) {
        connect_fd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
        if (connect_fd < 0){
            perror("client: socket");
            tmp = tmp->ai_next;
            continue;
        }
        
        result = connect(connect_fd, tmp->ai_addr, tmp->ai_addrlen);

        if (result < 0) {
            close(connect_fd);
            perror("client: connect");
            tmp = tmp->ai_next;
            continue;
        }

        connect_success = true;
        break;
    }
    freeaddrinfo(servinfo); // all done with this structure
    if (connect_success)
        return connect_fd;
    else
        return -1;
}

void *NetworkUtils::get_in_addr(sockaddr *sa) noexcept
{
    if (sa->sa_family == AF_INET)
        return &reinterpret_cast<sockaddr_in*>(sa)->sin_addr;
    else
        return &reinterpret_cast<sockaddr_in6*>(sa)->sin6_addr;
}


void NetworkUtils::serializeUnsignedInt(const unsigned int num, std::byte *buffer) noexcept
{
    uint32_t   converted  = htonl(num); 
    const auto byte_array = reinterpret_cast<std::byte*>(&converted);    
    memcpy(buffer, byte_array, sizeof(uint32_t));
}

uint32_t NetworkUtils::deserializeUnsignedInt(const std::byte *data) noexcept
{
    uint32_t host_value = 0;
    memcpy(&host_value, data, sizeof(host_value));
    host_value = ntohl(host_value);
    return host_value;  
}

bool NetworkUtils::sendAllData(const int connection_fd, const std::string &buffer) noexcept
{
    int bytes_sent = 0;        
    int result     = 0;
    int bytes_left = buffer.size();
    while (bytes_sent < buffer.size()) {
        result = send(connection_fd, &buffer[bytes_sent], bytes_left, 0);
        if (result < -1) {
            return false;
        } else {
            bytes_sent += result;
            bytes_left -= result;
        }
    }
    return true;
}

bool NetworkUtils::sendPacket(const int connection_fd, const Packet &packet)
{
    int bytes_sent = 0;        
    int result     = 0;
    int bytes_left = packet.size();
    while (bytes_sent < packet.size()) {
        result = send(connection_fd, &packet[bytes_sent], bytes_left, 0);
        if (result < -1) {
            return false;
        } else {
            bytes_sent += result;
            bytes_left -= result;
        }
    }
    return true;
}
