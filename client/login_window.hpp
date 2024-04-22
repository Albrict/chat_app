#pragma once
#include <FL/Fl_Double_Window.H>

class Fl_Input;
class Fl_Secret_Input;
class Fl_Group;
class Fl_Box;

namespace Chat {
    class LoginWindow final : public Fl_Double_Window {
    public:
        LoginWindow(const int x, const int y, const int connection_fd);
        ~LoginWindow() = default;
    private:
        [[nodiscard]] Fl_Group *createLoginGroup();
        [[nodiscard]] Fl_Group *createRegistrationGroup();
    
        static void loginButtonCallback(Fl_Widget *widget, void *data);
        static void signUpButtonCallback(Fl_Widget *widget, void *data);
        static void backButtonCallback(Fl_Widget *widget, void *data);
        static void registerButtonCallback(Fl_Widget *widget, void *data);
    private:
        const int       m_connection_fd;
        Fl_Group        *m_current_group         {};
        Fl_Input        *m_nickname_field        {};
        Fl_Secret_Input *m_password_field        {};
        Fl_Secret_Input *m_repeat_password_field {};
        Fl_Box          *m_info_box              {};
    };
}
