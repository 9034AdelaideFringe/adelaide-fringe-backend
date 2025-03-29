#include "controller/signup/SignupController.h"
#include "service/UserService.h"

SignupController::SignupController()
{
    REST_HANDLER(SignupController, "", "POST", signup);
}

response SignupController::signup(const request &req)
{
    auto x = json::load(req.body);
    if(!x || !x.has("email") || !x.has("password"))
    {
        CROW_LOG_INFO << x.s();
        return response(status::BAD_REQUEST);
    }

    
    const std::string email = x["email"].s();
    const std::string password = x["password"].s();

    const std::string hashedPassword = sha256(password);

    const std::string s = Config::get("database");
    PostgresConnection conn(s);

    auto user = UserService::findUser(conn, email);
    if(user.size() >= 2)
    {
        return json::wvalue({{"error", "email already existed"}});
    }

    uuid_t uuid;
    uuid_generate_random(uuid);

    char uuidStr[37] = {0};
    uuid_unparse(uuid, uuidStr);

    std::stringstream ss; 
    ss << R"(
    insert into "Users" (id, email, password, "createdAt", "updatedAt") 
    values (
    )" 
    << "'" << uuidStr << "'" << ", "
    << "'" << email << "'" << ", " 
    << "'" << hashedPassword << "'" << ", " 
    << "NOW()" << ", " 
    << "NOW()" << " " 
    << ") returning id;";
    

    std::vector<std::vector<std::string>> results; 

    json::wvalue res;

    if(conn.execute(ss.str(), results))
    {
        res["message"] = "ok";
        res["data"]["userId"] = results[1][0];
        return res;
    }
    res["error"] = "failed";
    

    return res;
}

/**
 * {
 *      "message": "ok",
 *      "data": {
 *          "userId": "xxxxxx"
 *      }    
 * }
 */