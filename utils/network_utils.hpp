#pragma once
#include <netdb.h>
#include <sys/poll.h>
#include <string_view>
#include "packet.hpp"

namespace NetworkUtils {
    [[nodiscard]] int getListenSocket(const char *port, const addrinfo *hints = nullptr) noexcept;
    [[nodiscard]] int getConnectionSocket(const char *host, const char *port, const addrinfo *hints = nullptr) noexcept;
    [[nodiscard]] void *get_in_addr(sockaddr *sa) noexcept;
    [[nodiscard]] std::string_view serializeUnsignedInt(const unsigned int num) noexcept;
    [[nodiscard]] uint32_t deserializeUnsignedInt(const char *buffer) noexcept;
    [[nodiscard]] bool sendAllData(const int connection_fd, const std::string &buffer) noexcept;
    [[nodiscard]] Packet getPacket(const int client_fd);
    [[nodiscard]] inline pollfd createPollfd(const int fd, const short events) noexcept
    {
        return (pollfd){ .fd = fd, .events = events }; 
    }

}
