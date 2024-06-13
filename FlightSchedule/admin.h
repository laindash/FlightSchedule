#ifndef ADMIN_H
#define ADMIN_H

#include <QtWidgets/QMainWindow>
#include "ui_admin.h"
#include "query_result.h"


QT_BEGIN_NAMESPACE
namespace Ui { class AdminClass; };
QT_END_NAMESPACE

class Admin : public QMainWindow
{
    Q_OBJECT

public:
    Admin(QWidget* parent = nullptr);
    ~Admin();

private slots:
    void airports_add();
    void days_of_week_add();
    void schedule_add();
    void planes_add();
    void statuses_add();
    void sales_add();
    void models_add();
    void flights_add();
    void benefits_add();

    void airports_save();
    void days_of_week_save();
    void schedule_save();
    void planes_save();
    void statuses_save();
    void sales_save();
    void models_save();
    void flights_save();
    void benefits_save();

    void saveTableToFile(QTableWidget* tableWidget);

private:
    Ui::AdminClass *_ui;
    QueryResult *_airports = nullptr, * _days_of_week = nullptr, * _schedule = nullptr, * _planes = nullptr, * _statuses = nullptr,
        * _sales = nullptr, * _models = nullptr, * _flights = nullptr, * _benefits = nullptr;
};

#endif