#include <FL/Fl_Button.H>
#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Box.H>

#include "client.hpp"
#include "chat_group.hpp"
#include "login_group.hpp"
#include "../utils/packet.hpp"
#include "../utils/network_utils.hpp"

using namespace Chat;

namespace {
    namespace Window {
        const char   *label  = "Connection";
        constexpr int width  = 300;
        constexpr int height = 200;
    }

    namespace Widget {
        constexpr int width  = Window::width / 2;
        constexpr int height = 20;
    }

    namespace ServerIpField {
        const char *label = "IP:"; 
        constexpr int x = Window::width / 2 - Widget::width / 2;
        constexpr int y = 1;
    }
    
    namespace ServerPortField {
        const char *label = "Port:";
        constexpr int x = ServerIpField::x;
        constexpr int y = Widget::height + 10; 
    }

    namespace InfoBox {
        const char *connection_error = "Can't connect to the server";
        
        constexpr int width = Widget::width + 50;
        constexpr int x     = ServerIpField::x;
        constexpr int y     = ServerPortField::y + Widget::height + 10; 
    }

    namespace ConnectButton {
        const char *label = "Connect";
        constexpr int x = ServerIpField::x;
        constexpr int y = InfoBox::y + Widget::height + 30; 
    }
}

Client::Client()
    : m_window(100, 100)
{
    startConnect();
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
    m_window.remove(m_connect);
    m_window.add(m_login);
    m_window.resize(m_window.x(), m_window.y(), m_login->w(), m_login->h());
    m_window.redraw();
    m_window.label(m_login->label());
}

void Client::startConnect()
{
    m_connect = createConnectionGroup(0, 0); 
    m_window.add(m_connect);
    m_window.resize(m_connect->x(), m_connect->y(), m_connect->w(), m_connect->h());
    m_window.label(m_connect->label());
}

Fl_Group *Client::createConnectionGroup(const int x, const int y)
{
    auto group = new Fl_Group(x, y, Window::width, Window::height, Window::label);
    group->begin();
    {
        m_server_ip   = new Fl_Input(ServerIpField::x, ServerIpField::y, Widget::width, Widget::height, ServerIpField::label);
        m_server_port = new Fl_Input(ServerPortField::x, ServerPortField::y, Widget::width, Widget::height, ServerPortField::label); 
        m_info_box    = new Fl_Box(InfoBox::x, InfoBox::y, InfoBox::width, Widget::height);         

        auto connect_button = new Fl_Button(ConnectButton::x, ConnectButton::y, Widget::width, Widget::height, ConnectButton::label);

        connect_button->callback(connectButtonCallback, this);
        m_info_box->labelcolor(FL_RED);
    }
    group->end();
    return group;
}

void Client::connectButtonCallback(Fl_Widget *widget, void *data)
{
    auto client = static_cast<Client*>(data);
    auto host   = client->m_server_ip->value();
    auto port   = client->m_server_port->value();

    client->m_connect_fd = NetworkUtils::getConnectionSocket(host, port);
    if (client->m_connect_fd < 0) {
        client->m_info_box->label(InfoBox::connection_error);
    } else  {
        client->m_info_box->label("");
        Fl::add_fd(client->m_connect_fd, FL_READ, connectionReadEventCallback, client);
        client->startLogin();
    }
}
