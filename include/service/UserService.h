#pragma once

#include <vector>
#include <string>
#include "utils/PostgresConnection.h"
#include "jwt-cpp/jwt.h"


class UserService
{
public:
    static std::vector<std::vector<std::string>> findUser(const PostgresConnection &conn, const std::string &email);

    static std::string generateJWTForUser(const crow::json::wvalue& user);

    static jwt::decoded_jwt<jwt::traits::kazuho_picojson> decodeJWT(const std::string& token);

    static bool updateLastLogin(const PostgresConnection& conn, const std::string& id);

    static std::vector<std::vector<std::string>> findUser(const PostgresConnection& conn, const std::string& email, const std::string& password);
};
