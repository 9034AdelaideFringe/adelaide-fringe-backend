#!/bin/bash

echo "开始构建和运行Docker容器..."

# 构建Docker镜像
docker build -t fringe-backend .

# 运行Docker容器
docker run -p 8080:8080 --rm fringe-backend

echo "服务器已关闭" 