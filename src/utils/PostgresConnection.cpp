#include "utils/PostgresConnection.h"
#include <iostream>

PostgresConnection::PostgresConnection(const std::string &connectionString)
    : conn(nullptr, PGConnDeleter{})
{
    conn.reset(PQconnectdb(connectionString.c_str()));
    checkConnection();
}

PostgresConnection::~PostgresConnection()
{
    close();
}

void PostgresConnection::checkConnection()
{
    if (PQstatus(conn.get()) != CONNECTION_OK)
    {
        CROW_LOG_ERROR << "Connection error: " << PQerrorMessage(conn.get()) << "\n";
        throw std::runtime_error(PQerrorMessage(conn.get()));
    }
}

bool PostgresConnection::execute(const std::string &query)
{
    return executeImpl(query);
}

bool PostgresConnection::execute(const std::string &query, std::vector<std::vector<std::string>> &results)
{
    PGresult *res = PQexec(conn.get(), query.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        CROW_LOG_ERROR << "Execute error: " << PQerrorMessage(conn.get()) << "\n";
        CROW_LOG_ERROR << "SQL Query: " << query << "\n"; // 输出查询语句
        CROW_LOG_ERROR << "Result status: " << PQresultStatus(res) << "\n"; // 输出执行结果状态
        CROW_LOG_ERROR << "Primary Error Message: " << PQresultErrorField(res, PG_DIAG_MESSAGE_PRIMARY) << "\n";
        PQclear(res);
        return false;
    }

    int rows = PQntuples(res);
    int cols = PQnfields(res);
    // 遍历所有行
    std::vector<std::string> fields;

    for(int i = 0; i < cols; i++)
    {
        fields.push_back(PQfname(res, i));
    }
    results.push_back(fields);

    for (int i = 0; i < rows; ++i)
    {
        std::vector<std::string> row;

        // 遍历所有列
        for (int j = 0; j < cols; ++j)
        {
            // const char *column_name = PQfname(res, j);        // 获取列名
            const char *column_value = PQgetvalue(res, i, j); // 获取列值
            row.push_back(column_value);
        }

        results.push_back(row); // 将行添加到结果集中
    }
    printResults(results);

    PQclear(res);
    return true;
}

bool PostgresConnection::execute(const std::string &query) const
{
    return executeImpl(query);
}

std::vector<std::vector<std::string>> PostgresConnection::query(const std::string &query)
{
    return queryImpl(query);
}

std::vector<std::vector<std::string>> PostgresConnection::query(const std::string &query) const
{
    return queryImpl(query);
}


void PostgresConnection::close()
{
    conn.reset();
}

bool PostgresConnection::isConnected() const
{
    return conn && PQstatus(conn.get()) == CONNECTION_OK;
}


void PostgresConnection::printResults(const std::vector<std::vector<std::string>> &results)
{
    CROW_LOG_INFO << "results rows: " << results.size();
    CROW_LOG_INFO << "results cols: " << results.front().size();
    for(const auto& row : results)
    {
        std::string line;
        for(const auto& s : row)
        {
            line += s + " ";
        }
        CROW_LOG_INFO << line;
    }
}
bool PostgresConnection::executeImpl(const std::string &query) const
{
    PGresult *res = PQexec(conn.get(), query.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        CROW_LOG_ERROR << "Execute error: " << PQerrorMessage(conn.get());
        PQclear(res);
        return false;
    }
    PQclear(res);
    return true;
}

std::vector<std::vector<std::string>> PostgresConnection::queryImpl(const std::string &query) const
{
    PGresult *res = PQexec(conn.get(), query.c_str());
    CROW_LOG_INFO << "Query: " << query.c_str();
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        CROW_LOG_ERROR << "Query error: " << PQerrorMessage(conn.get());
        PQclear(res);
        return {};
    }

    int rows = PQntuples(res);
    int cols = PQnfields(res);
    std::vector<std::vector<std::string>> results(rows + 1, std::vector<std::string>(cols));

    for(int i = 0; i < cols; i++)
    {
        const char* name = PQfname(res, i);
        results[0][i] = name;
    }

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            results[i + 1][j] = PQgetvalue(res, i, j);
        }
    }
    printResults(results);

    PQclear(res);
    return results;
}