#ifndef PASSENGER_H
#define PASSENGER_H


#include <QtWidgets/QMainWindow>
#include "ui_passenger.h"
#include "query_result.h"
#include <QCalendarWidget>
#include <QDate>
#include <memory>


QT_BEGIN_NAMESPACE
namespace Ui { class PassengerClass; };
QT_END_NAMESPACE

class Passenger : public QMainWindow
{
    Q_OBJECT

public:
    Passenger(QWidget* parent = nullptr);
    ~Passenger();

private slots:
    void flights_add();
    void benefits_add();
    void flights_save();

    void flight_search_btn_clicked();
    void saveTableToFile(QTableWidget* tableWidget);
    void setupComboBoxes();

    void date_input_btn_clicked();
    void set_flight_date(const QDate& date);

private:
    Ui::PassengerClass* _ui;
    QueryResult* _flights;
    std::unique_ptr<QCalendarWidget> _calendar = nullptr;
};

#endif