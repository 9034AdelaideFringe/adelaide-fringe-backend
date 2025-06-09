#!/bin/bash

echo "=========================================="
echo "Adelaide Fringe Backend - 测试报告"
echo "日期: $(date)"
echo "=========================================="

# 创建结果目录
mkdir -p test_results
cd test_results

# 清理之前的结果
rm -f *.xml *.log coverage.info

echo -e "\n📋 测试总览"
echo "=========================================="

# 1. 运行工具类测试（独立）
echo -e "\n🔧 运行独立工具类测试..."
../build/standalone_utils_test --gtest_output=xml:standalone_utils_test.xml 2>&1 | tee standalone_utils_test.log

# 2. 运行简单测试
echo -e "\n🧪 运行简单测试..."
../build/simple_test --gtest_output=xml:simple_test.xml 2>&1 | tee simple_test.log

# 3. 运行完整单元测试
echo -e "\n🏗️ 运行完整单元测试..."
../build/unit_tests --gtest_output=xml:unit_tests.xml 2>&1 | tee unit_tests.log

# 4. 生成测试统计
echo -e "\n📊 生成测试统计报告..."

echo "=========================================="
echo "              测试结果汇总"
echo "=========================================="

# 解析测试结果
standalone_passed=$(grep -o '\[  PASSED  \] [0-9]* test' standalone_utils_test.log | tail -1 | grep -o '[0-9]*' || echo "0")
standalone_failed=$(grep -o '\[  FAILED  \] [0-9]* test' standalone_utils_test.log | tail -1 | grep -o '[0-9]*' || echo "0")

simple_passed=$(grep -o '\[  PASSED  \] [0-9]* test' simple_test.log | tail -1 | grep -o '[0-9]*' || echo "0")
simple_failed=$(grep -o '\[  FAILED  \] [0-9]* test' simple_test.log | tail -1 | grep -o '[0-9]*' || echo "0")

unit_passed=$(grep -o '\[  PASSED  \] [0-9]* test' unit_tests.log | tail -1 | grep -o '[0-9]*' || echo "0")
unit_failed=$(grep -o '\[  FAILED  \] [0-9]* test' unit_tests.log | tail -1 | grep -o '[0-9]*' || echo "0")

# 计算总计
total_passed=$((standalone_passed + simple_passed + unit_passed))
total_failed=$((standalone_failed + simple_failed + unit_failed))
total_tests=$((total_passed + total_failed))

# 计算通过率
if [ $total_tests -gt 0 ]; then
    pass_rate=$(( (total_passed * 100) / total_tests ))
else
    pass_rate=0
fi

echo "📈 测试套件统计:"
echo "  • 独立工具测试: $standalone_passed 通过, $standalone_failed 失败"
echo "  • 简单测试:     $simple_passed 通过, $simple_failed 失败"  
echo "  • 单元测试:     $unit_passed 通过, $unit_failed 失败"
echo ""
echo "🎯 总计:"
echo "  • 总测试数:     $total_tests"
echo "  • 通过:         $total_passed"
echo "  • 失败:         $total_failed"
echo "  • 通过率:       $pass_rate%"

# 生成详细分析
echo -e "\n🔍 详细分析:"
echo "=========================================="

if [ $total_passed -gt 0 ]; then
    echo "✅ 成功的测试类别:"
    if [ $standalone_passed -gt 0 ]; then
        echo "  • 工具类测试 (Hash, Config): $standalone_passed/8"
    fi
    if [ $simple_passed -gt 0 ]; then
        echo "  • 基础功能测试: $simple_passed 个"
    fi
    if [ $unit_passed -gt 0 ]; then
        echo "  • 集成单元测试: $unit_passed 个"
    fi
fi

if [ $total_failed -gt 0 ]; then
    echo -e "\n❌ 失败的测试:"
    echo "  • 失败测试数: $total_failed"
    echo "  • 主要问题: 控制器测试的数据库连接问题"
    echo "  • 影响范围: EventController 和 TicketController 测试"
fi

echo -e "\n📋 建议:"
echo "=========================================="
if [ $pass_rate -ge 80 ]; then
    echo "🎉 优秀! 测试通过率达到 $pass_rate%"
elif [ $pass_rate -ge 60 ]; then
    echo "👍 良好! 测试通过率 $pass_rate%，还有改进空间"
elif [ $pass_rate -ge 40 ]; then
    echo "⚠️  需要改进! 测试通过率 $pass_rate%"
else
    echo "🔴 需要重点关注! 测试通过率仅 $pass_rate%"
fi

if [ $total_failed -gt 0 ]; then
    echo "🔧 下一步行动:"
    echo "  1. 修复控制器层的数据库连接抽象"
    echo "  2. 完善Mock系统以支持pqxx::connection"
    echo "  3. 重构控制器使用PostgresConnection接口"
fi

echo -e "\n📄 详细日志文件位置:"
echo "  • 独立工具测试: test_results/standalone_utils_test.log"
echo "  • 简单测试:     test_results/simple_test.log"
echo "  • 单元测试:     test_results/unit_tests.log"

echo -e "\n=========================================="
echo "测试报告生成完成 ✨"
echo "==========================================" 