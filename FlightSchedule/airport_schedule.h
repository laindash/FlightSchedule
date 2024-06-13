#ifndef AIRPORT_SCHEDULE_H
#define AIRPORT_SCHEDULE_H

#include <QtWidgets/QMainWindow>
#include "ui_airport_schedule.h"
#include "dispatcher.h"
#include "admin.h"
#include "passenger.h"
#include "cashier.h"
#include <memory>


QT_BEGIN_NAMESPACE
namespace Ui { class AirportScheduleClass; };
QT_END_NAMESPACE

class AirportSchedule : public QMainWindow
{
    Q_OBJECT

public:
    AirportSchedule(QWidget *parent = nullptr);
    ~AirportSchedule();

private slots:
    void dispatcher_btn_clicked();
    void admin_btn_clicked();
    void passenger_btn_clicked();
    void cashier_btn_clicked();

private:
    Ui::AirportScheduleClass* _ui;
    std::unique_ptr<Dispatcher> _dispatcher;
    std::unique_ptr<Admin> _admin;
    std::unique_ptr<Passenger> _passenger;
    std::unique_ptr<Cashier> _cashier;
};

#endif