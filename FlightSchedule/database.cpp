#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <iostream>
#include <QString>

// Константы для подключения к серверу
const std::string server = "tcp://localhost:3306";
const std::string username = "root";
const std::string database = "flight_schedule";
const std::string password = "2281";


sql::ResultSet* runQuery(const std::string& query_data) {
    sql::Driver* driver{};
    sql::Connection* con{};
    sql::Statement* query{};
    sql::ResultSet* res{};

    try {
        driver = get_driver_instance();
        con = driver->connect(server, username, password);
        con->setSchema(database);

        query = con->createStatement();
        res = query->executeQuery(query_data);

        // Возвращаем результат запроса
        return res;
    }
    catch (sql::SQLException e) {
        std::cout << "Could not connect to server. Error message: " << e.what() << std::endl;
        return nullptr;
    }
}
