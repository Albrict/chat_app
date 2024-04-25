#include "chat_group.hpp"
#include "../utils/packet.hpp"
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Multiline_Input.H>
#include <cstring>
#include <iostream>

namespace {
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
        const char *label = "Send";
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

Chat::ChatGroup::ChatGroup(const int x, const int y, const int connection_fd, const std::string &nickname)
    : Fl_Group(x, y, Window::width, Window::height, Window::title),
    m_connect_fd(connection_fd),
    m_nickname(nickname)
{
    
    Fl_Group::begin();
    {
        m_settings_bar  = new Fl_Menu_Bar(MenuBar::x, MenuBar::y, MenuBar::width, MenuBar::height);
        m_text_display  = new Fl_Text_Display(MessageOutput::x, MessageOutput::y, MessageOutput::width, MessageOutput::height);
        m_message_input = new Fl_Multiline_Input(MessageInput::x, MessageInput::y, MessageInput::width, MessageInput::height);
        m_buffer        = std::make_unique<Fl_Text_Buffer>();
        
        auto message_button = new Fl_Button(SendButton::x, SendButton::y, SendButton::width, SendButton::height, SendButton::label);

        m_text_display->buffer(m_buffer.get());
        message_button->callback(sendMessageCallback, this);
    }
    Fl_Group::end();
}

void Chat::ChatGroup::sendMessageCallback(Fl_Widget *widget, void *data)
{
    auto group            = static_cast<ChatGroup*>(data);
    std::string message   = group->m_message_input->value();
    std::string from      = group->m_nickname;

    NetworkUtils::MessagePacket packet(from, message);
    const bool result  = packet.send(group->m_connect_fd); 
    if (!result) {
        std::cerr << "Can't send message!\n";
    } else {
        std::string display_message = "You: " + message;
        display_message.push_back('\n');
        group->m_text_display->insert(display_message.c_str());
        group->m_message_input->value(""); 
    }
}

void Chat::ChatGroup::displayMessage(const NetworkUtils::MessagePacket &packet)
{
    auto sender  = packet.getSender();
    auto message = packet.getMessage();

    sender.append(": ");
    message.push_back('\n');
    
    const auto final_message = sender + message;
    m_text_display->insert(final_message.c_str());
}
