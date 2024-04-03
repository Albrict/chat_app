#include "client.hpp"
#include <iostream>
#include <memory>
int main()
{
    const int  window_width  = 1024;
    const int  window_height = 768;
    const char *window_title = "Chat client";
    
    std::unique_ptr<Chat::Client> client;  
    try {
        client = std::make_unique<Chat::Client>(window_width, window_height, window_title);
    } catch(std::exception &ex) {
        std::cerr << ex.what();
        return 1;
    }
    return client->run();
}
