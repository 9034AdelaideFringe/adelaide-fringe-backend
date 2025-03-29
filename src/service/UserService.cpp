#include "service/UserService.h"
#include <string>

std::vector<std::vector<std::string>> UserService::findUser(const PostgresConnection &conn, const std::string &email)
{
    std::string query = R"(
        select * from "Users" where email = ')" + email + "'";
    return conn.query(query);
}
