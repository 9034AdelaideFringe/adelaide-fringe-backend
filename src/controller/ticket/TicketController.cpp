#include "controller/ticket/TicketController.h"
#include <pqxx/pqxx>
#include "utils/response.h"
#include "utils/Config.h"
#include "utils/uuid.h"
#include "utils/response.h"
#include "utils/Singleton.h"
#include "service/UserService.h"
#include "service/UploadService.h"
#include "middlewares/AuthMiddleware.h"

TicketController::TicketController() : bp_("ticket")
{
    getBlueprint().register_blueprint(bp_);
    // bp_.CROW_MIDDLEWARES(getApp(), AuthMiddleware);

    CROW_BP_ROUTE(bp_, "").methods("GET"_method)([this](const request &req)
                                                 { return this->getAllTickets(req); });
    // CROW_BP_ROUTE(bp_, "").methods("POST"_method)([this](const request &req)
    //                                               { return this->createTicket(req); });
    CROW_BP_ROUTE(bp_, "").methods("PUT"_method)([this](const request &req)
                                                 { return this->updateTicket(req); });
    CROW_BP_ROUTE(bp_, "/<string>").methods("GET"_method)([this](const request &req, const string &id)
                                                          { return this->getTicket(req, id); });
    CROW_BP_ROUTE(bp_, "").methods("DELETE"_method)([this](const request &req)
                                                    { return this->deleteTicket(req); });
    CROW_BP_ROUTE(bp_, "/userid/<string>").methods("GET"_method)([this](const request &req, const string &id)
                                                                 { return this->getTicketsByUserId(req, id); });
                                                                 
    CROW_BP_ROUTE(bp_, "/scan/<string>").methods("GET"_method)([this](const request &req, const string &id)
                                                                 { return this->scanTicket(req, id); });

}

response TicketController::getAllTickets(const request &req)
{
    string query = R"(
    SELECT COALESCE(json_agg(data), '[]'::json) FROM (
            SELECT * FROM "tickets"
        ) data
    )";

    pqxx::connection conn{Config::get("database")};
    pqxx::work w{conn};

    auto r = w.exec(query);

    w.commit();

    CROW_LOG_INFO << r[0][0].c_str();

    auto data = json::load(r[0][0].c_str());

    return json::wvalue{{"message", "ok"}, {"data", data}};
}

// response TicketController::createTicket(const request &req)
// {
//     auto body = json::load(req.body);
//     multipart::message_view multipartMessage(req);
//     string order_id(multipartMessage.get_part_by_name("order_id").body);
//     string ticket_type_id(multipartMessage.get_part_by_name("ticket_type_id").body);
//     string event_id(multipartMessage.get_part_by_name("event_id").body);

//     multipart::part_view qr_codePart = multipartMessage.get_part_by_name("qr_code");
//     string qr_code = UploadService::uploadFile(multipart::part_view(qr_codePart));

//     string status(multipartMessage.get_part_by_name("status").body);
//     string issue_date(multipartMessage.get_part_by_name("issue_date").body);
//     string expiry_date(multipartMessage.get_part_by_name("expiry_date").body);
//     string last_refund_date(multipartMessage.get_part_by_name("last_refund_date").body);

//     string ticket_id = generateUUID();

//     string query = R"(
//     with inserted as(
//         insert into "tickets"
//         ("ticket_id", "order_id", "ticket_type_id", "event_id", "qr_code", "status", "issue_date", "expiry_date", "last_refund_date")
//         values ($1, $2, $3, $4, $5, $6, $7, $8, $9)
//         returning *)
//         select row_to_json(inserted) from inserted
//     )";

//     pqxx::connection conn{Config::get("database")};
//     pqxx::work w{conn};

//     auto r = w.exec_params(query,
//         ticket_id,
//         order_id,
//         ticket_type_id,
//         event_id,
//         qr_code,
//         status,
//         issue_date,
//         expiry_date,
//         last_refund_date
//     );

//     w.commit();

//     auto data = json::load(r[0][0].c_str());

//     return json::wvalue{{"message", "ok"}, {"data", data}};

// }

response TicketController::updateTicket(const request &req)
{
    auto body = json::load(req.body);
    CROW_LOG_INFO << req.body;

    string ticket_id(body["ticket_id"].s());
    string order_id(body["order_id"].s());
    string ticket_type_id(body["ticket_type_id"].s());
    string event_id(body["event_id"].s());
    string status(body["status"].s());
    string issue_date(body["issue_date"].s());
    string expiry_date(body["expiry_date"].s());
    string last_refund_date(body["last_refund_date"].s());
    string scan_date(body["scan_date"].s());

    string query = R"(
        WITH updated AS (
            UPDATE "tickets"
            SET "order_id" = $2,
                "ticket_type_id" = $3,
                "event_id" = $4,
                "status" = $5,
                "issue_date" = $6,
                "expiry_date" = $7,
                "last_refund_date" = $8,
                "scan_date" = $9
            WHERE "ticket_id" = $1 
            RETURNING *
        )
        SELECT json_agg(updated) FROM updated;
    )";

    pqxx::connection conn{Config::get("database")};
    pqxx::work w{conn};
    auto r = w.exec_params(query, ticket_id, order_id, ticket_type_id, event_id, status, issue_date, expiry_date, last_refund_date, scan_date);
    w.commit();

    auto dataStr = r[0][0].c_str();
    auto data = json::load(dataStr);
    CROW_LOG_INFO << dataStr;

    return json::wvalue{{"message", "ok"}, {"data", data}};
}

response TicketController::getTicket(const request &req, const string &id)
{
    CROW_LOG_INFO << id;
    pqxx::connection conn{Config::get("database")};
    pqxx::work w{conn};

    string query = R"(
        SELECT json_agg(t) 
        FROM (
            SELECT * FROM "tickets" WHERE "ticket_id" = $1
        ) AS t;
    )";

    auto r = w.exec_params(query, id);
    w.commit();
    auto dataStr = r[0][0].c_str();
    auto data = json::load(dataStr);
    CROW_LOG_INFO << data;
    return json::wvalue{{"message", "ok"}, {"data", data}};
}

response TicketController::deleteTicket(const request &req)
{
    CROW_LOG_INFO << req.body;
    auto body = json::load(req.body);

    string id(body["ticket_id"].s());

    pqxx::connection conn{Config::get("database")};
    pqxx::work w{conn};

    string query = R"(
        with deleted as(
            delete from "tickets"
            where "ticket_id" = $1
            returning *
        )
            select json_agg(deleted) from deleted
    )";
    auto r = w.exec_params(query, id);
    w.commit();
    auto dataStr = r[0][0].c_str();
    auto data = json::load(dataStr);

    CROW_LOG_INFO << data;
    return json::wvalue{{"message", "ok"}, {"data", data}};
}

response TicketController::getTicketsByUserId(const request &req, const string &id)
{
    CROW_LOG_INFO << id;
    pqxx::connection conn{Config::get("database")};
    pqxx::work w{conn};

    string query = R"(
    SELECT COALESCE(json_agg(t), '[]'::json) 
    FROM (
        SELECT t.*, o.* 
        FROM "tickets" t 
        JOIN "orders" o ON t."order_id" = o."order_id" 
        WHERE o."user_id" = $1
    ) AS t;
)";

    auto r = w.exec_params(query, id);
    w.commit();

    auto data = json::wvalue();

    auto dataStr = r[0][0].c_str();

    data = json::load(dataStr);

    CROW_LOG_INFO << data.dump();
    return json::wvalue{{"message", "ok"}, {"data", data}};
}


response TicketController::scanTicket(const request &req, const string &id)
{
    CROW_LOG_INFO << id;
    pqxx::connection conn{Config::get("database")};
    pqxx::work w{conn};

    string query = R"(
        with updated as (
        update "tickets" set "status" = $2, "scan_date" = NOW() where "ticket_id" = $1
        returning *
        )
        select json_agg(updated) from updated
)";

    auto r = w.exec_params(query, id, "USED");
    w.commit();

    auto data = json::wvalue();

    auto dataStr = r[0][0].c_str();

    data = json::load(dataStr);

    CROW_LOG_INFO << data.dump();
    return json::wvalue{{"message", "ok"}, {"data", data}};
}