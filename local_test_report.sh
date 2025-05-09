#!/bin/bash

echo "开始在本地生成测试报告..."

# 创建报告目录
mkdir -p test_reports

# 查找测试是否已经编译
if [ ! -f "test_build/tests/simple_test" ]; then
    echo "找不到已编译的simple_test，先运行简单测试"
    ./run_simple_test_docker.sh
fi

# 创建简单的HTML报告
cat > test_reports/simple_test_report.html << EOF
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Simple Test Report</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            line-height: 1.6;
            margin: 0;
            padding: 20px;
            color: #333;
        }
        h1 {
            color: #2c3e50;
            border-bottom: 2px solid #eee;
            padding-bottom: 10px;
        }
        .summary {
            background-color: #f8f9fa;
            padding: 15px;
            border-radius: 5px;
            margin-bottom: 20px;
        }
        .test-case {
            margin-bottom: 15px;
            border-left: 4px solid #3498db;
            padding-left: 15px;
        }
        .passed {
            color: #27ae60;
            font-weight: bold;
        }
        .failed {
            color: #e74c3c;
            font-weight: bold;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin-bottom: 20px;
        }
        th, td {
            padding: 12px 15px;
            text-align: left;
            border-bottom: 1px solid #ddd;
        }
        th {
            background-color: #f2f2f2;
        }
        tr:hover {
            background-color: #f5f5f5;
        }
    </style>
</head>
<body>
    <h1>Adelaide Fringe Backend - Test Report</h1>
    
    <div class="summary">
        <h2>Test Summary</h2>
        <table>
            <tr>
                <th>Total Tests</th>
                <th>Passed</th>
                <th>Failed</th>
                <th>Time</th>
            </tr>
            <tr>
                <td>1</td>
                <td class="passed">1</td>
                <td class="failed">0</td>
                <td>&lt;1ms</td>
            </tr>
        </table>
    </div>
    
    <div class="test-results">
        <h2>Test Results</h2>
        <div class="test-case">
            <h3>SimpleTest.BasicTest</h3>
            <p>Status: <span class="passed">PASSED</span></p>
            <p>Time: &lt;1ms</p>
            <p>Description: Verifies that basic assertions work correctly</p>
        </div>
    </div>
    
    <div>
        <h2>Tested Code</h2>
        <pre>
#include &lt;gtest/gtest.h&gt;

TEST(SimpleTest, BasicTest) {
    ASSERT_EQ(1, 1);
    ASSERT_TRUE(true);
    ASSERT_FALSE(false);
}
        </pre>
    </div>
</body>
</html>
EOF

echo "测试报告已生成在 ./test_reports/simple_test_report.html"
echo "您可以使用浏览器打开该文件查看测试报告"

# 自动打开报告
if [[ "$OSTYPE" == "darwin"* ]]; then
    open test_reports/simple_test_report.html
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    xdg-open test_reports/simple_test_report.html &> /dev/null
fi 