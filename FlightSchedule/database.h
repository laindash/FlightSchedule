#ifndef DATABASE_H
#define DATABASE_H


#include <vector>
#include <string>
#include <cppconn/prepared_statement.h>
#include <QComboBox>


sql::ResultSet* runQuery(const std::string& query_data);
QStringList getDataFromTable(const std::string& query);
QComboBox* createComboBox(const QStringList& dataList);
void setupComboBox(QComboBox* comboBox, const QStringList& dataList);
std::string getCurrentDateAndTime();

#endif
