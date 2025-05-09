#include "controller/signup/SignupController.h"
#include "service/UserService.h"
#include "utils/uuid.h"
#include "pqxx/pqxx"
#include "utils/response.h"
#include <string>



SignupController::SignupController() : bp_("signup")
{
    getBlueprint().register_blueprint(bp_);
    CROW_BP_ROUTE(bp_, "").methods("POST"_method)([this](const request& req){
        return this->signup(req);
    });
    // REST_HANDLER(SignupController, "", "POST", signup);
}

response SignupController::signup(const request &req)
{
    auto body = json::load(req.body);
    CROW_LOG_INFO << body;
    if(!body || !body.has("email") || !body.has("password") || !body.has("name"))
    {
        return json::wvalue{{"error", "missing fields"}};
    }

    
    const std::string email = body["email"].s();
    const std::string password = body["password"].s();
    const std::string name = body["name"].s();
    const std::string hashedPassword = sha256(password);
    std::string id = generateUUID();


    pqxx::connection conn(Config::get("database"));


    pqxx::work w{conn};

    std::string query = R"(insert into "users" 
    (user_id, email, password, name) 
        values ($1, $2, $3, $4)
        returning *;
    )";
    pqxx::result r = w.exec_params(query, id, email, hashedPassword, name);

    w.commit();

    auto data = resultsToJSON(r);
    

    return json::wvalue{{"message", "ok"}, {"data", data}};

}

/**
 * {
 *      "message": "ok",
 *      "data": {
 *          "userId": "xxxxxx"
 *      }    
 * }
 */