#pragma once
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <memory>
#include <vector>
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

        [[nodiscard]] Fl_Menu_Bar *createMenuBar(const int x, const int y, const int width, const int height);
        [[nodiscard]] Fl_Multiline_Input *createMessageInput(const int x, const int y, const int width, const int height);
        [[nodiscard]] Fl_Button *createSendMessageButton(const int x, const int y, const int width, const int height);
        [[nodiscard]] Fl_Text_Display *createMessageDisplay(const int x, const int y, const int width, const int height);
    private:
        using string_ptr = std::unique_ptr<std::string>;
        Fl_Double_Window                          m_window;
        std::unique_ptr<OptionsWindow>            m_options_window {};
        std::unique_ptr<std::vector<string_ptr>>  m_output_buffer  {};
        Fl_Multiline_Input                        *m_message_input  = nullptr;
        Fl_Text_Display                           *m_message_output = nullptr;
        int                                       m_connect_fd = 0;
    };
}
