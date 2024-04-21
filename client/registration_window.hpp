#pragma once
#include <FL/Fl_Double_Window.H>

class Fl_Input;
class Fl_Secret_Input;

namespace Chat {
    class RegistrationWindow final : public Fl_Double_Window {
    public:
        RegistrationWindow(const int x, const int y, const char *title);
        ~RegistrationWindow() = default;
    private:
        Fl_Input        *m_nickname_field         {};
        Fl_Secret_Input *m_password_field         {};
        Fl_Secret_Input *m_confirm_password_field {};
    };
}
