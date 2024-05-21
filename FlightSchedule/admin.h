#ifndef ADMIN_H
#define ADMIN_H

#include <QtWidgets/QMainWindow>
#include "ui_admin.h"
#include "query_result.h"
#include "admin_queries.h"


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
    void airports_btn_clicked();
    void days_of_week_btn_clicked();
    void schedule_btn_clicked();
    void planes_btn_clicked();
    void statuses_btn_clicked();
    void sales_btn_clicked();
    void models_btn_clicked();
    void flights_btn_clicked();
    void benefits_btn_clicked();
    void admin_queries_btn_clicked();

private:
    Ui::AdminClass* _ui;
    QueryResult* _query_result;
    AdminQueries* _admin_queries;
    
    void enableAirportsButton();
    void enableDaysOfWeekButton();
    void enableScheduleButton();
    void enablePlanesButton();
    void enableStatusesButton();
    void enableSalesButton();
    void enableModelsButton();
    void enableFlightsButton();
    void enableBenefitsButton();
    void enableAdminQueriesButton();
};

#endif