#include "database.hpp"
#include "server.hpp"
#include "sha.hpp"
#include "sqlite3.h"
#include <iostream>
#include <memory>
#include <ostream>
int main()
{
    try {
        const char *database_name = "server.db";
        const char *port = "3490";

        auto database    = std::make_unique<Chat::Database>(database_name);
        auto server      = Chat::Server(port, std::move(database));

        return server.run(); 
    } catch (std::exception &ex) {
        std::cout << ex.what(); 
        return 1;
    }
//    const char *input = "sdfkafd";
//    SHA1 checksum;
//    checksum.update(input);
//    const auto output = checksum.final();
//    std::cout << "Password: " << output << std::endl;
}
