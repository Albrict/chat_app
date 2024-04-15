#include <iostream>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_Text_Display.H>

#include <stdexcept>
#include <vector>

#include "client.hpp"
#include "options_window.hpp"
#include "../utils/packet.hpp"
#include "../utils/network_utils.hpp"

using namespace Chat;

namespace {
     
    namespace Adjustments {
        constexpr int padding = 20;
    }

    namespace Window {
        const char    *title = "Chat client";
        constexpr int width  = 512;
        constexpr int height = 320; 
    }

    namespace MenuBar {
        constexpr int x      = 0;
        constexpr int y      = 0;
        constexpr int width  = Window::width; 
        constexpr int height = 20;
    };
    
    namespace SendButton {
        constexpr int height = 40;
        constexpr int x      = Window::width / 1.5;
        constexpr int y      = Window::height - height; 
        constexpr int width  = Window::width - x;
    }

    namespace MessageInput {
        constexpr int height = 40;
        constexpr int x      = 0;
        constexpr int y      = Window::height - height;
        constexpr int width  = Window::width - SendButton::width; 
    }

    namespace MessageOutput {
        constexpr int width  = Window::width - 20;
        constexpr int height = SendButton::y - MenuBar::height;
        constexpr int x      = 10;
        constexpr int y      = MenuBar::height;
    }
}

Client::Client()
    : m_window(Window::width, Window::height, Window::title)
{
    const char *connection_error = "Can't connect to the server!\n";
    m_connect_fd = NetworkUtils::getConnectionSocket("shabaka-pc", "3490");
    if (m_connect_fd < 0)
        throw std::runtime_error(connection_error);
    
    Fl::add_fd(m_connect_fd, FL_READ, connectionReadEventCallback, this);
    m_window.resizable(&m_window);
    m_window.size_range(Window::width, Window::height);

    m_window.begin();
    {
        const int bar_height    = 20;
        auto      bar           = createMenuBar(MenuBar::x, MenuBar::y, MenuBar::width, MenuBar::height);
        auto      button        = createSendMessageButton(SendButton::x, SendButton::y, SendButton::width, SendButton::height);
        auto      buffer        = new Fl_Text_Buffer;

        m_message_output        = new Fl_Text_Display(MessageOutput::x, MessageOutput::y, MessageOutput::width, MessageOutput::height);
        m_message_input         = createMessageInput(MessageInput::x, MessageInput::y, MessageInput::width, MessageInput::height);
        m_output_buffer         = std::make_unique<std::vector<string_ptr>>();

        m_message_output->buffer(buffer);
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
    NetworkUtils::Packet packet(fd); 
    if (packet.isEmpty()) {
        ;
    } else {
        auto client         = static_cast<Client*>(data);
        auto message_output = client->m_message_output;
        auto message        = std::make_unique<std::string>(packet.asString());
        message->append("\n");
        message_output->insert(message->c_str());
        client->m_output_buffer->emplace_back(std::move(message));
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

Fl_Text_Display *Client::createMessageDisplay(const int x, const int y, const int width, const int height)
{
    
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
    auto client             = static_cast<Client*>(data);
    auto message            = client->m_message_input->value();
    auto message_as_bytes   = reinterpret_cast<std::byte*>(const_cast<char*>(message));
    auto packet_data        = std::vector<std::byte>(message_as_bytes, message_as_bytes + strlen(message) + 1);
    NetworkUtils::Packet packet(packet_data, 4);
    bool result  = packet.send(client->m_connect_fd); 
    if (!result) {
        std::cerr << "Can't send message!\n";
    }
}
