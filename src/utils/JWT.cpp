#include "utils/JWT.h"

std::string generateJWT()
{
    
    auto token = jwt::create()
    .set_type("JWS")
    .set_issuer("auth0")
    .set_payload_claim("sample", jwt::claim(std::string("test")))
    .sign(jwt::algorithm::hs256{"secret"});
    return token;
}