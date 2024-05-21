#ifndef QUERY_RESULT_H
#define QUERY_RESULT_H


#include <QMainWindow>
#include "ui_query_result.h"
#include "database.h"
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>


QT_BEGIN_NAMESPACE
namespace Ui { class QueryResultClass; };
QT_END_NAMESPACE

class QueryResult : public QMainWindow {
	Q_OBJECT


public:
	QueryResult(QWidget *parent = nullptr);
	QueryResult(QWidget* parent, QString& table_name, QString& primary_key_column_name);
	~QueryResult();

	void addDataToTable(sql::ResultSet* resultSet);

private slots:
	void saveDataInDB();
	void addNewRow();
	void deleteRow();

private:
	Ui::QueryResultClass *_ui;
	QString _table_name{}, _primary_key_column_name{};

	void contextMenuEvent(QContextMenuEvent* event);
	void editSize();
};


#endif