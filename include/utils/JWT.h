#pragma once

#include "jwt-cpp/jwt.h"
#include <string>
#include "crow.h"

using namespace std;

std::string generateJWT();

jwt::decoded_jwt<jwt::traits::kazuho_picojson> decodeJWT(const std::string& token);

bool verifyJWT(const jwt::decoded_jwt<jwt::traits::kazuho_picojson>& decoded, const std::string& role);


void setCookie(crow::response& res, const string& jwt);