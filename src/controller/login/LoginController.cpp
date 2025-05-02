#include "controller/login/LoginController.h"
#include "service/LoginService.h"
#include "pqxx/pqxx"
#include "utils/JWT.h"

LoginController::LoginController()
{
    REST_HANDLER(LoginController, "", "POST", login);
}

response LoginController::login(const request &req)
{
    CROW_LOG_INFO << req.body;
    json::wvalue json;
    auto user = json::load(req.body);
    if(!user || !user.has("email") || !user.has("password"))
    {
        return json::wvalue{{"error", "missing email or password"}};
    }

    const std::string hashedPassword = sha256(user["password"].s());

    const std::string email = user["email"].s();


    pqxx::connection conn(Config::get("database"));


    pqxx::work w{conn};

    std::string query = R"(select * from "users" 
        where "email" = $1 AND "password" = $2
    )";

    pqxx::result r = w.exec_params(query,
                                   email, hashedPassword);


    w.commit();

    json::wvalue data;

    for (int i = 0; i < r.size(); i++)
    {
        for (int j = 0; j < r.columns(); j++)
        {
            auto item = r.at(i).at(j);
            data[item.name()] = item.as<std::string>();
        }
    }

    CROW_LOG_INFO << data.dump();


    std::string jwt = LoginService::generateJWTForUser(data);
    auto res = response(data);


    setCookie(res, jwt);

    return res;

}

response LoginController::jwt(const request &req)
{
    std::string jwt = generateJWT();
    CROW_LOG_INFO << jwt;

    return response("ok");
}
