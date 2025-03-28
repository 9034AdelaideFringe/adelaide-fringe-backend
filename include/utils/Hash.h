#pragma once

#include <openssl/sha.h>
#include <string>
#include <sstream>
#include <iomanip>

std::string sha256(const std::string& s);