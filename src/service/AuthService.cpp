#include "service/AuthService.h"
#include "service/UserService.h"
#include <utils/Config.h>
#include <exception>

using namespace std;

string AuthService::authJWT(const request &req)
{
    auto cookieHeader = req.get_header_value("Cookie");
    std::string jwt;

    std::string tokenPrefix = "jwt=";
    auto pos = cookieHeader.find(tokenPrefix);
    if (pos != std::string::npos)
    {
        auto end = cookieHeader.find(';', pos);
        jwt = cookieHeader.substr(pos + tokenPrefix.size(), end - pos - tokenPrefix.size());
    }

    string id;

    try
    {
        auto decoded = UserService::decodeJWT(jwt);
        auto idClaim = decoded.get_payload_claim("id");
        id = idClaim.as_string();
    }
    catch(const invalid_argument& e)
    {
        throw invalid_argument("user not login");
    }
    
    return id;
}

std::string AuthService::generateJWTForUser(const crow::json::wvalue& user)
{
    auto now = std::chrono::system_clock::now();
    auto expiry = now + std::chrono::hours(24 * 30);
    string dumpId = user["user_id"].dump();
    string id = json::load(dumpId).s();
    string dumpRole = user["role"].dump();
    string role = json::load(dumpRole).s();
    
    const std::string& secret = Config::get("JWTSecret");
    auto token = jwt::create()
    .set_type("JWT")
    .set_issuer("lxc")
    .set_expires_at(expiry)
    .set_payload_claim("id", jwt::claim(id))
    .set_payload_claim("role", jwt::claim(role))
    .sign(jwt::algorithm::hs256{secret});
    return token;
}

void AuthService::setCookie(crow::response &res, const string& jwt)
{
    res.set_header("Set-Cookie", "jwt=" + jwt + "; HttpOnly; Secure; Path=/; SameSite=Strict");
}

jwt::decoded_jwt<jwt::traits::kazuho_picojson> AuthService::decodeJWT(const string& jwt)
{
    auto result = jwt::decode(jwt);
    return result;
}

string AuthService::authAdmin(const request &req)
{
    auto cookieHeader = req.get_header_value("Cookie");
    std::string jwt;

    std::string tokenPrefix = "jwt=";
    auto pos = cookieHeader.find(tokenPrefix);
    if (pos != std::string::npos)
    {
        auto end = cookieHeader.find(';', pos);
        jwt = cookieHeader.substr(pos + tokenPrefix.size(), end - pos - tokenPrefix.size());
    }

    string id;
    string role;

    try
    {
        auto decoded = UserService::decodeJWT(jwt);
        auto idClaim = decoded.get_payload_claim("id");
        auto roleClaim = decoded.get_payload_claim("role");
        id = idClaim.as_string();
        role = roleClaim.as_string();
        if(role != "ADMIN")
        {
            throw invalid_argument("user isn't admin");
        }
    }
    catch(const invalid_argument& e)
    {
        if(e.what() != "")
        {
            throw e;
        }
        throw invalid_argument("user not login");
    }
    
    return id;
}