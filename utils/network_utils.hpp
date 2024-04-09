#pragma once
#include <netdb.h>
#include <sys/poll.h>
#include "packet.hpp"

namespace NetworkUtils {
    [[nodiscard]] int getListenSocket(const char *port, const addrinfo *hints = nullptr) noexcept;
    [[nodiscard]] int getConnectionSocket(const char *host, const char *port, const addrinfo *hints = nullptr) noexcept;
    [[nodiscard]] void *get_in_addr(sockaddr *sa) noexcept;
    [[nodiscard]] bool sendAllData(const int connection_fd, const std::string &buffer) noexcept;
    [[nodiscard]] bool sendPacket(const int connection_fd, const Packet &packet);
    [[nodiscard]] inline pollfd createPollfd(const int fd, const short events) noexcept
    {
        return (pollfd){ .fd = fd, .events = events }; 
    }
    void serializeUnsignedInt(const unsigned int num, std::byte *buffer) noexcept;
    [[nodiscard]] uint32_t deserializeUnsignedInt(const std::byte *data) noexcept;
}
