#pragma once
#include <netdb.h>
#include <sys/poll.h>

namespace NetworkUtils {
    [[nodiscard]] int getListenSocket(const char *port, const addrinfo *hints = nullptr) noexcept;
    [[nodiscard]] int getConnectionSocket(const char *host, const char *port, const addrinfo *hints = nullptr) noexcept;
    [[nodiscard]] void *get_in_addr(sockaddr *sa) noexcept;
    [[nodiscard]] inline pollfd createPollfd(const int fd, const short events) noexcept
    {
        return (pollfd){ .fd = fd, .events = events }; 
    }
}
