#include "utils/JWT.h"
#include "jwt-cpp/jwt.h"
#include "utils/Config.h"

std::string generateJWT()
{

    auto token = jwt::create()
                     .set_type("JWS")
                     .set_issuer("clx")
                     .set_payload_claim("role", jwt::claim(std::string("test")))
                     .sign(jwt::algorithm::hs256{"secret"});
    return token;
}

jwt::decoded_jwt<jwt::traits::kazuho_picojson> decodeJWT(const std::string &token)
{
    auto decoded = jwt::decode(token);

    return decoded;
}

bool verifyJWT(const jwt::decoded_jwt<jwt::traits::kazuho_picojson> &decoded, const std::string& role)
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

void setCookie(crow::response &res, const string& jwt)
{
    res.set_header("Set-Cookie", "jwt=" + jwt + "; HttpOnly; Secure=false; Path=/; SameSite=Strict");
}