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