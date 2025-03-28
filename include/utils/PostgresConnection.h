#pragma once

#include "postgresql/libpq-fe.h"
#include <string>
#include <stdexcept>
#include <vector>
#include <memory>
#include "crow.h"

class PostgresConnection
{
public:
    PostgresConnection(const std::string &connectionString);
    ~PostgresConnection();

    bool execute(const std::string& query);
    bool execute(const std::string& query) const;

    bool execute(const std::string& query, std::vector<std::vector<std::string>>& results);


    std::vector<std::vector<std::string>> query(const std::string &query);
    std::vector<std::vector<std::string>> query(const std::string &query) const;


    void close();
    bool isConnected() const;

private:
    struct PGConnDeleter
    {
        void operator()(PGconn *conn) const
        {
            if (conn)
                PQfinish(conn);
        }
    };
    std::unique_ptr<PGconn, PGConnDeleter> conn;
    void checkConnection();

    static void printResults(const std::vector<std::vector<std::string>> &results);

    bool executeImpl(const std::string& query) const;
    std::vector<std::vector<std::string>> queryImpl(const std::string& query) const;

};