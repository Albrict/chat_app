#include "login_window.hpp"
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Secret_Input.H>
#include <FL/Fl_Box.H>
#include <FL/Fl.H>
#include <iostream>
#include <string>
#include "../utils/packet.hpp"

namespace {
    namespace Window {
        const char   *label  = "Login";
        constexpr int width  = 300;
        constexpr int height = 200;
    }
    
    namespace Widget {
        constexpr int width  = Window::width / 2;
        constexpr int height = 20;
    }

    namespace NicknameField {
        const char *label = "Nickname:"; 
        constexpr int x = Window::width / 2 - Widget::width / 2;
        constexpr int y = 1;
    }
    
    namespace PasswordField {
        const char *label = "Password:";
        constexpr int x = NicknameField::x;
        constexpr int y = Widget::height + 10; 
    }
    
    namespace RepeatPasswordField {
        const char *label = "Repeat password";
        constexpr int x = NicknameField::x;
        constexpr int y = PasswordField::y + Widget::height + 10;
    }

    namespace InfoBox {
        const char *incorrect_label    = "Incorrect password or nickname";
        const char *dont_exist_label   = "User does not exist";
        const char *illegal_symbols    = "Illegal symbols: /";
        const char *incorrect_password = "Incorrect password";
        const char *fields_empty       = "Fields are empty";

        constexpr int x = NicknameField::x;
        constexpr int y = RepeatPasswordField::y + Widget::height + 10; 
    }

    namespace LoginButton {
        const char *label = "Log in";
        constexpr int x = NicknameField::x;
        constexpr int y = InfoBox::y + Widget::height + 30; 
    }

    namespace SignUpButton {
        const char *label = "Sign Up";
        constexpr int x = LoginButton::x;
        constexpr int y = LoginButton::y + Widget::height + 10; 
    }

    namespace RegisterButton {
        const char *label = "Register";
        constexpr int x = LoginButton::x;
        constexpr int y = LoginButton::y; 
    }

    namespace BackButton {
        const char *label = "Back";
        constexpr int x = SignUpButton::x;
        constexpr int y = SignUpButton::y; 
    }
    
    [[nodiscard]] bool checkPasswordAndNicknameCorrectness(const std::string &nickname, const std::string &password, const std::string &repeat_password, Fl_Box *error_display);
}

Chat::LoginWindow::LoginWindow(const int x, const int y, const int connection_fd)
    : Fl_Double_Window(x, y, Window::width, Window::height, Window::label),
    m_connection_fd(connection_fd)
{
    Fl_Group::begin();
    {
        m_current_group = createLoginGroup(); 
    }
    Fl_Group::end();
}

Fl_Group *Chat::LoginWindow::createLoginGroup()
{
    auto group = new Fl_Group(0, 0, Window::width, Window::height);
    group->begin();
    {
        m_nickname_field = new Fl_Input(NicknameField::x, NicknameField::y, Widget::width, Widget::height, NicknameField::label);
        m_password_field = new Fl_Secret_Input(PasswordField::x, PasswordField::y, Widget::width, Widget::height, PasswordField::label);
        m_info_box              = new Fl_Box(InfoBox::x, InfoBox::y, Widget::width, Widget::height);

        auto login_button        = new Fl_Button(LoginButton::x, LoginButton::y, Widget::width, Widget::height, LoginButton::label);
        auto registration_button = new Fl_Button(SignUpButton::x, SignUpButton::y, Widget::width, Widget::height, SignUpButton::label);
        
        m_info_box->labelcolor(FL_RED);
        m_info_box->hide();
        login_button->callback(loginButtonCallback, this);
        registration_button->callback(signUpButtonCallback, this);
    }
    group->end();
    return group;
}

Fl_Group *Chat::LoginWindow::createRegistrationGroup()
{
    auto group = new Fl_Group(0, 0, Window::width, Window::height);
    group->begin();
    {
        m_nickname_field        = new Fl_Input(NicknameField::x + 50, NicknameField::y, Widget::width, Widget::height, NicknameField::label);
        m_password_field        = new Fl_Secret_Input(PasswordField::x + 50, PasswordField::y, Widget::width, Widget::height, PasswordField::label);
        m_repeat_password_field = new Fl_Secret_Input(RepeatPasswordField::x + 50, RepeatPasswordField::y, Widget::width, Widget::height, RepeatPasswordField::label);
        m_info_box              = new Fl_Box(InfoBox::x, InfoBox::y, Widget::width, Widget::height);
         
        auto register_button = new Fl_Button(RegisterButton::x, RegisterButton::y, Widget::width, Widget::height, RegisterButton::label);
        auto back_button     = new Fl_Button(BackButton::x, BackButton::y, Widget::width, Widget::height, BackButton::label);

        register_button->callback(registerButtonCallback, this);
        back_button->callback(backButtonCallback, this);
        m_info_box->labelcolor(FL_RED);
        m_info_box->hide();
    }
    group->end();
    return group;
}


void Chat::LoginWindow::loginButtonCallback(Fl_Widget *widget, void *data)
{
    auto login_window    = static_cast<Chat::LoginWindow*>(data);
    std::string nickname = login_window->m_nickname_field->value();
    std::string password = login_window->m_password_field->value();
    
    if (!checkPasswordAndNicknameCorrectness(nickname, password, password, login_window->m_info_box)) {
        login_window->m_info_box->show();
        return;
    }

    NetworkUtils::LoginPacket packet(nickname, password, NetworkUtils::LoginPacket::Type::LOGIN);
    bool result = packet.send(login_window->m_connection_fd);
    if (!result) {
        std::cerr << "Can't send message!\n";
    }
    if (login_window->m_info_box->visible())
        login_window->m_info_box->hide();
}

void Chat::LoginWindow::signUpButtonCallback(Fl_Widget *widget, void *data)
{
    auto login_window = static_cast<Chat::LoginWindow*>(data);
    Fl::delete_widget(login_window->m_current_group);

    login_window->m_current_group = login_window->createRegistrationGroup();
    login_window->add(login_window->m_current_group);
}

void Chat::LoginWindow::registerButtonCallback(Fl_Widget *widget, void *data)
{
    auto login_window       = static_cast<LoginWindow*>(data);
    std::string nickname    = login_window->m_nickname_field->value();
    std::string password    = login_window->m_password_field->value();
    std::string repeat_pass = login_window->m_repeat_password_field->value(); 
    
    if (!checkPasswordAndNicknameCorrectness(nickname, password, repeat_pass, login_window->m_info_box)) {
        login_window->m_info_box->show();
        return;
    }

    NetworkUtils::LoginPacket packet(nickname, password, NetworkUtils::LoginPacket::REGISTRATION);
    bool result  = packet.send(login_window->m_connection_fd);
    if (!result) {
        std::cerr << "Can't send message!\n";
    }
    if (login_window->m_info_box->visible())
        login_window->m_info_box->hide();
}

void Chat::LoginWindow::backButtonCallback(Fl_Widget *widget, void *data)
{
    auto login_window = static_cast<Chat::LoginWindow*>(data);
    Fl::delete_widget(login_window->m_current_group);

    login_window->m_current_group = login_window->createLoginGroup();
    login_window->add(login_window->m_current_group);
}


namespace {
    bool checkPasswordAndNicknameCorrectness(const std::string &nickname, const std::string &password, const std::string &repeat_password, Fl_Box *error_display)
    {
        if (nickname.empty() || password.empty() || repeat_password.empty()) {
            error_display->label(InfoBox::fields_empty);
            return false;
        }
        if (password != repeat_password) {
            error_display->label(InfoBox::incorrect_password);
            return false;
        }

        for (const auto &ch : nickname) {
            if (ch == '/') {
                error_display->label(InfoBox::illegal_symbols);
                return false;
            }
        }
        for (const auto &ch : password) {
            if (ch == '/') {
                error_display->label(InfoBox::illegal_symbols);
                return false;
            }
        }
        return true;
    }
}

