#!/bin/bash

# 生成配置文件
cat > config.json << EOF
{
    "database": "${DATABASE_URL}",
    "JWTSecret": "${JWT_SECRET}"
}
EOF

# 启动服务器
exec ./server 