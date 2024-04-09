#include <FL/Fl_Widget.H>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Multiline_Input.H>

#include <stdexcept>

#include "client.hpp"
#include "options_window.hpp"
#include "../utils/network_utils.hpp"

using namespace Chat;

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

    m_window.begin();
    {
        const int bar_height = 20;
        auto      bar        = createMenuBar(0, 0, win_width, bar_height);
        auto      button     = createSendMessageButton(700, 500, 100, 10);
        m_message_input      = createMessageInput(500, 500, 200, 100);
    }
    m_window.end();
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

Fl_Menu_Bar *Client::createMenuBar(const int x, const int y, const int width, const int height)
{
    Fl_Menu_Item popup[] = {
        {"&Settings",    FL_ALT+'a', settingsButtonCallback, this},
        {0}
    };
    Fl_Menu_Bar *bar = new Fl_Menu_Bar(x, y, width, height);
    bar->copy(popup);
    return bar;
}

Fl_Multiline_Input *Client::createMessageInput(const int x, const int y, const int width, const int height)
{
    auto input = new Fl_Multiline_Input(x, y, width, height); 
    return input;
}



Fl_Button *Client::createSendMessageButton(const int x, const int y, const int width, const int height)
{
    const char *button_label = "Send message";
    auto button = new Fl_Button(x, y, width, height, button_label); 
    button->callback(sendMessageCallback, this);
    return button;
}

void Client::settingsButtonCallback(Fl_Widget *widget, void *data)
{
    auto client = static_cast<Client*>(data);
    if (!client->m_options_window) {
        const     int win_width  = 400;
        const     int win_height = 300;
        constexpr int win_x      = 1024 / 2;
        constexpr int win_y      = 768  / 2;

        client->m_options_window = std::make_unique<OptionsWindow>(win_x, win_y, win_width, win_height, "Settings");
        client->m_options_window->show();
    } else {
        if (client->m_options_window->visible() == 0) // Window is visible
            client->m_options_window->show();
        else 
            client->m_options_window->hide();
    }
}

void Client::sendMessageCallback(Fl_Widget *widget, void *data)
{
    auto client            = static_cast<Client*>(data);
    auto message           = client->m_message_input->value();
    auto packet_data       = reinterpret_cast<const std::byte*>(message);

    NetworkUtils::Packet packet(strlen(message) + 1, NetworkUtils::Packet::Type::CLIENT, packet_data);
    bool result  = NetworkUtils::sendPacket(client->m_connect_fd, packet);
    if (!result) {
        std::cerr << "Can't send message!\n";
    }
}
