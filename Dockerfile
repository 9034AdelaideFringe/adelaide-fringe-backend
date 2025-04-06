# 使用官方的 C++ 基础镜像
FROM gcc:latest

# 设置工作目录
WORKDIR /app

# 复制源代码到容器中
COPY . .

# 安装依赖
RUN apt-get update && \
    apt-get install -y libasio-dev libpqxx-dev gcc make cmake curl && \
    # 安装 Crow
    curl -O -L https://github.com/CrowCpp/Crow/releases/download/v1.2.1.2/Crow-1.2.1-Linux.deb && \
    apt install "./Crow-1.2.1-Linux.deb" -y && \
    rm ./Crow-1.2.1-Linux.deb

ENV DATABASE_URL="postgres://neondb_owner:npg_9nH8RLBWUdro@ep-silent-leaf-a77kylcx-pooler.ap-southeast-2.aws.neon.tech/neondb?connect_timeout=15&sslmode=require"
ENV JWT_SECRET="111"
# 生成 config.json 文件
RUN echo '{ "database": "'$DATABASE_URL'", "JWTSecret": "'$JWT_SECRET'" }' > config.json

# 构建项目
RUN cmake -B build && cmake --build build

# 设置容器启动命令
CMD ["./server"]  # 启动命令为 ./server