#pragma once

#include "jwt-cpp/jwt.h"
#include <string>

std::string generateJWT();

auto decodeJWT(const std::string& token);

auto verifyJWT(const auto& decoded, const std::string& role);