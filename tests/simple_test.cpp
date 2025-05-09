#include <gtest/gtest.h>

// 简单的测试函数
TEST(SimpleTest, BasicTest) {
    // 进行基本断言
    EXPECT_TRUE(true);
    EXPECT_EQ(2 + 2, 4);
}

// 主函数
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 