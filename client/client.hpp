#pragma once
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

#include <memory>
#include <string>

class Fl_Menu_Bar;
class Fl_Multiline_Input;
class Fl_Button;
class Fl_Text_Display;
class Fl_Text_Buffer;

namespace Chat {
    class OptionsWindow;

    class Client final {
    public:
        Client();
        ~Client();

        int run();
    private:
        static void connectionReadEventCallback(FL_SOCKET fd, void *data);
        static void settingsButtonCallback(Fl_Widget *widget, void *data);
        static void sendMessageCallback(Fl_Widget *widget, void *data);
        static void registrationCallback(Fl_Widget *widget, void *data);

        [[nodiscard]] Fl_Menu_Bar *createMenuBar(const int x, const int y, const int width, const int height);
        [[nodiscard]] Fl_Multiline_Input *createMessageInput(const int x, const int y, const int width, const int height);
        [[nodiscard]] Fl_Button *createSendMessageButton(const int x, const int y, const int width, const int height);
        [[nodiscard]] Fl_Text_Display *createMessageDisplay(const int x, const int y, const int width, const int height);
    private:
        using string_ptr = std::unique_ptr<std::string>;
        std::unique_ptr<Fl_Double_Window>         m_current_window  {};
        std::unique_ptr<OptionsWindow>            m_options_window  {};
        std::unique_ptr<Fl_Text_Buffer>           m_buffer          {};
        Fl_Multiline_Input                        *m_message_input  {};
        Fl_Text_Display                           *m_message_output {};
        int                                       m_connect_fd      {};
    };
}
