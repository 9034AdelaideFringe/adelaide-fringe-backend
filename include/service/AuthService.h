#pragma once

#include "utils/PostgresConnection.h"
#include <string>
#include "crow.h"
#include "jwt-cpp/jwt.h"

using namespace crow;
using namespace std;

class AuthService
{
public:
    static string generateJWTForUser(const json::wvalue &user);
    static string authJWT(const request &req);
    static string authAdmin(const request& req);
    static jwt::decoded_jwt<jwt::traits::kazuho_picojson> decodeJWT(const string& jwt);
    static void setCookie(response &res, const string &jwt);
    
};
