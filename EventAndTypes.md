# Event和TicketType同时创建的解决方案

## 问题背景

当前系统在创建活动(Event)和票种(TicketType)时需要两次独立的API调用，流程繁琐且可能导致数据不一致。具体问题包括：

1. 前端需要发起两次API请求，增加网络往返次数
2. 如果第一次请求成功但第二次失败，会导致孤立的Event记录
3. 前端逻辑复杂，需要在两个请求之间处理依赖关系

## 解决方案概述

为了解决上述问题，我们设计了一个优化方案：在同一个数据库事务中同时创建Event和相关的TicketType数据。通过引入一个新的API端点，使前端只需发送一次请求，就能完成整个创建流程。

## 具体实现步骤

### 1. 修改EventService.h

首先需要在EventService头文件中添加新的方法声明：
```cpp
// src/service/EventService.cpp
json::wvalue EventService::createEventWithTicketTypes(const request &req)
{
    CROW_LOG_INFO << "Creating event with ticket types in a single transaction";
    
    // 解析multipart/form-data请求
    multipart::message_view fileMessage(req);
    
    // 提取Event相关字段
    auto titlePart = fileMessage.get_part_by_name("title").body;
    auto descriptionPart = fileMessage.get_part_by_name("description").body;
    auto short_descriptionPart = fileMessage.get_part_by_name("short_description").body;
    auto imagePart = fileMessage.get_part_by_name("image");
    auto venueSeatingLayoutPart = fileMessage.get_part_by_name("venueSeatingLayout");
    auto datePart = fileMessage.get_part_by_name("date").body;
    auto timePart = fileMessage.get_part_by_name("time").body;
    auto end_timePart = fileMessage.get_part_by_name("end_time").body;
    auto venuePart = fileMessage.get_part_by_name("venue").body;
    auto capacityPart = fileMessage.get_part_by_name("capacity").body;
    auto categoryPart = fileMessage.get_part_by_name("category").body;
    auto statusPart = fileMessage.get_part_by_name("status").body;
    auto created_byPart = fileMessage.get_part_by_name("created_by").body;
    
    // 提取票种信息的JSON字符串
    auto ticketTypesPart = fileMessage.get_part_by_name("ticket_types").body;
    
    // 检查必要字段
    if (titlePart.empty() || descriptionPart.empty() || imagePart.body.empty() || 
        venueSeatingLayoutPart.body.empty() || ticketTypesPart.empty())
    {
        CROW_LOG_ERROR << "Missing required fields for event creation";
        return json::wvalue{{"error", "missing required fields"}};
    }
    
    // 解析票种信息JSON
    json::rvalue ticketTypesJson;
    try {
        ticketTypesJson = json::load(std::string(ticketTypesPart));
        if (!ticketTypesJson.is_array()) {
            CROW_LOG_ERROR << "ticket_types must be an array";
            return json::wvalue{{"error", "ticket_types must be an array"}};
        }
        
        // 验证票种数组不为空
        if (ticketTypesJson.size() == 0) {
            CROW_LOG_ERROR << "No ticket types provided";
            return json::wvalue{{"error", "At least one ticket type must be provided"}};
        }
        
        // 验证每个票种数据
        for (int i = 0; i < ticketTypesJson.size(); i++) {
            std::string errorMessage;
            if (!validateTicketTypeData(ticketTypesJson[i], errorMessage)) {
                CROW_LOG_ERROR << "Invalid ticket type data: " << errorMessage;
                return json::wvalue{{"error", errorMessage}};
            }
        }
    } catch (const std::exception &e) {
        CROW_LOG_ERROR << "Failed to parse ticket_types JSON: " << e.what();
        return json::wvalue{{"error", std::string("Invalid ticket_types JSON: ") + e.what()}};
    }
    
    // 准备Event数据
    auto title = std::string(titlePart);
    auto description = std::string(descriptionPart);
    auto short_description = std::string(short_descriptionPart);
    
    // 上传图片文件
    std::string image;
    try {
        CROW_LOG_INFO << "Uploading event image";
        image = UploadService::uploadFile(imagePart);
        CROW_LOG_INFO << "Image uploaded successfully: " << image;
    } catch (const FileUploadException& e) {
        CROW_LOG_ERROR << "Image upload failed: " << e.what();
        return json::wvalue{{"error", std::string("Image upload failed: ") + e.what()}};
    }
    
    // 上传场地座位布局
    std::string venueSeatingLayout;
    try {
        CROW_LOG_INFO << "Uploading venue seating layout";
        venueSeatingLayout = UploadService::uploadFile(venueSeatingLayoutPart);
        CROW_LOG_INFO << "Venue seating layout uploaded successfully: " << venueSeatingLayout;
    } catch (const FileUploadException& e) {
        CROW_LOG_ERROR << "Venue seating layout upload failed: " << e.what();
        return json::wvalue{{"error", std::string("Venue seating layout upload failed: ") + e.what()}};
    }
    
    auto date = std::string(datePart);
    auto time = std::string(timePart);
    auto end_time = std::string(end_timePart);
    auto venue = std::string(venuePart);
    auto capacity = std::string(capacityPart);
    auto category = std::string(categoryPart);
    auto status = std::string(statusPart);
    auto created_by = std::string(created_byPart);
    
    // 生成Event ID
    std::string event_id = generateUUID();
    CROW_LOG_INFO << "Generated new event_id: " << event_id;
    
    // 开始数据库事务
    CROW_LOG_INFO << "Starting database transaction";
    pqxx::connection conn(Config::get("database"));
    pqxx::work w{conn};
    
    try {
        // 第一步：创建Event
        CROW_LOG_INFO << "Creating event in database";
        std::string eventQuery = R"(insert into "events" 
        (
        "event_id",
        "title", 
        "description", 
        "short_description", 
        "image", 
        "venueseatinglayout", 
        "date", 
        "time", 
        "end_time", 
        "venue", 
        "capacity",
        "category",
        "status",
        "created_by",
        "created_at",
        "updated_at"
        ) 
        values ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, NOW(), NOW())
        returning *;
        )";
        
        auto eventResult = w.exec_params(eventQuery,
                                      event_id,
                                      title,
                                      description,
                                      short_description,
                                      image,
                                      venueSeatingLayout,
                                      date,
                                      time,
                                      end_time,
                                      venue,
                                      capacity,
                                      category,
                                      status,
                                      created_by);
        
        if (eventResult.empty()) {
            // 回滚事务并返回错误
            CROW_LOG_ERROR << "Failed to create event, no rows returned";
            w.abort();
            return json::wvalue{{"error", "Failed to create event"}};
        }
        
        CROW_LOG_INFO << "Event created successfully";
        
        // 解析创建的Event数据
        auto eventData = resultsToJSON(eventResult);
        
        // 第二步：创建所有TicketTypes
        CROW_LOG_INFO << "Creating " << ticketTypesJson.size() << " ticket types";
        std::vector<json::wvalue> ticketTypeResults;
        
        for (int i = 0; i < ticketTypesJson.size(); i++) {
            const auto& ticketType = ticketTypesJson[i];
            std::string ticket_type_id = generateUUID();
            std::string name = ticketType["name"].s();
            std::string description = ticketType["description"].s();
            std::string price = ticketType["price"].s();
            std::string available_quantity = ticketType["available_quantity"].s();
            
            CROW_LOG_INFO << "Creating ticket type " << i+1 << ": " << name;
            
            std::string ticketTypeQuery = R"(
                INSERT INTO "tickettypes" (
                "ticket_type_id",
                "event_id",
                "name",
                "description",
                "price",
                "available_quantity",
                "created_at",
                "updated_at"
                ) VALUES ($1, $2, $3, $4, $5, $6, NOW(), NOW())
                RETURNING *;
            )";
            
            auto ticketTypeResult = w.exec_params(
                ticketTypeQuery, 
                ticket_type_id, 
                event_id, 
                name, 
                description, 
                price, 
                available_quantity
            );
            
            if (ticketTypeResult.empty()) {
                // 如果任何一个票种创建失败，回滚整个事务
                CROW_LOG_ERROR << "Failed to create ticket type " << name;
                w.abort();
                return json::wvalue{{"error", "Failed to create ticket type: " + name}};
            }
            
            CROW_LOG_INFO << "Ticket type created successfully: " << ticket_type_id;
            ticketTypeResults.push_back(resultsToJSON(ticketTypeResult));
        }
        
        // 提交事务
        CROW_LOG_INFO << "All operations successful, committing transaction";
        w.commit();
        
        // 返回创建成功的Event和所有TicketType
        return json::wvalue{
            {"message", "Event and ticket types created successfully"}, 
            {"data", eventData}, 
            {"ticket_types", std::move(ticketTypeResults)}
        };
        
    } catch (const std::exception &e) {
        // 异常处理，确保事务回滚
        CROW_LOG_ERROR << "Transaction failed: " << e.what();
        w.abort();
        return json::wvalue{{"error", std::string("Transaction failed: ") + e.what()}};
    }
}

// 验证票种数据的辅助方法
bool EventService::validateTicketTypeData(const json::rvalue &ticketType, std::string &errorMessage) {
    // 检查必要字段是否存在
    if (!ticketType.has("name")) {
        errorMessage = "Ticket type name is required";
        return false;
    }
    
    if (!ticketType.has("description")) {
        errorMessage = "Ticket type description is required";
        return false;
    }
    
    if (!ticketType.has("price")) {
        errorMessage = "Ticket type price is required";
        return false;
    }
    
    if (!ticketType.has("available_quantity")) {
        errorMessage = "Ticket type available_quantity is required";
        return false;
    }
    
    // 检查价格是否为有效数字
    try {
        std::string priceStr = ticketType["price"].s();
        double price = std::stod(priceStr);
        if (price < 0) {
            errorMessage = "Price cannot be negative";
            return false;
        }
    } catch (const std::exception &e) {
        errorMessage = "Invalid price format";
        return false;
    }
    
    // 检查数量是否为有效整数
    try {
        std::string quantityStr = ticketType["available_quantity"].s();
        int quantity = std::stoi(quantityStr);
        if (quantity < 0) {
            errorMessage = "Available quantity cannot be negative";
            return false;
        }
    } catch (const std::exception &e) {
        errorMessage = "Invalid available quantity format";
        return false;
    }
    
    return true;
}
```
### 4. 实现EventController中的路由和方法
```cpp
// include/service/EventService.h
class EventService {
public:
    static json::wvalue createEvent(const request &req, const string &id = "");
    static json::wvalue updateEvent(const request &req);
    static json::wvalue deleteEvent(const request &req);
    
    // 新增方法 - 在单个事务中创建活动和票种
    static json::wvalue createEventWithTicketTypes(const request &req);

private:
    static json::wvalue createEventImpl(const request &req, const string &id);
    static json::wvalue updateEventImpl(const request &req);
    static json::wvalue deleteEventImpl(const request &req);
    
    // 新增私有辅助方法 - 用于验证票种数据
    static bool validateTicketTypeData(const json::rvalue &ticketType, std::string &errorMessage);
};
```

### 2. 修改EventController.h

接下来，在EventController头文件中添加新的方法声明：

```cpp
// include/controller/event/EventController.h
class EventController : public Controller {
private:
    blueprint bp_;

public:
    EventController();
    
    response createEvent(const request &req);
    response updateEvent(const request &req);
    response deleteEvent(const request &req);
    response getEvent(const request &req);
    response getEventById(const std::string &id);
    response searchEvent(const request &req);
    response getTicketTypes(const request &req, const string &id);
    
    // 新增方法 - 处理创建活动和票种的复合请求
    response createEventWithTicketTypes(const request &req);
};
```

### 3. 创建组合服务

然后，在EventService.cpp中实现新方法，用于同时处理Event和TicketType的创建:
```cpp
// src/controller/event/EventController.cpp
EventController::EventController() : bp_("event")
{
    getBlueprint().register_blueprint(bp_);
    // 现有路由...
    
    // 新增路由 - 一次性创建Event和TicketType
    CROW_BP_ROUTE(bp_, "/with-ticket-types").methods("POST"_method)([this](const request &req){
        return this->createEventWithTicketTypes(req);
    });
}

// 新方法实现
response EventController::createEventWithTicketTypes(const request &req)
{
    CROW_LOG_INFO << "Received request to create event with ticket types";
    
    // 检查Content-Type是否为multipart/form-data
    auto contentType = req.get_header_value("Content-Type");
    if (contentType.find("multipart/form-data") == std::string::npos) {
        CROW_LOG_ERROR << "Invalid content type for event creation with ticket types: " << contentType;
        return json::wvalue{{"error", "Content-Type must be multipart/form-data"}};
    }
    
    // 检查用户权限（可选）
    auto cookieHeader = req.get_header_value("Cookie");
    std::string jwt;
    
    std::string tokenPrefix = "jwt=";
    auto pos = cookieHeader.find(tokenPrefix);
    if (pos != std::string::npos) {
        auto end = cookieHeader.find(';', pos);
        jwt = cookieHeader.substr(pos + tokenPrefix.size(), end - pos - tokenPrefix.size());
        
        try {
            auto decoded = UserService::decodeJWT(jwt);
            std::string role = decoded.get_payload_claim("role").as_string();
            
            // 仅允许管理员或组织者创建活动
            if (role != "admin" && role != "organizer") {
                CROW_LOG_ERROR << "Unauthorized attempt to create event: " << role;
                return json::wvalue{{"error", "Unauthorized: Only admins and organizers can create events"}};
            }
        } catch (const std::exception &e) {
            CROW_LOG_ERROR << "JWT validation failed: " << e.what();
            return json::wvalue{{"error", "Authentication error"}};
        }
    }
    
    // 调用服务层方法处理请求
    return EventService::createEventWithTicketTypes(req);
}
```
## 需要修改的文件
以下是本方案需要修改的文件清单：

1. **include/service/EventService.h**
   - 添加 `createEventWithTicketTypes` 方法声明
   - 添加 `validateTicketTypeData` 辅助方法声明

2. **src/service/EventService.cpp**
   - 实现 `createEventWithTicketTypes` 方法
   - 实现 `validateTicketTypeData` 辅助方法

3. **include/controller/event/EventController.h**
   - 添加 `createEventWithTicketTypes` 方法声明

4. **src/controller/event/EventController.cpp**
   - 注册新路由 `/with-ticket-types`
   - 实现 `createEventWithTicketTypes` 方法

5. **不需要修改的文件**
   - 不需要修改 TicketController 相关文件
   - 不需要修改数据库结构，使用现有表

## 数据流向图
数据流向图
```
┌─────────┐                                ┌─────────┐                             ┌──────────┐
│  前端   │                                │  后端   │                             │  数据库  │
└────┬────┘                                └────┬────┘                             └────┬─────┘
     │                                          │                                       │
     │  1. 发送multipart/form-data请求          │                                       │
     │  包含Event数据和TicketTypes数组           │                                       │
     │  POST /event/with-ticket-types           │                                       │
     │─────────────────────────────────────────>│                                       │
     │                                          │                                       │
     │                                          │  2. 验证请求参数和用户权限             │
     │                                          │───────────────────────┐               │
     │                                          │                       │               │
     │                                          │<──────────────────────┘               │
     │                                          │                                       │
     │                                          │  3. 上传图片和座位布局文件             │
     │                                          │───────────────────────┐               │
     │                                          │                       │               │
     │                                          │<──────────────────────┘               │
     │                                          │                                       │
     │                                          │  4. 生成event_id                     │
     │                                          │───────────────────────┐               │
     │                                          │                       │               │
     │                                          │<──────────────────────┘               │
     │                                          │                                       │
     │                                          │  5. 开启数据库事务                    │
     │                                          │──────────────────────────────────────>│
     │                                          │                                       │
     │                                          │  6. 插入event记录                     │
     │                                          │──────────────────────────────────────>│
     │                                          │                                       │
     │                                          │  7. 返回创建的event信息               │
     │                                          │<──────────────────────────────────────│
     │                                          │                                       │
     │                                          │  8. 循环创建多个票种记录              │
     │                                          │──────────────────────────────────────>│
     │                                          │                                       │
     │                                          │  9. 返回创建的票种信息                │
     │                                          │<──────────────────────────────────────│
     │                                          │                                       │
     │                                          │ 10. 如果全部成功，提交事务            │
     │                                          │──────────────────────────────────────>│
     │                                          │                                       │
     │                                          │ 11. 如果有错误，回滚事务              │
     │                                          │──────────────────────────────────────>│
     │                                          │                                       │
     │                                          │ 12. 返回事务结果                      │
     │                                          │<──────────────────────────────────────│
     │                                          │                                       │
     │ 13. 返回响应(包含event和tickets数据)     │                                       │
     │<─────────────────────────────────────────│                                       │
     │                                          │                                       │
```
## API 测试示例

### 使用curl测试

可以使用以下curl命令测试新的API端点：
```bash
# 创建Event和TicketTypes的Curl命令示例
curl -X POST http://localhost:8080/api/event/with-ticket-types \
  -H "Content-Type: multipart/form-data" \
  -H "Cookie: jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpZCI6InVzZXIxMjMiLCJyb2xlIjoiYWRtaW4ifQ.8tat0J1VHDxJYoCX_uuZXBVmkR0qJdHRB8O6U0bGYkQ" \
  -F "title=Adelaide Fringe Comedy Show" \
  -F "description=An amazing comedy show at Adelaide Fringe!" \
  -F "short_description=Comedy show" \
  -F "image=@/path/to/image.jpg" \
  -F "venueSeatingLayout=@/path/to/layout.jpg" \
  -F "date=2025-06-15" \
  -F "time=19:00" \
  -F "end_time=21:00" \
  -F "venue=Adelaide Festival Centre" \
  -F "capacity=200" \
  -F "category=Comedy" \
  -F "status=UPCOMING" \
  -F "created_by=user123" \
  -F 'ticket_types=[
    {
      "name": "普通票",
      "description": "普通入场券",
      "price": "25.00",
      "available_quantity": "150"
    },
    {
      "name": "VIP票",
      "description": "前排座位+饮品",
      "price": "50.00",
      "available_quantity": "50"
    }
  ]'
```

```
## 后端响应示例

```json
{
  "message": "Event and ticket types created successfully",
  "data": [
    {
      "event_id": "e12345ab-cd67-89ef-0123-456789abcdef",
      "title": "Adelaide Fringe Comedy Show",
      "description": "An amazing comedy show at Adelaide Fringe!",
      "short_description": "Comedy show",
      "image": "/uploads/events/image_e12345ab.jpg",
      "venueseatinglayout": "/uploads/events/seating_e12345ab.jpg",
      "date": "2025-06-15",
      "time": "19:00",
      "end_time": "21:00",
      "venue": "Adelaide Festival Centre",
      "capacity": "200",
      "category": "Comedy",
      "status": "UPCOMING",
      "created_by": "user123",
      "created_at": "2025-05-15T10:30:45.123Z",
      "updated_at": "2025-05-15T10:30:45.123Z"
    }
  ],
  "ticket_types": [
    {
      "ticket_type_id": "t98765fe-dc43-21ba-9876-543210fedcba",
      "event_id": "e12345ab-cd67-89ef-0123-456789abcdef",
      "name": "普通票",
      "description": "普通入场券",
      "price": "25.00",
      "available_quantity": "150",
      "created_at": "2025-05-15T10:30:45.456Z",
      "updated_at": "2025-05-15T10:30:45.456Z"
    },
    {
      "ticket_type_id": "t24680bd-ac97-53ef-1357-924680acbd13",
      "event_id": "e12345ab-cd67-89ef-0123-456789abcdef",
      "name": "VIP票",
      "description": "前排座位+饮品",
      "price": "50.00",
      "available_quantity": "50",
      "created_at": "2025-05-15T10:30:45.789Z",
      "updated_at": "2025-05-15T10:30:45.789Z"
    }
  ]
}
```

## 错误处理示例

```json
{
  "error": "Invalid ticket types JSON: Expected array"
}
```

或

```json
{
  "error": "Failed to create ticket type: VIP票"
}
```

## 方案优势

1. **减少API调用**：前端只需发送一次请求，替代原来的两次独立API调用
2. **事务一致性**：通过数据库事务确保Event和TicketType创建的原子性，要么全部成功，要么全部失败
3. **减少错误处理**：如果创建Event或TicketType时发生错误，整个操作会自动回滚，避免孤立数据
4. **提高性能**：减少网络往返次数和数据库连接开销
5. **简化前端逻辑**：前端不再需要处理两次请求之间的依赖关系
6. **增强用户体验**：用户只需提交一次表单，减少等待时间和操作步骤
7. **降低出错率**：防止第二步骤在用户端被意外跳过，确保Event和TicketTypes数据的完整性

## 部署注意事项

1. 后端需要支持处理multipart/form-data格式的请求
2. 确保服务器配置允许足够大的上传文件大小限制
3. 在生产环境中实施适当的请求频率限制和文件大小限制
4. 添加适当的日志记录以便于调试和监控
5. 实施健壮的错误处理和用户友好的错误消息