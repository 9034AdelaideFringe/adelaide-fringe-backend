#pragma once

#include "crow.h"
#include <string>
#include <functional>
#include <vector>
#include "utils/Singleton.h"

class Controller
{
public:
    using Handler = std::function<crow::response(const crow::request&)>;
    Controller();

protected:
    std::vector<std::pair<std::string, Handler>> handlers_;

    void registerAll();
};

// #define REST_CONTROLLER(path) inline constexpr const char* basePath = path;
#define REST_CONTROLLER(path) \
    static inline constexpr const char* basePath() { return path; }


#define REST_HANDLER(ClassName, path, httpMethod, function) \
    getApp().route_dynamic(std::string(basePath()) + path)    \
    .methods(httpMethod##_method)([this](const crow::request& req)   -> crow::response             \
{                                   \
    return this->function(req);                                \
});                             
