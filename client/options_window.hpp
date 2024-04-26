#pragma once
#include <FL/Fl_Double_Window.H>

class Fl_Input;
class Fl_Box;
class Fl_Button;

namespace Chat {
    class OptionsWindow final : public Fl_Double_Window {
    public:
        OptionsWindow(const int x, const int y, const int width, const int height, const char *title);
        ~OptionsWindow() = default;
    private:
        Fl_Input  *host             {};
        Fl_Input  *port             {};
        Fl_Box    *connection_state {};
        Fl_Button *save             {};
    };
}
