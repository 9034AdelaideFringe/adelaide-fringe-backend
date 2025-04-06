#include "utils/JWT.h"
#include "jwt-cpp/jwt.h"
#include "utils/Config.h"

std::string generateJWT()
{

    auto token = jwt::create()
                     .set_type("JWS")
                     .set_issuer("auth0")
                     .set_payload_claim("sample", jwt::claim(std::string("test")))
                     .sign(jwt::algorithm::hs256{"secret"});
    return token;
}
auto decodeJWT(const std::string &token)
{
    auto decoded = jwt::decode(token);

    return decoded;
}

auto verifyJWT(const auto &decoded, const std::string& role)
{
    auto verifier = jwt::verify()
                        .with_issuer("clx")
                        .with_claim("role", jwt::claim(role))
                        .allow_algorithm(jwt::algorithm::hs256{Config::get("JWTSecrets")});
    try
    {
        verifier.verify(decoded);
    }
    catch(const std::exception& e)
    {
        return false;
    }
    return true;
}