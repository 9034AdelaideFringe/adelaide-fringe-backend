#include "controller/login/LoginController.h"
#include "service/LoginService.h"

LoginController::LoginController()
{
    REST_HANDLER(LoginController, "", "POST", login);
    // REST_HANDLER(LoginController, "/jwt", "GET", jwt);
}

response LoginController::login(const request &req)
{
    json::wvalue json;
    auto user = json::load(req.body);
    if(!user || !user.has("email") || !user.has("password"))
    {
        return response(crow::BAD_REQUEST);
    }

    const std::string hashedPassword = sha256(user["password"].s());

    const std::string email = user["email"].s();

    PostgresConnection conn(Config::get("database"));


    auto results = LoginService::findUser(conn, email, hashedPassword);
    if(results.size() == 1)
    {
        json["error"] = "not found";
        return response(json);
    }


    CROW_LOG_INFO << "results not empty";

    int rows = results.size();
    int cols = results.front().size();
    
    for(int i = 0; i < cols; i++)
    {
        const std::string& name = results.front().at(i);
        json[name] = results.at(1).at(i);
    }
    

    LoginService::updateLastLogin(conn, json["id"].dump());

    std::string jwt = LoginService::generateJWTForUser(json);
    auto res = response(json);
    res.set_header("Set-Cookie", "jwt=" + jwt + "; HttpOnly; Secure; Path=/; SameSite=Strict");
    return res;



}

response LoginController::jwt(const request &req)
{
    std::string jwt = generateJWT();
    CROW_LOG_INFO << jwt;

    return response("ok");
}
