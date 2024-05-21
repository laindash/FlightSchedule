#ifndef AIRPORT_SCHEDULE_H
#define AIRPORT_SCHEDULE_H

#include <QtWidgets/QMainWindow>
#include "ui_airport_schedule.h"
#include "dispatcher.h"
#include "admin.h"
#include "passenger.h"


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

private:
    Ui::AirportScheduleClass* _ui;
    Dispatcher* _dispatcher;
    Admin* _admin;
    Passenger* _passenger;

    void enableDispatcherButton();
    void enableAdminButton();
    void enablePassengerButton();
};

#endif