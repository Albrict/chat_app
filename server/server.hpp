#pragma once
#include <sys/socket.h>
#include <poll.h>
#include <vector>
#include <string>
#include "../utils/network_utils.hpp"

namespace Chat {
    class Server {
    public:
        Server(const char *port);
        ~Server();

        int run();
    private:
        void checkConnections();
        void clearDisconnectQueue();
        bool handleConnectionRequest();
    private:
        static const int           packet_size        = 256;
        int                        m_listen_fd        {};
        int                        m_connection_fd    {};
        socklen_t                  m_address_length   {};
        sockaddr_storage           m_connection_info  {};
        std::vector<pollfd>        m_pollfds          {};
        NetworkUtils::DataBuffer   m_buffer           {packet_size};
        std::string                m_address          {};
    };
}
