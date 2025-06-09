#!/bin/bash

# Adelaide Fringe Backend - 单元测试运行脚本
# 此脚本构建项目、运行测试并生成覆盖率报告

set -e  # 如果任何命令失败，脚本退出

echo "🚀 Adelaide Fringe Backend - 单元测试执行"
echo "=========================================="

# 检查必要的工具
echo "📋 检查环境..."

if ! command -v cmake &> /dev/null; then
    echo "❌ CMake 未找到，请安装 CMake"
    exit 1
fi

if ! command -v make &> /dev/null; then
    echo "❌ Make 未找到，请安装 Make"
    exit 1
fi

echo "✅ 环境检查完成"

# 清理之前的构建
echo "🧹 清理之前的构建..."
rm -rf build/
rm -rf test_reports/
mkdir -p test_reports

# 构建项目
echo "🔨 构建项目..."
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage" -DCMAKE_POLICY_VERSION_MINIMUM=3.5

if ! cmake --build build; then
    echo "❌ 项目构建失败"
    exit 1
fi

echo "✅ 项目构建成功"

# 运行简单测试
echo "🧪 运行简单测试..."
if [ -f "build/tests/simple_test" ]; then
    echo "运行 simple_test..."
    cd build/tests
    ./simple_test --gtest_output=xml:../../test_reports/simple_test_results.xml
    cd ../..
    echo "✅ 简单测试完成"
else
    echo "⚠️  simple_test 未找到，跳过"
fi

# 运行单元测试（如果可用）
echo "🧪 运行单元测试..."
if [ -f "build/tests/unit_tests" ]; then
    echo "运行 unit_tests..."
    cd build/tests
    ./unit_tests --gtest_output=xml:../../test_reports/unit_test_results.xml
    cd ../..
    echo "✅ 单元测试完成"
else
    echo "⚠️  unit_tests 未找到，可能由于依赖问题无法构建"
    echo "  这通常是由于 Crow 库或其他依赖未正确安装"
fi

# 运行集成测试（如果可用）
echo "🧪 运行集成测试..."
if [ -f "build/tests/integration/integration_tests" ]; then
    echo "运行 integration_tests..."
    cd build/tests/integration
    ./integration_tests --gtest_output=xml:../../../test_reports/integration_test_results.xml
    cd ../../..
    echo "✅ 集成测试完成"
else
    echo "⚠️  integration_tests 未找到"
fi

# 生成覆盖率报告（如果可用）
echo "📊 生成覆盖率报告..."
if command -v gcov &> /dev/null && command -v lcov &> /dev/null; then
    echo "生成 LCOV 覆盖率报告..."
    
    # 初始化覆盖率
    lcov --directory build --zerocounters
    
    # 运行测试后收集覆盖率数据
    lcov --directory build --capture --output-file test_reports/coverage.info
    
    # 过滤掉系统头文件和测试文件
    lcov --remove test_reports/coverage.info '/usr/*' '/opt/*' '*/tests/*' '*/build/*' --output-file test_reports/coverage_filtered.info
    
    # 生成HTML报告
    if command -v genhtml &> /dev/null; then
        genhtml test_reports/coverage_filtered.info --output-directory test_reports/coverage_html
        echo "📄 HTML覆盖率报告生成在: test_reports/coverage_html/index.html"
    fi
    
    # 显示覆盖率摘要
    lcov --summary test_reports/coverage_filtered.info
    
else
    echo "⚠️  gcov/lcov 未找到，跳过覆盖率报告生成"
    echo "  安装方法: sudo apt-get install lcov (Ubuntu/Debian)"
    echo "  或: brew install lcov (macOS)"
fi

# 生成测试摘要
echo "📋 生成测试摘要..."

cat > test_reports/test_summary.md << EOF
# Adelaide Fringe Backend - 测试报告

## 测试执行时间
生成时间: $(date)

## 测试结果摘要

### 简单测试
$(if [ -f "test_reports/simple_test_results.xml" ]; then echo "✅ 执行成功"; else echo "❌ 未执行"; fi)

### 单元测试  
$(if [ -f "test_reports/unit_test_results.xml" ]; then echo "✅ 执行成功"; else echo "❌ 未执行"; fi)

### 集成测试
$(if [ -f "test_reports/integration_test_results.xml" ]; then echo "✅ 执行成功"; else echo "❌ 未执行"; fi)

## 覆盖率报告
$(if [ -f "test_reports/coverage_filtered.info" ]; then echo "✅ 生成成功"; else echo "❌ 未生成"; fi)

## 文件结构
\`\`\`
test_reports/
├── test_summary.md (本文件)
├── simple_test_results.xml (如果存在)
├── unit_test_results.xml (如果存在) 
├── integration_test_results.xml (如果存在)
├── coverage.info (如果存在)
├── coverage_filtered.info (如果存在)
└── coverage_html/ (如果存在)
    └── index.html
\`\`\`

## 下一步
1. 修复任何失败的测试
2. 提高代码覆盖率至80%以上
3. 添加更多边界条件测试
4. 完善集成测试覆盖

## 当前进度
- [x] 基础测试框架设置
- [x] 工具类单元测试 (Config, Hash, UUID)
- [x] 控制器测试框架 (部分完成)
- [ ] 服务层测试
- [ ] 完整的集成测试
- [ ] 性能测试
EOF

echo "📄 测试摘要报告生成在: test_reports/test_summary.md"

# 显示最终结果
echo ""
echo "🎉 测试执行完成！"
echo "=========================================="
echo "📁 测试报告位置: test_reports/"

if [ -f "test_reports/coverage_html/index.html" ]; then
    echo "🌐 打开覆盖率报告:"
    echo "   file://$(pwd)/test_reports/coverage_html/index.html"
fi

echo ""
echo "🎯 当前测试策略进度:"
echo "   Phase 1: 核心控制器测试 - 🚧 进行中"
echo "   Phase 2: 业务逻辑测试 - ⏳ 待开始"  
echo "   Phase 3: 集成测试 - ⏳ 待开始"
echo "   Phase 4: 优化测试 - ⏳ 待开始"
echo ""
echo "📊 目标: 80%+ 代码覆盖率"
echo "✨ 继续努力！" 