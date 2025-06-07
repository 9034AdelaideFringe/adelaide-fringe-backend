#include "controller/cart/CartController.h"
#include <pqxx/pqxx>
#include "utils/response.h"
#include "utils/Config.h"
#include "service/AuthService.h"
#include <string>
#include "utils/Singleton.h"
#include "utils/uuid.h"
#include "service/TicketService.h"

using namespace std;

CartController::CartController() : bp_("cart")
{
    getBlueprint().register_blueprint(bp_);

    CROW_BP_ROUTE(bp_, "/<string>").methods("GET"_method)([this](const request &req, const string &id)
                                                          { return this->getCartByUserId(req, id); });

    CROW_BP_ROUTE(bp_, "").methods("POST"_method)([this](const request &req)
                                                  { return this->addToCart(req); });

    CROW_BP_ROUTE(bp_, "").methods("PUT"_method)([this](const request &req)
                                                 { return this->updateCart(req); });

    CROW_BP_ROUTE(bp_, "").methods("DELETE"_method)([this](const request &req)
                                                    { return this->removeFromCart(req); });

    CROW_BP_ROUTE(bp_, "/<string>").methods("POST"_method)([this](const request &req, const string &id)
                                                           { return this->checkOut(req, id); });
}

response CartController::getCartByUserId(const request &req, const string &id)
{
    CROW_LOG_INFO << "Getting cart for user ID: " << id;
    pqxx::connection conn{Config::get("database")};
    pqxx::work w{conn};

    // 修改 SQL 查询以连接 cart, tickettypes, 和 events 表，并构建嵌套 JSON
    std::string query = R"(
        SELECT COALESCE(json_agg(
            json_build_object(
                'user_id', c.user_id,
                'seat', c.seat, 
                'ticket_type_details', json_build_object(
                    'ticket_type_id', tt.ticket_type_id,
                    'name', tt.name, -- 包含 tickettypes 的 name
                    'price', tt.price
                ),
                'event_details', json_build_object(
                    'event_id', e.event_id,
                    'title', e.title, -- 包含 events 的 title
                    'image', e.image, -- 包含 events 的 image
                    'date', e.date,   -- 包含 events 的 date
                    'time', e.time,   -- 包含 events 的 time
                    'venue', e.venue, -- 包含 events 的 venue
                    'end_time', e.end_time,
                    'status', e.status
                )
            )
        ), '[]'::json)
        FROM "cart" c
        JOIN "tickettypes" tt ON c."ticket_type_id" = tt."ticket_type_id" -- 加入 tickettypes 表
        JOIN "events" e ON tt."event_id" = e."event_id" -- 加入 events 表
        WHERE c."user_id" = $1;
    )";

    auto r = w.exec_params(query, id);

    w.commit();

    auto data = json::wvalue();

    // 从查询结果中加载 JSON 字符串
    // json_agg 返回一个包含单个 JSON 字符串的行和列
    if (!r.empty() && !r[0].empty() && !r[0][0].is_null()) {
        auto dataStr = r[0][0].c_str();
        data = json::load(dataStr);
    } else {
        // 如果查询结果为空或包含 NULL，返回一个空的 JSON 数组
        data = json::wvalue::list();
    }


    CROW_LOG_INFO << "Cart by user ID response: " << data.dump();
    return json::wvalue{{"message", "ok"}, {"data", data}};
}

response CartController::addToCart(const request &req)
{
    auto body = json::load(req.body);
    string user_id = body["user_id"].s();
    string ticket_type_id = body["ticket_type_id"].s();
    string quantity = body["quantity"].s();
    string seat = body["seat"].s();
    if (user_id.empty() || ticket_type_id.empty() || quantity.empty())
    {
        return json::wvalue{{"error", "missing field"}};
    }

    auto cart_item_id = generateUUID();

    pqxx::connection conn{Config::get("database")};
    pqxx::work w{conn};

    std::string query = R"(
        INSERT INTO "cart" ("cart_item_id", "user_id", "ticket_type_id", "quantity", "added_at", "seat")
        VALUES ($1, $2, $3, $4, $5, $6)
        RETURNING *
    )";

    auto r = w.exec_params(query, cart_item_id, user_id, ticket_type_id, quantity, "NOW()", seat);

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
    if (user_id.empty() || ticket_type_id.empty() || quantity.empty() || cart_item_id.empty())
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
    if (cart_item_id.empty())
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

response CartController::checkOut(const request &req, const string &id)
{

    pqxx::connection conn{Config::get("database")};
    pqxx::work w{conn};

    string query = R"(
        select tt.*, c.*
        from cart c
        join tickettypes tt on tt.ticket_type_id = c.ticket_type_id
        where c.user_id = $1
    )";

    auto r = w.exec_params(query, id);
    double totalAmount = 0;

    for (const pqxx::row &row : r)
    {
        int quantity = row["quantity"].as<int>();
        int available = row["available_quantity"].as<int>();
        double price = row["price"].as<double>();
        totalAmount += price * quantity;
        if (quantity > available)
        {
            throw runtime_error("available quantity not available");
        }
    }

    auto order_res = w.exec_params(R"(
    INSERT INTO orders (order_id, user_id, total_amount, order_date)
    VALUES (gen_random_uuid(), $1, $2, NOW())
    RETURNING *
)",
                                   id, totalAmount);

    string order_id = order_res[0]["order_id"].as<string>();

    query = R"(
    UPDATE tickettypes t
    SET available_quantity = t.available_quantity - c.total_quantity
    FROM (
      SELECT ticket_type_id, SUM(quantity) AS total_quantity
      FROM cart
      WHERE user_id = $1
      GROUP BY ticket_type_id
    ) c
    WHERE t.ticket_type_id = c.ticket_type_id;
    )";

    r = w.exec_params(query, id);

    // 5. 插入所有票（关键点：用 generate_series）
    r = w.exec_params(R"(
        INSERT INTO tickets (ticket_id, order_id, ticket_type_id, event_id, issue_date, expiry_date, last_refund_date， seat)
        SELECT
            gen_random_uuid(),
            $1,
            c.ticket_type_id,
            tt.event_id,
            NOW(),
            NOW() + INTERVAL '7 days',
            NOW() + INTERVAL '7 days',
            c.seat
        FROM cart c
        JOIN tickettypes tt ON c.ticket_type_id = tt.ticket_type_id,
            generate_series(1, c.quantity)
        WHERE c.user_id = $2
        returning *
    )",
                  order_id, id);

    query = R"(
        update "tickets" set "qr_code" = './images/' || "ticket_id" || '.png'
        where "order_id" = $1
        returning *
    )";

    r = w.exec_params(query, order_id);

    for(const pqxx::row& rowResult : r)
    {
        const string& path = rowResult["qr_code"].c_str();
        const string& id = rowResult["ticket_id"].c_str();
        const string& url = "https://x.badtom.dpdns.org/api/ticket/scan/" + string(id);
        TicketService::generateQRCode(path, url);
    }
    

    // 6. 清空购物车
    r = w.exec_params(R"(
        WITH deleted AS (
        DELETE FROM cart
        WHERE user_id = $1
        RETURNING *
    )
SELECT json_agg(deleted) FROM deleted;
)",
                      id);

    w.commit();

    auto dataStr = r[0][0].c_str();
    auto data = json::load(dataStr);

    return json::wvalue{{"message", "ok"}, {"data", data}};
}
