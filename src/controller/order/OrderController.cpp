#include "controller/order/OrderController.h"
#include "utils/Singleton.h"
#include <pqxx/pqxx>
#include "utils/Config.h"

OrderController::OrderController() : bp_("order")
{
    getBlueprint().register_blueprint(bp_);

    CROW_BP_ROUTE(bp_, "").methods("GET"_method)([this](const request& req){
        return this->getAllOrders(req);
    });

    CROW_BP_ROUTE(bp_, "/<string>").methods("GET"_method)([this](const request& req, const string& id){
        return this->getOrderById(req, id);
    });

    CROW_BP_ROUTE(bp_, "").methods("POST"_method)([this](const request& req){
        return this->updateOrder(req);
    });

    CROW_BP_ROUTE(bp_, "").methods("DELETE"_method)([this](const request& req){
        return this->deleteOrder(req);
    });

    CROW_BP_ROUTE(bp_, "/userid/<string>").methods("GET"_method)([this](const request& req, const string& id){
        return this->getOrdersByUserId(req, id);
    });

    
}

response OrderController::getAllOrders(const request &req)
{
    pqxx::connection conn(Config::get("database"));
    pqxx::work w{conn};

    string query = R"(
        SELECT json_agg(t) FROM (SELECT * FROM orders) AS t;
    )";

    auto r = w.exec(query);

    json::wvalue res{{"message", "ok"}};

    if (r.empty())
    {
        res["data"] = json::wvalue::list();
    }
    else
    {
        auto dataStr = r[0][0].c_str();
        auto data = json::load(dataStr);
        res["data"] = data;
    }

    CROW_LOG_INFO << res.dump();

    return res;
}

response OrderController::getOrderById(const request &req, const string &id)
{
    CROW_LOG_INFO << id;
    pqxx::connection conn(Config::get("database"));
    pqxx::work w{conn};

    string query = R"(
        SELECT json_agg(t) FROM (SELECT * FROM orders where "order_id" = $1) AS t;
    )";

    auto r = w.exec_params(query, id);

    json::wvalue res{{"message", "ok"}};

    if (r.empty())
    {
        res["data"] = json::wvalue::list();
    }
    else
    {
        auto dataStr = r[0][0].c_str();
        auto data = json::load(dataStr);
        res["data"] = data;
    }

    CROW_LOG_INFO << res.dump();

    return res;
}

response OrderController::updateOrder(const request &req)
{
    CROW_LOG_INFO << req.body;
    auto body = json::load(req.body);
    string status(body["status"].s());
    string order_date(body["order_date"].s());
    string total_amount(body["total_amount"].s());
    string user_id(body["user_id"].s());
    string order_id(body["order_id"].s());

    pqxx::connection conn(Config::get("database"));
    pqxx::work w{conn};

    string query = R"(
        with updated as (update "orders"
        set
        "user_id" = $2,
        "total_amount" = $3,
        "order_date" = $4,
        "status" = $5
        where "order_id" = $1
        returning *)
        select json_agg(updated) from updated
    )";

    auto r = w.exec_params(query, order_id, user_id, total_amount, order_date, status);

    json::wvalue res{{"message", "ok"}};

    if (r.empty())
    {
        res["data"] = json::wvalue::list();
    }
    else
    {
        auto dataStr = r[0][0].c_str();
        auto data = json::load(dataStr);
        res["data"] = data;
    }

    CROW_LOG_INFO << res.dump();

    return res;
}

response OrderController::deleteOrder(const request &req)
{
    CROW_LOG_INFO << req.body;
    auto body = json::load(req.body);
    string order_id(body["order_id"].s());

    pqxx::connection conn(Config::get("database"));
    pqxx::work w{conn};

    string query = R"(
        with deleted as (
        delete from "orders" where "order_id" = $1
        returning *)
        select json_agg(deleted) from deleted
    )";

    auto r = w.exec_params(query, order_id);

    json::wvalue res{{"message", "ok"}};

    if (r.empty())
    {
        res["data"] = json::wvalue::list();
    }
    else
    {
        auto dataStr = r[0][0].c_str();
        auto data = json::load(dataStr);
        res["data"] = data;
    }

    CROW_LOG_INFO << res.dump();

    return res;
}

response OrderController::getOrdersByUserId(const request &req, const string &id)
{
    CROW_LOG_INFO << id;
    pqxx::connection conn(Config::get("database"));
    pqxx::work w{conn};

    string query = R"(
        SELECT json_agg(t) FROM (SELECT * FROM orders where "user_id" = $1) AS t;
    )";

    auto r = w.exec_params(query, id);

    json::wvalue res{{"message", "ok"}};

    if (r.empty())
    {
        res["data"] = json::wvalue::list();
    }
    else
    {
        auto dataStr = r[0][0].c_str();
        auto data = json::load(dataStr);
        res["data"] = data;
    }

    CROW_LOG_INFO << res.dump();

    return res;
}