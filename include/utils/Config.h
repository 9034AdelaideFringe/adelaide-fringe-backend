#pragma once

#include "crow.h"
#include <string>
#include <fstream>
#include <iostream>


class Config
{
public:

    static void loadConfig(const std::string& path);
    // Config(const std::string& path);

    static bool has(const std::string& s);

    static std::string get(const std::string& s);

    // static std::string 
    
private:

    static crow::json::rvalue config_;
};
