#ifndef CASHIER_H
#define CASHIER_H

#include <QtWidgets/QMainWindow>
#include "ui_cashier.h"
#include "query_result.h"
#include <QCalendarWidget>
#include <QDate>
#include <memory>


QT_BEGIN_NAMESPACE
namespace Ui { class CashierClass; };
QT_END_NAMESPACE

class Cashier : public QMainWindow
{
    Q_OBJECT

public:
    Cashier(QWidget* parent = nullptr);
    ~Cashier();

private slots:
    void sales_add();
    void flights_add();
    void benefits_add();

    void sales_save();
    void flights_save();
    void benefits_save();

    void passenger_search_btn_clicked();
    void passenger_create_btn_clicked();
    void flight_search_btn_clicked();

    void saveTableToFile(QTableWidget* tableWidget);
    void setupComboBoxes();

    void sell_date_input_btn_clicked();
    void set_sell_date(const QDate& date);

    void date_input_btn_clicked();
    void set_flight_date(const QDate& date);

private:
    Ui::CashierClass* _ui;
    QueryResult * _sales = nullptr, * _flights = nullptr, * _benefits = nullptr;
    std::unique_ptr<QCalendarWidget> _calendar = nullptr;
};

#endif