#include "controller/ticket_type/TicketTypeController.h"
#include "utils/Singleton.h"
#include "utils/Config.h"
#include <pqxx/pqxx>
#include <string>

TicketTypeController::TicketTypeController() : bp_("ticket-type")
{
    getBlueprint().register_blueprint(bp_);

    CROW_BP_ROUTE(bp_, "").methods("GET"_method)([this](const request& req){
        return this->getTicketTypes(req);
    });
    CROW_BP_ROUTE(bp_, "/<string>").methods("GET"_method)([this](const request& req, const string& id){
        return this->getTicketTypeById(req, id);
    });
}

response TicketTypeController::getTicketTypes(const request &req)
{
    pqxx::connection conn{Config::get("database")};

    pqxx::work w{conn};

    string query = R"(
        select COALESCE(
            json_agg(
                json_build_object(
                    'ticket_type_id', ticket_type_id,
                    'event_id', event_id,
                    'name', name,
                    'description', description,
                    'price', price,
                    'available_quantity', available_quantity,
                    'created_at', created_at,
                    'updated_at', updated_at
                )
            ), '[]'::json
        )
            from "tickettypes"
    )";

    auto r = w.exec(query);
    w.commit();

    auto data = r[0][0].c_str();
    json::wvalue res{{"message", "ok"}, {"data", json::load(data)}};
    CROW_LOG_INFO << res.dump();

    return res;
}

response TicketTypeController::getTicketTypeById(const request &req, const string& id)
{
    pqxx::connection conn{Config::get("database")};

    pqxx::work w{conn};

    string query = R"(
        select COALESCE(
            json_agg(
                row_to_json(tickettypes)
            ),
            '[]'::json

        )
            from "tickettypes" where "event_id" = $1
    )";

    auto r = w.exec_params(query, id);
    w.commit();

    auto data = r[0][0].c_str();
    json::wvalue res{{"message", "ok"}, {"data", json::load(data)}};
    CROW_LOG_INFO << res.dump();

    return res;
}
