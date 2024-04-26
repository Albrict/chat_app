#pragma once
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

class Fl_Group;
class Fl_Input;
class Fl_Box;

namespace Chat {
    class OptionsWindow;
    class ChatGroup;
    class LoginGroup;

    class Client final {
    public:
        Client();
        ~Client();

        int run();
    private:
        Fl_Group *createConnectionGroup(const int x, const int y);
        void connect();
        void startConnect();
        void startLogin();
        void startChat();
        static void connectionReadEventCallback(FL_SOCKET fd, void *data);
        static void connectButtonCallback(Fl_Widget *widget, void *data);
    private:
        Fl_Double_Window  m_window;                        
        Fl_Group          *m_connect        {};
        LoginGroup        *m_login          {};
        ChatGroup         *m_chat           {};
        Fl_Input          *m_server_ip      {};
        Fl_Input          *m_server_port    {};
        Fl_Box            *m_info_box       {};
        int               m_connect_fd      {};
        bool              m_connected       {false};
    };
}
