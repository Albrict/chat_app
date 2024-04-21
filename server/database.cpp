#include <filesystem>
#include <iostream>
#include <stdexcept>
#include "database.hpp"
#include "sqlite3.h"

namespace {
    inline bool createDatabase(sqlite3 **connection, const char *database_name);
    inline bool openDatabase(sqlite3 **connection, const char *database_name);
    inline void addQuotes(std::string &str);
    bool inline checkSQLresult(const int result);
}

Chat::Database::Database(const char *database_name)
{
    const char *error = "Can't create/open database\n";
    bool result = std::filesystem::exists(database_name);
    if (!result) // Database don't exist
        result = createDatabase(&m_connection, database_name);
    else
        result = openDatabase(&m_connection, database_name); 

    if (!result)
        throw std::runtime_error(error);
}

Chat::Database::~Database()
{
    sqlite3_close(m_connection);
}

bool Chat::Database::isUserExists(const char *nickname)
{
    bool result = false;
    std::string sql = "SELECT nickname FROM users WHERE nickname='";
    sql += nickname;
    sql.push_back('\'');

    auto callback = +[](void *result, int count, char **data, char **columns) -> int {
        if (count > 0) {
            bool *bool_result = static_cast<bool*>(result); 
            *bool_result = true;
            return 0;
        }
        return -1;
    };
    const int success = sqlite3_exec(m_connection, sql.c_str(), callback, &result, nullptr);
    if (success != SQLITE_OK)
        std::cerr << sqlite3_errstr(success);
    return result;
}

bool Chat::Database::addUser(std::string &nickname, std::string &password, std::string &salt)
{
    std::string sql = "INSERT INTO users(nickname, password, salt) VALUES(";
    addQuotes(nickname);
    addQuotes(password);
    addQuotes(salt);

    nickname.append(", ");
    password.append(", ");
    salt.append(");");

    sql += nickname + password + salt;
    const int result = sqlite3_exec(m_connection, sql.c_str(), nullptr, nullptr, nullptr);
    return checkSQLresult(result);
}

namespace {
    bool inline checkSQLresult(const int result)
    {
        if (result != SQLITE_OK) {
            sqlite3_errstr(result);
            return false;
        } else {
            return true;
        }
    }

    int inline createUserTable(sqlite3 *connection)
    {
        const char *create = "CREATE TABLE users ("
                             "id       INTEGER PRIMARY KEY," 
                             "nickname TEXT    NOT NULL," 
                             "password TEXT    NOT NULL,"
                             "salt     TEXT    NOT NULL);";
        const int result = sqlite3_exec(connection, create, nullptr, nullptr, nullptr);
        return checkSQLresult(result);
    }

    inline bool createDatabase(sqlite3 **connection, const char *database_name)
    {
        const int result = sqlite3_open(database_name, connection);
        if (checkSQLresult(result)) {
            return createUserTable(*connection);
        } else {
            return false;
        }
    }

    inline bool openDatabase(sqlite3 **connection, const char *database_name)
    {
        const int result = sqlite3_open(database_name, connection);
        return checkSQLresult(result);
    }

    inline void addQuotes(std::string &str)
    {
        str.insert(0, "\'");
        str.push_back('\'');
    }
}
