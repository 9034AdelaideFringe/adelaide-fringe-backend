#include <gtest/gtest.h>
#include "utils/Hash.h"

TEST(HashTest, GeneratesSHA256Hash) {
    std::string input = "password123";
    std::string expected = "ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f";
    
    std::string actual = sha256(input);
    EXPECT_EQ(actual, expected);
}

TEST(HashTest, EmptyStringHash) {
    std::string input = "";
    std::string expected = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
    
    std::string actual = sha256(input);
    EXPECT_EQ(actual, expected);
}

TEST(HashTest, ConsistentHashing) {
    std::string input = "test_password";
    
    // 多次哈希应该产生相同的结果
    std::string hash1 = sha256(input);
    std::string hash2 = sha256(input);
    
    EXPECT_EQ(hash1, hash2);
} 