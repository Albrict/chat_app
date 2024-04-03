#pragma once
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

namespace Chat {
    class Client final {
    public:
        Client(const int win_width, const int win_height, const char *win_title = nullptr);
        ~Client();

        int run();
    private:
        static void connectionReadEventCallback(FL_SOCKET fd, void *data);
    private:
        Fl_Double_Window m_window;
        int m_connect_fd = 0;
    };
}
