#include "controller/login/LoginController.h"
#include "service/LoginService.h"
#include "pqxx/pqxx"
#include "utils/JWT.h"
#include "utils/response.h"
#include "utils/Singleton.h"
#include "service/AuthService.h"

LoginController::LoginController() : bp_("login")
{
    getBlueprint().register_blueprint(bp_);

    CROW_BP_ROUTE(bp_, "").methods("POST"_method)([this](const request &req)
                                                  { return this->login(req); });
}

response LoginController::login(const request &req)
{
    auto body = json::load(req.body);
    CROW_LOG_INFO << body;
    if (!body || !body.has("email") || !body.has("password"))
    {
        return json::wvalue{{"error", "missing email or password"}};
    }

    const string email = body["email"].s();
    const string hashedPassword = sha256(body["password"].s());

    CROW_LOG_INFO << string(body["password"].s());
    CROW_LOG_INFO << hashedPassword;

    pqxx::connection conn(Config::get("database"));

    pqxx::work w{conn};

    std::string query = R"(select * from "users" 
        where "email" = $1 AND "password" = $2
    )";

    auto r = w.exec_params(query,
                           email, hashedPassword);

    w.commit();

    auto data = resultsToJSON(r);
    CROW_LOG_INFO << data.dump();

    response res = json::wvalue{{"message", "ok"}, {"data", data}};

    auto jwt = AuthService::generateJWTForUser(data[0]);
    AuthService::setCookie(res, jwt);

    return res;
    
}

response LoginController::jwt(const request &req)
{
    std::string jwt = generateJWT();
    CROW_LOG_INFO << jwt;

    return response("ok");
}
