#include <stdexcept>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include "client.hpp"
#include "../utils/network_utils.hpp"

using namespace Chat;

namespace {
    int getConnectionSocket(const char *host, const char *port);
    void *get_in_addr(struct sockaddr *sa);
}

Client::Client(const int win_width, const int win_height, const char *win_title)
    : m_window(win_width, win_height, win_title)
{
    const char *connection_error = "Can't connect to the server!\n";
    m_connect_fd = NetworkUtils::getConnectionSocket("shabaka-pc", "3490");
    if (m_connect_fd < 0)
        throw std::runtime_error(connection_error);
    
    Fl::add_fd(m_connect_fd, FL_READ, connectionReadEventCallback, nullptr);
    m_window.resizable(&m_window);
    m_window.size_range(win_width, win_height);
    m_window.show();
}

Client::~Client()
{
    close(m_connect_fd); 
}

int Client::run()
{
    return Fl::run();
}

void Client::connectionReadEventCallback(FL_SOCKET fd, void *data)
{
    std::string m_buffer;
    m_buffer.resize(256);
    const int n_bytes   = recv(fd, m_buffer.data(), m_buffer.size(), 0); 
    
    if (n_bytes <= 0) {
        // Got connection error or closed by server 
        if (n_bytes == 0)
            printf("connection closed on socket:%d\n", fd);
        else 
            perror("recv");
    } else {
        // Received some data
        printf("Received data: %s\n", m_buffer.c_str());
    }
}
