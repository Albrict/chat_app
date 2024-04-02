#pragma once
#include <sys/socket.h>
#include <poll.h>
#include <vector>
#include <string>

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
        const char *handleDataFromClient(const pollfd &pollfd_connect);
    private:
        int                 m_listen_fd        {0};
        int                 m_connection_fd    {0};
        socklen_t           m_address_length   {0};
        sockaddr_storage    m_connection_info  {0};
        std::vector<pollfd> m_pollfds          {};
        std::string         m_address          {};
        std::string         m_buffer           {};
    };
}
