#!/bin/bash

echo "开始构建和运行简单测试..."

# 构建Docker镜像
docker build -t fringe-simple-test -f Dockerfile.test .

# 运行测试容器
docker run --rm fringe-simple-test

echo "测试完成" 