#include "options_window.hpp"
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>

using namespace Chat;

OptionsWindow::OptionsWindow(const int x, const int y, const int width, const int height, const char *title)
    : Fl_Double_Window(x, y, width, height, title)
{
    Fl_Group::begin();
    {
        const char *host_label   = "Host:";
        const char *port_label   = "Port:";
        const char *save_label   = "Save";
        const char *state_label  = "Disconnected"; 

        const int  input_x       = 40;
        const int  input_y       = 0;        
        const int  input_width   = width - 60;
        const int  input_height  = 25;  
        
        const int  box_width     = input_x / 2; 
        const int  box_x         = width / 2 - box_width / 2;
        const int  box_y         = input_x + input_height;

        const int  button_width  = input_width / 4;
        const int  button_x      = width / 2 - button_width / 2;
        const int  button_y      = height - input_height * 2;

        host             = new Fl_Input(input_x, input_y, input_width, input_height, host_label);
        port             = new Fl_Input(input_x, input_y + input_height, input_width, input_height, port_label);
        connection_state = new Fl_Box(box_x, box_y, box_width, input_height, state_label);
        save             = new Fl_Button(button_x, button_y, button_width, input_height, save_label);

        connection_state->labelcolor(FL_RED);
    }
    Fl_Group::end();
}
