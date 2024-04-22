#pragma once
#include <string>

struct sqlite3;

namespace Chat {
    class Database {
    public:
        Database(const char *database_name);
        ~Database();
    
        [[nodiscard]] bool isUserExists(const std::string &nickname);
        [[nodiscard]] bool addUser(std::string &nickname, std::string &password, std::string &salt);
        [[nodiscard]] bool loginUser(const std::string &nickname, const std::string &password);
    private:
        sqlite3 *m_connection = nullptr;
    };
}
