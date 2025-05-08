#pragma once

#include "utils/PostgresConnection.h"
#include <gmock/gmock.h>

class MockPostgresConnection : public PostgresConnection {
public:
    MockPostgresConnection() : PostgresConnection() {}
    
    // Non-const methods
    MOCK_METHOD(bool, execute, (const std::string& query), (override));
    MOCK_METHOD(bool, execute, (const std::string& query, std::vector<std::vector<std::string>>& results), (override));
    MOCK_METHOD(std::vector<std::vector<std::string>>, query, (const std::string& query), (override));
    
    // Const methods
    MOCK_METHOD(bool, execute, (const std::string& query), (const, override));
    MOCK_METHOD(bool, execute, (const std::string& query, std::vector<std::vector<std::string>>& results), (const, override));
    MOCK_METHOD(std::vector<std::vector<std::string>>, query, (const std::string& query), (const, override));
    MOCK_METHOD(bool, isConnected, (), (const, override));
    
    // Additional methods may be needed if any private methods are used in tests
}; 