#pragma once
#include <string>

struct sqlite3;

namespace Chat {
    class Database {
    public:
        Database(const char *database_name);
        ~Database();
    
        [[nodiscard]] bool isUserExists(const char *nickname);
        [[nodiscard]] bool addUser(std::string &nickname, std::string &password, std::string &salt);
    private:
        sqlite3 *m_connection = nullptr;
    };
}
