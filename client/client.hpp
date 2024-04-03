#pragma once
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <memory>

class Fl_Menu_Bar;

namespace Chat {
    class OptionsWindow;

    class Client final {
    public:
        Client(const int win_width, const int win_height, const char *win_title = nullptr);
        ~Client();

        int run();
    private:
        static void connectionReadEventCallback(FL_SOCKET fd, void *data);
        static void settingsButtonCallback(Fl_Widget *widget, void *data);

        [[nodiscard]] Fl_Menu_Bar *createMenuBar(const int x, const int y, const int width, const int height);
    private:
        Fl_Double_Window                  m_window;
        std::unique_ptr<OptionsWindow>    m_options_window {};
        int                               m_connect_fd = 0;
    };
}
