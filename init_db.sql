-- 创建用户表
CREATE TABLE IF NOT EXISTS "users" (
    "user_id" VARCHAR(50) PRIMARY KEY,
    "email" VARCHAR(255) UNIQUE NOT NULL,
    "password" VARCHAR(255) NOT NULL,
    "name" VARCHAR(255) NOT NULL,
    "role" VARCHAR(50) DEFAULT 'user',
    "created_at" TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    "updated_at" TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 创建活动表
CREATE TABLE IF NOT EXISTS "events" (
    "event_id" VARCHAR(50) PRIMARY KEY,
    "title" VARCHAR(255) NOT NULL,
    "description" TEXT,
    "location" VARCHAR(255),
    "start_time" TIMESTAMP,
    "end_time" TIMESTAMP,
    "created_by" VARCHAR(50) REFERENCES "users"("user_id"),
    "tickettypes" JSONB DEFAULT '[]',
    "images" JSONB DEFAULT '[]',
    "capacity" INTEGER DEFAULT 0,
    "created_at" TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    "updated_at" TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 创建票类型表
CREATE TABLE IF NOT EXISTS "tickettypes" (
    "tickettype_id" VARCHAR(50) PRIMARY KEY,
    "event_id" VARCHAR(50) REFERENCES "events"("event_id"),
    "name" VARCHAR(255) NOT NULL,
    "price" DECIMAL(10, 2) NOT NULL,
    "description" TEXT,
    "quantity" INTEGER DEFAULT 0,
    "created_at" TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    "updated_at" TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 创建票表
CREATE TABLE IF NOT EXISTS "tickets" (
    "ticket_id" VARCHAR(50) PRIMARY KEY,
    "user_id" VARCHAR(50) REFERENCES "users"("user_id"),
    "event_id" VARCHAR(50) REFERENCES "events"("event_id"),
    "tickettype_id" VARCHAR(50) REFERENCES "tickettypes"("tickettype_id"),
    "status" VARCHAR(50) DEFAULT 'active',
    "created_at" TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    "updated_at" TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 插入测试用户数据 (密码: password123，经过sha256哈希)
INSERT INTO "users" ("user_id", "email", "password", "name", "role")
VALUES 
('user-1', 'test@example.com', 'ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f', 'Test User', 'user'),
('admin-1', 'admin@example.com', 'ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f', 'Admin User', 'admin')
ON CONFLICT DO NOTHING;

-- 插入测试活动数据
INSERT INTO "events" ("event_id", "title", "description", "location", "start_time", "end_time", "created_by", "capacity")
VALUES 
('event-1', '测试活动1', '这是测试活动1的描述', '测试地点1', '2025-06-01 18:00:00', '2025-06-01 22:00:00', 'admin-1', 100),
('event-2', '测试活动2', '这是测试活动2的描述', '测试地点2', '2025-06-15 19:00:00', '2025-06-15 23:00:00', 'admin-1', 50)
ON CONFLICT DO NOTHING;

-- 插入测试票类型数据
INSERT INTO "tickettypes" ("tickettype_id", "event_id", "name", "price", "description", "quantity")
VALUES 
('tickettype-1', 'event-1', '标准票', 99.99, '标准入场券', 50),
('tickettype-2', 'event-1', 'VIP票', 199.99, 'VIP入场券', 20),
('tickettype-3', 'event-2', '普通票', 59.99, '普通入场券', 30)
ON CONFLICT DO NOTHING;

-- 更新活动的票类型关联
UPDATE "events"
SET "tickettypes" = '[
    {"id": "tickettype-1", "name": "标准票", "price": 99.99, "description": "标准入场券", "quantity": 50},
    {"id": "tickettype-2", "name": "VIP票", "price": 199.99, "description": "VIP入场券", "quantity": 20}
]'::jsonb
WHERE "event_id" = 'event-1';

UPDATE "events"
SET "tickettypes" = '[
    {"id": "tickettype-3", "name": "普通票", "price": 59.99, "description": "普通入场券", "quantity": 30}
]'::jsonb
WHERE "event_id" = 'event-2'; 