#ifndef DATABASE_H
#define DATABASE_H


#include <vector>
#include <string>
#include <cppconn/prepared_statement.h>


sql::ResultSet* runQuery(const std::string& query_data);


#endif
