#!/bin/bash

# 创建build目录
mkdir -p test_build

# 进入build目录
cd test_build

# 配置CMake以启用测试，添加策略参数
cmake .. -DBUILD_TESTING=ON -DCMAKE_POLICY_VERSION_MINIMUM=3.5

# 编译
make

# 运行简单测试
echo "运行简单测试..."
GTEST_COLOR=1 ./tests/simple_test --gtest_color=yes

echo "测试完成" 