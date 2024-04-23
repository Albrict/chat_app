#pragma once
#include <FL/Fl_Group.H>
#include <memory>
#include <string>
#include "../utils/packet.hpp"

class Fl_Multiline_Input;
class Fl_Text_Display;
class Fl_Menu_Bar;
class Fl_Text_Buffer;

namespace Chat {
    class ChatGroup final : public Fl_Group {
    public:
        explicit ChatGroup(const int x, const int y, const int connection_fd, const std::string &nickname);
        ~ChatGroup() = default;

        void displayMessage(const NetworkUtils::MessagePacket &packet);
    private:
        static void sendMessageCallback(Fl_Widget *widget, void *data);
    private:
        std::unique_ptr<Fl_Text_Buffer> m_buffer {};
        Fl_Multiline_Input *m_message_input      {};
        Fl_Text_Display    *m_text_display       {};
        Fl_Menu_Bar        *m_settings_bar       {};
        int                 m_connect_fd         {};
        std::string         m_nickname           {};
    };
}
