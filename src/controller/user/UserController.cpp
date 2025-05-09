#include "controller/user/UserController.h"
#include <pqxx/connection.hxx>
#include <utils/Singleton.h>
#include <pqxx/pqxx>
#include <utils/Config.h>
#include <string>
#include "service/AuthService.h"
#include "utils/response.h"
#include "utils/Hash.h"

using namespace std;

UserController::UserController() : bp_("user")
{
    getBlueprint().register_blueprint(bp_);
        // getApp().register_blueprint(bp_);

    CROW_BP_ROUTE(bp_, "").methods("GET"_method)([this](const request &req)
                                                 { return this->getUser(req); });
    
    CROW_BP_ROUTE(bp_, "").methods("POST"_method)([this](const request &req)
                                                 { return this->updateUser(req); });
    
}

response UserController::updateUser(const request &req)
{
    string id = AuthService::authJWT(req);

    json::rvalue body = json::load(req.body);

    if(!body.has("password") || !body.has("name"))
    {
        return json::wvalue{{"error", "missing fields"}};
    }

    string password = body["password"].s();
    string name = body["name"].s();
    string hashedPassword = sha256(password);

    pqxx::connection conn{Config::get("database")};
    pqxx::work w{conn};

    std::string query = R"(
        update "users" 
        set 
        "password" = $1,
        "name" = $2
        where "user_id" = $3
        returning *;
    )";

    auto r = w.exec_params(query, hashedPassword, name, id);

    w.commit();

    auto data = resultsToJSON(r);
    data[0]["password"] = password;

    return json::wvalue{{"message", "ok"}, {"data", data}};
}

response UserController::getUser(const request &req)
{
    string id = AuthService::authJWT(req);
    CROW_LOG_INFO << id;

    pqxx::connection conn{Config::get("database")};
    pqxx::work w{conn};

    std::string query = R"(
        select * from "users"
        where "user_id" = $1
    )";

    auto r = w.exec_params(query, id);

    w.commit();

    auto data = resultsToJSON(r);

    return json::wvalue{{"message", "ok"}, {"data", data}};
}
