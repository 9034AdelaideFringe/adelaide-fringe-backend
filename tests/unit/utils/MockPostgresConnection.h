#pragma once

#include "utils/PostgresConnection.h"
#include <gmock/gmock.h>

class MockPostgresConnection {
public:
    MockPostgresConnection() {}
    
    // Non-const methods
    MOCK_METHOD(bool, execute, (const std::string& query));
    MOCK_METHOD(bool, execute, (const std::string& query, std::vector<std::vector<std::string>>& results));
    MOCK_METHOD(std::vector<std::vector<std::string>>, query, (const std::string& query));
    
    // Const methods
    MOCK_METHOD(bool, execute, (const std::string& query), (const));
    MOCK_METHOD(bool, execute, (const std::string& query, std::vector<std::vector<std::string>>& results), (const));
    MOCK_METHOD(std::vector<std::vector<std::string>>, query, (const std::string& query), (const));
    MOCK_METHOD(bool, isConnected, (), (const));
    MOCK_METHOD(void, close, ());
    
    // Additional methods may be needed if any private methods are used in tests
}; 