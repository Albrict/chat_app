#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <FL/Fl_Group.H>
#include <stdexcept>

#include "client.hpp"
#include "chat_group.hpp"
#include "login_group.hpp"
#include "../utils/packet.hpp"
#include "../utils/network_utils.hpp"

using namespace Chat;

namespace {
    Fl_Group *createChatGroup(const int x, const int y, const int m_connect_fd);
}

Client::Client()
    : m_window(100, 100)
{

    const char *connection_error = "Can't connect to the server!\n";
    m_connect_fd = NetworkUtils::getConnectionSocket("shabaka-pc", "3490");
    if (m_connect_fd < 0)
        throw std::runtime_error(connection_error);
     
    Fl::add_fd(m_connect_fd, FL_READ, connectionReadEventCallback, this);
    startLogin();
    m_window.show();
    m_window.position(Fl::w() / 2, Fl::h() / 2);
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
    NetworkUtils::Packet packet(fd); 
    if (packet.isEmpty()) {
        ;
    } else {
        auto client = static_cast<Client*>(data);
        switch(packet.type()) {
        using enum NetworkUtils::Packet::Type; 
        case SERVER_LOGIN_OK:
        case SERVER_REGISTRATION_OK:
            client->startChat();
            break;
        case _MESSAGE:
            if (client->m_chat)
                client->m_chat->displayMessage(NetworkUtils::MessagePacket(packet));
        default:
            break;
        }
        auto message = packet.asString();
        message.append("\n");
        std::cout << message;
    }
}

void Client::startChat()
{
    m_chat = new ChatGroup(0, 0, m_connect_fd, m_login->nickname());
    m_window.remove(m_login);
    m_window.add(m_chat);
    m_window.resize(m_window.x(), m_window.y(), m_chat->w(), m_chat->h());
    m_window.redraw();
    m_window.label(m_chat->label());
}

void Client::startLogin()
{
    m_login = new LoginGroup(0, 0, m_connect_fd);
    m_window.add(m_login);
    m_window.resize(m_login->x(), m_login->y(), m_login->w(), m_login->h());
    m_window.label(m_login->label());
}

//void Client::settingsButtonCallback(Fl_Widget *widget, void *data)
//{
//    auto client = static_cast<Client*>(data);
//    if (!client->m_options_window) {
//        const     int win_width  = 400;
//        const     int win_height = 300;
//        constexpr int win_x      = 1024 / 2;
//        constexpr int win_y      = 768  / 2;
//
//        client->m_options_window = std::make_unique<OptionsWindow>(win_x, win_y, win_width, win_height, "Settings");
//        client->m_options_window->show();
//    } else {
//        if (client->m_options_window->visible() == 0) // Window is visible
//            client->m_options_window->show();
//        else 
//            client->m_options_window->hide();
//    }
//}
//
//
//void Client::registrationCallback(Fl_Widget *widget, void *data)
//{
//    auto client             = static_cast<Client*>(data);
//    auto message            = client->m_message_input->value();
//    auto message_as_bytes   = reinterpret_cast<std::byte*>(const_cast<char*>(message));
//    auto packet_data        = std::vector<std::byte>(message_as_bytes, message_as_bytes + strlen(message) + 1);
//
//    NetworkUtils::Packet packet(packet_data, 4);
//    bool result  = packet.send(client->m_connect_fd); 
//    if (!result) {
//        std::cerr << "Can't send message!\n";
//    }
//}
