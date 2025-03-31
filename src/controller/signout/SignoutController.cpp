#include "controller/signout/SignoutController.h"
#include "service/UserService.h"

SignoutController::SignoutController()
{
    REST_HANDLER(SignoutController, "", "GET", signout);
}

response SignoutController::signout(const request &req)
{
    auto headers = req.headers;
    // headers.
    auto cookie = req.get_header_value("Cookie");
    CROW_LOG_INFO << cookie;

    int eqIndex = cookie.find('=');
    auto jwtString = cookie.substr(eqIndex + 1);
    auto decoded = UserService::decodeJWT(jwtString);
    response res;

/*     const std::string& secret = Config::get("JWTSecret");
    auto token = jwt::create()
    .set_type("JWT")
    .set_issuer("lxc")
    .set_payload_claim("id", jwt::claim(crow::json::load(user["id"].dump()).s()))
    .set_payload_claim("role", jwt::claim(crow::json::load(user["role"].dump()).s()))
    .set_payload_claim("expire", jwt::claim(std::to_string(expiryTimestamp)))
    .sign(jwt::algorithm::hs256{secret});
    return token; */

    // auto verifier = jwt::verify().with_issuer("clx")
    // log the user out!
    res.set_header("Set-Cookie", "jwt=; HttpOnly; Secure; Path=/; SameSite=Strict; Expires=Thu, 01 Jan 1970 00:00:00 GMT");

    return json::wvalue{{"message", "ok"}};
}
