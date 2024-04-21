#include "registration_window.hpp"
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Secret_Input.H>

namespace {
    namespace Window {
        constexpr int width = 300;
        constexpr int height = 150;
    }

    namespace NicknameField {
        const char *label = "Nickname:"; 
        constexpr int width  = Window::width / 2;
        constexpr int x = Window::width / 2 - width / 2 + 60;
        constexpr int y = 1;
        constexpr int height = 20;
    }

    namespace PasswordField {
        const char *label = "Password:";
        constexpr int x = NicknameField::x;
        constexpr int y = NicknameField::height + 10; 
        constexpr int width  = NicknameField::width;
        constexpr int height = NicknameField::height;
    }

    namespace ConfirmField {
        const char *label = "Confirm password:";
        constexpr int x = PasswordField::x;
        constexpr int y = PasswordField::y + PasswordField::height + 10; 
        constexpr int width  = PasswordField::width;
        constexpr int height = PasswordField::height;
    }
}

Chat::RegistrationWindow::RegistrationWindow(const int x, const int y, const char *title)
    : Fl_Double_Window(x, y, Window::width, Window::height, title)
{
    const char *button_label = "Register";
    Fl_Group::begin();
    {
        m_nickname_field         = new Fl_Input(NicknameField::x, NicknameField::y, NicknameField::width, NicknameField::height, NicknameField::label);
        m_password_field         = new Fl_Secret_Input(PasswordField::x, PasswordField::y, PasswordField::width, PasswordField::height, PasswordField::label);
        m_confirm_password_field = new Fl_Secret_Input(ConfirmField::x, ConfirmField::y, ConfirmField::width, ConfirmField::height, ConfirmField::label);
    }
    Fl_Group::end();
}
