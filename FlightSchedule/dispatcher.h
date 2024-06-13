#ifndef DISPATCHER_H
#define DISPATCHER_H


#include <QtWidgets/QMainWindow>
#include "ui_dispatcher.h"
#include "query_result.h"
#include <QCalendarWidget>
#include <QDate>
#include <memory>


QT_BEGIN_NAMESPACE
namespace Ui { class DispatcherClass; };
QT_END_NAMESPACE

class Dispatcher : public QMainWindow
{
    Q_OBJECT

public:
    Dispatcher(QWidget* parent = nullptr);
   ~Dispatcher();

private slots:
    void airports_add();
    void schedule_add();
    void planes_add();
    void flights_add();

    void airports_save();
    void schedule_save();
    void planes_save();
    void flights_save();

    void flight_search_btn_clicked();
    void flight_create_btn_clicked();
    void flight_status_change_btn_clicked();
    void plane_search_btn_clicked();
    void airport_search_btn_clicked();
    void schedule_search_btn_clicked();
    void schedule_create_btn_clicked();

    void saveTableToFile(QTableWidget* tableWidget);
    void setupComboBoxes();

    void date_input_btn_clicked();
    void set_flight_date(const QDate& date);

private:
    Ui::DispatcherClass* _ui;
    QueryResult *_airports = nullptr, * _schedule = nullptr, * _planes = nullptr, * _flights = nullptr;
    std::unique_ptr<QCalendarWidget> _calendar = nullptr;
};

#endif