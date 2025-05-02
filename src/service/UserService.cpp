#include "service/UserService.h"
#include <string>
#include <utils/Config.h>
// #include "UserService.h"

std::vector<std::vector<std::string>> UserService::findUser(const PostgresConnection &conn, const std::string &email)
{
    std::string query = R"(
        select * from "Users" where email = ')" + email + "'";
    return conn.query(query);
}

std::string UserService::generateJWTForUser(const crow::json::wvalue& user)
{
    auto now = std::chrono::system_clock::now();
    auto expiry = now + std::chrono::hours(24 * 30);
    auto expiryTimestamp = std::chrono::duration_cast<std::chrono::seconds>(expiry.time_since_epoch()).count();
    
    const std::string& secret = Config::get("JWTSecret");
    auto token = jwt::create()
    .set_type("JWT")
    .set_issuer("lxc")
    .set_payload_claim("id", jwt::claim(crow::json::load(user["id"].dump()).s()))
    .set_payload_claim("role", jwt::claim(crow::json::load(user["role"].dump()).s()))
    .set_payload_claim("expire", jwt::claim(std::to_string(expiryTimestamp)))
    .sign(jwt::algorithm::hs256{secret});
    return token;
}

jwt::decoded_jwt<jwt::traits::kazuho_picojson> UserService::decodeJWT(const std::string &token)
{
    auto result = jwt::decode(token);
    return result;
}

bool UserService::updateLastLogin(const PostgresConnection &conn, const std::string &id)
{

    auto query = R"(
        update "Users" set "lastLogin" = NOW() where id = ')" + id + "'";

    return conn.execute(query);
}

std::vector<std::vector<std::string>> UserService::findUser(const PostgresConnection& conn, const std::string& email, const std::string& password)
{
    std::string query = R"(
        select * from "Users" where 
    )";

    std::stringstream ss;
    ss << query << "email = " << "'" << email << "'" << " and " << "password = " << "'" << password << "';";

    return conn.query(ss.str());
}