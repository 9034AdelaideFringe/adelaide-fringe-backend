#!/bin/bash

echo "开始使用Docker构建和运行测试..."

# 创建报告目录
mkdir -p test_reports

# 构建Docker镜像
docker build -t fringe-backend-tests --target test-stage .

# 运行测试容器
docker run --rm -it -v $(pwd)/test_reports:/app/test_reports fringe-backend-tests bash -c "cd /app/test_build && \
    echo '运行单元测试...' && \
    if [ -f ./tests/unit_tests ]; then \
        GTEST_COLOR=1 ./tests/unit_tests --gtest_color=yes --gtest_output=xml:/app/test_reports/unit_tests.xml; \
    else \
        echo '单元测试可执行文件不存在'; \
    fi && \
    echo '运行集成测试...' && \
    if [ -f ./tests/integration/integration_tests ]; then \
        GTEST_COLOR=1 ./tests/integration/integration_tests --gtest_color=yes --gtest_output=xml:/app/test_reports/integration_tests.xml; \
    else \
        echo '集成测试可执行文件不存在'; \
        echo '查找所有测试可执行文件:'; \
        find /app -name '*tests' -type f -executable; \
    fi && \
    echo '生成测试报告...' && \
    apt-get update && apt-get install -y python3-pip && \
    pip install gtest-html-report && \
    gtest-html-report --source-dir=/app --target-dir=/app/test_reports --report-dir=/app/test_reports/html"

echo "测试报告已生成在 ./test_reports/html 目录"
echo "测试完成" 