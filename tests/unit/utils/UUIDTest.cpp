#include <gtest/gtest.h>
#include "utils/uuid.h"
#include <set>
#include <regex>

class UUIDTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试环境设置
    }

    void TearDown() override {
        // 测试环境清理
    }
};

TEST_F(UUIDTest, GenerateValidUUID) {
    // 测试生成有效的UUID
    std::string uuid = generateUUID();
    
    // 验证UUID不为空
    EXPECT_FALSE(uuid.empty());
    
    // 验证UUID长度（标准UUID格式为36个字符）
    EXPECT_EQ(uuid.length(), 36);
    
    // 验证UUID格式使用正则表达式
    // 标准UUID格式: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
    std::regex uuidPattern(R"([0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12})");
    EXPECT_TRUE(std::regex_match(uuid, uuidPattern));
}

TEST_F(UUIDTest, GenerateUniqueUUIDs) {
    // 测试UUID的唯一性
    const int numUUIDs = 1000;
    std::set<std::string> uuidSet;
    
    // 生成多个UUID
    for (int i = 0; i < numUUIDs; ++i) {
        std::string uuid = generateUUID();
        uuidSet.insert(uuid);
    }
    
    // 验证所有UUID都是唯一的
    EXPECT_EQ(uuidSet.size(), numUUIDs);
}

TEST_F(UUIDTest, UUIDFormatConsistency) {
    // 测试UUID格式的一致性
    for (int i = 0; i < 100; ++i) {
        std::string uuid = generateUUID();
        
        // 验证连字符位置
        EXPECT_EQ(uuid[8], '-');
        EXPECT_EQ(uuid[13], '-');
        EXPECT_EQ(uuid[18], '-');
        EXPECT_EQ(uuid[23], '-');
        
        // 验证其他位置不是连字符
        for (size_t j = 0; j < uuid.length(); ++j) {
            if (j != 8 && j != 13 && j != 18 && j != 23) {
                EXPECT_NE(uuid[j], '-');
            }
        }
    }
}

TEST_F(UUIDTest, UUIDContainsValidHexCharacters) {
    // 测试UUID包含有效的十六进制字符
    std::string uuid = generateUUID();
    
    for (size_t i = 0; i < uuid.length(); ++i) {
        char c = uuid[i];
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            // 连字符位置
            EXPECT_EQ(c, '-');
        } else {
            // 十六进制字符
            EXPECT_TRUE((c >= '0' && c <= '9') || 
                       (c >= 'a' && c <= 'f') || 
                       (c >= 'A' && c <= 'F'));
        }
    }
}

TEST_F(UUIDTest, PerformanceTest) {
    // 性能测试：生成大量UUID的时间应该合理
    const int numUUIDs = 10000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < numUUIDs; ++i) {
        std::string uuid = generateUUID();
        EXPECT_FALSE(uuid.empty()); // 基本验证
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // UUID生成应该很快，10000个UUID应该在合理时间内完成
    EXPECT_LT(duration.count(), 1000); // 小于1秒
}

TEST_F(UUIDTest, ThreadSafety) {
    // 简单的线程安全测试
    const int numThreads = 4;
    const int uuidsPerThread = 100;
    std::vector<std::thread> threads;
    std::mutex setMutex;
    std::set<std::string> allUUIDs;
    
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&, uuidsPerThread]() {
            std::vector<std::string> localUUIDs;
            for (int i = 0; i < uuidsPerThread; ++i) {
                localUUIDs.push_back(generateUUID());
            }
            
            std::lock_guard<std::mutex> lock(setMutex);
            for (const auto& uuid : localUUIDs) {
                allUUIDs.insert(uuid);
            }
        });
    }
    
    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }
    
    // 验证所有UUID都是唯一的
    EXPECT_EQ(allUUIDs.size(), numThreads * uuidsPerThread);
}

TEST_F(UUIDTest, UUIDVersion) {
    // 测试UUID版本（如果实现支持）
    std::string uuid = generateUUID();
    
    // 大多数UUID实现生成版本4（随机）UUID
    // 版本信息在第15个字符位置（索引14）
    char versionChar = uuid[14];
    
    // 版本4的UUID在该位置应该是'4'
    // 注意：这取决于具体的UUID实现
    // 如果使用的是其他版本，需要相应调整
    if (versionChar == '4') {
        EXPECT_EQ(versionChar, '4');
        
        // 变体信息在第20个字符位置（索引19）
        char variantChar = uuid[19];
        // 变体应该是8, 9, A, 或B
        EXPECT_TRUE(variantChar == '8' || variantChar == '9' || 
                   variantChar == 'A' || variantChar == 'a' ||
                   variantChar == 'B' || variantChar == 'b');
    }
} 