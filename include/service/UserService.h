#pragma once

#include <vector>
#include <string>
#include "utils/PostgresConnection.h"

class UserService
{
public:
    static std::vector<std::vector<std::string>> findUser(const PostgresConnection &conn, const std::string &email);
};
