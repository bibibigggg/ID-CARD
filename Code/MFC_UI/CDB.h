#pragma once
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <string>

class CLoginDB {
public:
    CLoginDB();
    ~CLoginDB();
    bool Connect();
    void Disconnect();
    bool ExecuteQuery(const std::string& query);
    sql::ResultSet* GetResult();
    bool CheckLogin(const std::string& userID, const std::string& password);
    bool RegisterUser(const std::string& userID, const std::string& password, const std::string& name);
    bool CheckIdUniqueness(const std::string& userID);

protected:
    sql::Connection* m_conn;
    sql::ResultSet* m_result;
    std::string query;
    sql::Statement* stmt;

private:
    std::string m_host;
    std::string m_user;
    std::string m_password;
    std::string m_database;
};
