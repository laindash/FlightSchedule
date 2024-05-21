#ifndef PASSENGER_H
#define PASSENGER_H


#include <QtWidgets/QMainWindow>
#include "ui_passenger.h"
#include "query_result.h"


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
    void airports_btn_clicked();
    void planes_btn_clicked();
    void flights_btn_clicked();
    void benefits_btn_clicked();

private:
    Ui::PassengerClass* _ui;
    QueryResult* _query_result;

    void enableAirportsButton();
    void enablePlanesButton();
    void enableFlightsButton();
    void enableBenefitsButton();
};

#endif