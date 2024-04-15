#include "client.hpp"
#include <iostream>

int main()
{
    try {
        Chat::Client client;
        return client.run();
    } catch(std::exception &ex) {
        std::cerr << ex.what();
        return 1;
    }
}
