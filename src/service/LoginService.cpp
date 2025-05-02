#include "service/LoginService.h"
#include <chrono>
#include <utils/PostgresConnection.h>
#include <service/UserService.h>

std::string LoginService::generateJWTForUser(const crow::json::wvalue& user)
{
    return UserService::generateJWTForUser(user);
}

bool LoginService::updateLastLogin(const PostgresConnection& conn, const std::string &id)
{
    return UserService::updateLastLogin(conn, id);
}

std::vector<std::vector<std::string>> LoginService::findUser(const PostgresConnection& conn, const std::string& email, const std::string& password)
{
    return UserService::findUser(conn, email, password);
}