# 使用官方的 C++ 基础镜像
FROM gcc:latest AS base

# 设置工作目录
WORKDIR /app

# 复制源代码到容器中
COPY . .

# 安装依赖
RUN apt-get update && \
    apt-get install -y libasio-dev libpqxx-dev gcc make cmake curl libssl-dev uuid-dev postgresql-client && \
    # 安装 Crow
    curl -O -L https://github.com/CrowCpp/Crow/releases/download/v1.2.1.2/Crow-1.2.1-Linux.deb && \
    apt install "./Crow-1.2.1-Linux.deb" -y && \
    rm ./Crow-1.2.1-Linux.deb

ENV DATABASE_URL=""
ENV JWT_SECRET=""

# 生成 config.json 文件
COPY entrypoint.sh /app/entrypoint.sh
RUN chmod +x /app/entrypoint.sh
ENTRYPOINT ["/app/entrypoint.sh"]

# 初始化数据库
RUN PGPASSWORD=npg_9nH8RLBWUdro psql -h ep-silent-leaf-a77kylcx-pooler.ap-southeast-2.aws.neon.tech -U neondb_owner -d dev -f init_db.sql || echo "数据库初始化可能失败，请手动检查"

# 构建阶段
FROM base AS build

# 列出目录内容，帮助调试
RUN ls -la

# 构建项目并显示详细信息
RUN cmake -B build -DCMAKE_VERBOSE_MAKEFILE=ON && cmake --build build

# 构建后列出目录内容，帮助调试
RUN ls -la && ls -la build/

# 复制可执行文件到根目录
RUN cp build/server ./server || echo "无法找到可执行文件"
RUN chmod +x ./server || echo "无法设置可执行权限"

# 测试阶段
FROM base AS test-stage

# 构建测试
RUN rm -rf test_build && \
    mkdir -p test_build && \
    cd test_build && \
    cmake .. -DBUILD_TESTING=ON -DCMAKE_POLICY_VERSION_MINIMUM=3.5 && \
    make && \
    ls -la && \
    echo "测试构建完成" && \
    echo "可用测试:" && \
    find . -name "*_tests" -type f -executable

# 生产阶段
FROM build AS prod

# 设置容器启动命令
CMD ["./server"]