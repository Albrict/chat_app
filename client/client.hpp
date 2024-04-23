#pragma once
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

class Fl_Group;

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
        void startLogin();
        void startChat();
        static void connectionReadEventCallback(FL_SOCKET fd, void *data);
    private:
        Fl_Double_Window  m_window;                        
        LoginGroup        *m_login          {};
        ChatGroup         *m_chat           {};
        int               m_connect_fd      {};
    };
}
