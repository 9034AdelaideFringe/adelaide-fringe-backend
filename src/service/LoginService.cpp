#include "service/LoginService.h"
#include <chrono>
#include <utils/PostgresConnection.h>

std::string LoginService::generateJWTForUser(const crow::json::wvalue& user)
{
    auto now = std::chrono::system_clock::now();
    auto expiry = now + std::chrono::hours(24 * 30);
    auto expiryTimestamp = std::chrono::duration_cast<std::chrono::seconds>(expiry.time_since_epoch()).count();

    const std::string& secret = Config::get("JWTSecret");
    auto token = jwt::create()
    .set_type("JWT")
    .set_issuer("lxc")
    .set_payload_claim("id", jwt::claim(user["id"].dump()))
    .set_payload_claim("role", jwt::claim(user["role"].dump()))
    .set_payload_claim("expire", jwt::claim(std::to_string(expiryTimestamp)))
    .sign(jwt::algorithm::hs256{secret});
    return token;
}

bool LoginService::updateLastLogin(const PostgresConnection& conn, const std::string &id)
{

    auto query = R"(
        update "Users" set "lastLogin" = NOW() where id = '
    )" + id + "'";

    return conn.execute(query);
}
std::vector<std::vector<std::string>> LoginService::findUser(const PostgresConnection& conn, const std::string& email, const std::string& password)
{
    std::string query = R"(
        select * from "Users" where 
    )";

    std::stringstream ss;
    ss << query << "email = " << "'" << email << "'" << " and " << "password = " << "'" << password << "';";

    return conn.query(ss.str());
}