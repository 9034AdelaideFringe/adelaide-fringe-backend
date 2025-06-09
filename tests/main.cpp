#include <gtest/gtest.h>
#include "utils/Config.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    // Load test configuration
    try {
        Config::loadConfig("test_config.json");
        std::cout << "Test config loaded successfully" << std::endl;
        std::cout << "Database config: " << Config::get("database") << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Warning: Could not load test config: " << e.what() << std::endl;
        // Continue with tests anyway
    }
    
    return RUN_ALL_TESTS();
} 