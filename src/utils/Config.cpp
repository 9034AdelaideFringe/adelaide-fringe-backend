#include "utils/Config.h"

crow::json::rvalue Config::config_;

bool Config::has(const std::string &s)
{
    return config_.has(s);
}

std::string Config::get(const std::string &s)
{
    if(has(s))
    {
        return config_[s].s();
    }
    return "";
}

void Config::loadConfig(const std::string &path)
{
    std::fstream fs(path, std::ios::in | std::ios::out);
    if (!fs)
    {
        throw std::runtime_error("Failed to open file: " + path);
    }

    std::stringstream buffer;
    buffer << fs.rdbuf();
    config_ = crow::json::load(buffer.str());
}
