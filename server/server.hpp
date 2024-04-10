#pragma once
#include <sys/socket.h>
#include <poll.h>
#include <vector>
#include <string>
#include "../utils/packet.hpp"

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
        void handlePacket(const NetworkUtils::Packet &packet, const int sender_fd);
    private:
        int                        m_listen_fd        {};
        int                        m_connection_fd    {};
        socklen_t                  m_address_length   {};
        sockaddr_storage           m_connection_info  {};
        std::vector<pollfd>        m_pollfds          {};
        std::string                m_address          {};
    };
}
