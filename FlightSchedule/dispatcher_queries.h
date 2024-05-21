#ifndef DISPATCHER_QUERIES_H
#define DISPATCHER_QUERIES_H


#include <QtWidgets/QMainWindow>
#include "ui_dispatcher_queries.h"
#include "query_result.h"


QT_BEGIN_NAMESPACE
namespace Ui { class DispatcherQueriesClass; };
QT_END_NAMESPACE

class DispatcherQueries : public QMainWindow
{
    Q_OBJECT

public:
    DispatcherQueries(QWidget* parent = nullptr);
    ~DispatcherQueries();

private slots:
    void stone_availability_btn_clicked();
    void metal_availability_btn_clicked();
    void param_search_btn_clicked();

private:
    Ui::DispatcherQueriesClass* _ui;
    QueryResult* _query_result;


    void enableStoneAvailabilityButton();
    void enableMetalAvailabilityButton();
    void enableParamSearchButton();
};

#endif