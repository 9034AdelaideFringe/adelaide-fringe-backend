#include "controller/signup/SignupController.h"
#include "service/UserService.h"
#include "utils/uuid.h"
#include "pqxx/pqxx"
#include "utils/response.h"

SignupController::SignupController()
{
    REST_HANDLER(SignupController, "", "POST", signup);
}

response SignupController::signup(const request &req)
{
    auto x = json::load(req.body);
    if(!x || !x.has("email") || !x.has("password") || !x.has("name"))
    {
        CROW_LOG_INFO << std::string(x);
        return response(status::BAD_REQUEST);
    }

    
    const std::string email = x["email"].s();
    const std::string password = x["password"].s();
    const std::string name = x["name"].s();
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
    

    // json::wvalue data;

    // for (int i = 0; i < r.size(); i++)
    // {
    //     for (int j = 0; j < r.columns(); j++)
    //     {
    //         auto item = r.at(i).at(j);
    //         data[item.name()] = item.as<std::string>();
    //     }
    // }

    // json::wvalue res;


    return json::wvalue{{"message", "ok"}, {"data", data}};

    // PostgresConnection conn(s);

    // auto user = UserService::findUser(conn, email);
    // if(user.size() >= 2)
    // {
    //     return json::wvalue({{"error", "email already existed"}});
    // }

    // // uuid_t uuid;
    // // uuid_generate_random(uuid);

    // // char uuidStr[37] = {0};
    // // uuid_unparse(uuid, uuidStr);
    // std::string uuidStr = generateUUID();

    // std::stringstream ss; 
    // ss << R"(
    // insert into "Users" (id, email, password, "createdAt", "updatedAt") 
    // values (
    // )" 
    // << "'" << uuidStr << "'" << ", "
    // << "'" << email << "'" << ", " 
    // << "'" << hashedPassword << "'" << ", " 
    // << "NOW()" << ", " 
    // << "NOW()" << " " 
    // << ") returning id;";
    

    // std::vector<std::vector<std::string>> results; 

    // json::wvalue res;

    // if(conn.execute(ss.str(), results))
    // {
    //     res["message"] = "ok";
    //     res["data"]["userId"] = results[1][0];
    //     return res;
    // }
    // res["error"] = "failed";
    

    // return res;
}

/**
 * {
 *      "message": "ok",
 *      "data": {
 *          "userId": "xxxxxx"
 *      }    
 * }
 */