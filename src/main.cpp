#include "server.hpp"
#include <iostream>
#include <memory>
int main()
{
    const char *port = "3490";
    std::unique_ptr<Chat::Server> server; 
    try {
        server = std::make_unique<Chat::Server>(port);
        return server->run();
    } catch(std::exception &ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }
}
