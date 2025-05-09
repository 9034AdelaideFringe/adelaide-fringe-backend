#include "controller/cart/CartController.h"
#include <pqxx/pqxx>
#include "utils/response.h"
#include "utils/Config.h"
#include "service/AuthService.h"
#include <string>
#include "utils/Singleton.h"
#include "utils/uuid.h"

using namespace std;


CartController::CartController() : bp_("api", "cart")
{
    getApp().register_blueprint(bp_);

    CROW_BP_ROUTE(bp_, "/").methods("GET"_method)([this](const request& req)
    {
        return this->getCart(req);
    });
    CROW_BP_ROUTE(bp_, "/").methods("POST"_method)([this](const request& req)
    {
        return this->addToCart(req);
    });
    CROW_BP_ROUTE(bp_, "/").methods("PUT"_method)([this](const request& req)
    {
        return this->updateCart(req);
    });
    CROW_BP_ROUTE(bp_, "/").methods("DELETE"_method)([this](const request& req)
    {
        return this->removeFromCart(req);
    });


}


response CartController::getCart(const request &req)
{
    string id = AuthService::authJWT(req);


    pqxx::connection conn{Config::get("database")};
    pqxx::work w{conn};

    std::string query = R"(
        select * from "cart"
        where "user_id" = $1
    )";

    auto r = w.exec_params(query, id);

    w.commit();

    auto data = resultsToJSON(r);

    return json::wvalue{{"message", "ok"}, {"data", data}};
}

response CartController::addToCart(const request &req)
{
    auto body = json::load(req.body);
    string user_id = body["user_id"].s();
    string ticket_type_id = body["ticket_type_id"].s();
    string quantity = body["quantity"].s();
    if(user_id.empty() || ticket_type_id.empty() || quantity.empty())
    {
        return json::wvalue{{"error", "missing field"}};
    }

    auto cart_item_id = generateUUID();

    pqxx::connection conn{Config::get("database")};
    pqxx::work w{conn};

    std::string query = R"(
        INSERT INTO "cart" ("cart_item_id", "user_id", "ticket_type_id", "quantity", "added_at")
        VALUES ($1, $2, $3, $4, $5)
        RETURNING *
    )";

    auto r = w.exec_params(query, cart_item_id, user_id, ticket_type_id, quantity, "NOW()");

    w.commit();

    auto data = resultsToJSON(r);

    return json::wvalue{{"message", "ok"}, {"data", data}};
}


response CartController::updateCart(const request &req)
{
    auto body = json::load(req.body);
    string cart_item_id = body["cart_item_id"].s();
    string user_id = body["user_id"].s();
    string ticket_type_id = body["ticket_type_id"].s();
    string quantity = body["quantity"].s();
    if(user_id.empty() || ticket_type_id.empty() || quantity.empty() || cart_item_id.empty())
    {
        return json::wvalue{{"error", "missing field"}};
    }


    pqxx::connection conn{Config::get("database")};
    pqxx::work w{conn};

    std::string query = R"(
        UPDATE "cart"
        SET "user_id" = $2,
            "ticket_type_id" = $3,
            "quantity" = $4
        WHERE "cart_item_id" = $1
        RETURNING *
    )";

    auto r = w.exec_params(query, cart_item_id, user_id, ticket_type_id, quantity);

    w.commit();

    auto data = resultsToJSON(r);

    return json::wvalue{{"message", "ok"}, {"data", data}};
}

response CartController::removeFromCart(const request &req)
{
    auto body = json::load(req.body);
    string cart_item_id = body["cart_item_id"].s();
    if(cart_item_id.empty())
    {
        return json::wvalue{{"error", "missing field"}};
    }


    pqxx::connection conn{Config::get("database")};
    pqxx::work w{conn};

    std::string query = R"(
        DELETE FROM "cart"
        WHERE "cart_item_id" = $1
        RETURNING *
    )";

    auto r = w.exec_params(query, cart_item_id);

    w.commit();

    auto data = resultsToJSON(r);

    return json::wvalue{{"message", "ok"}, {"data", data}};
}

// response CartController::checkOut(const request &req)
// {
//     string userId = AuthService::authJWT(req);

//     auto body = json::load(req.body);


//     pqxx::connection conn{Config::get("database")};
//     pqxx::work w{conn};

//     std::string query = R"(
//         INSERT INTO "cart" ("cart_item_id", "user_id", "ticket_type_id", "quantity", "added_at")
//         VALUES ($1, $2, $3, $4, $5)
//         RETURNING *
//     )";

//     auto r = w.exec_params(query, cart_item_id, user_id, ticket_type_id, quantity, "NOW()");

//     w.commit();

//     auto data = resultsToJSON(r);

//     return json::wvalue{{"message", "ok"}, {"data", data}};
// }