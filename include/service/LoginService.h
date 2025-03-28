#pragma once
#include <string>
#include "jwt-cpp/jwt.h"
#include "crow.h"
#include "utils/Config.h"
#include "utils/PostgresConnection.h"

class LoginService
{
public:
    static std::string generateJWTForUser(const crow::json::wvalue& user);

    static bool updateLastLogin(const PostgresConnection& conn, const std::string& id);

    static std::vector<std::vector<std::string>> findUser(const PostgresConnection& conn, const std::string& email, const std::string& password);
};
