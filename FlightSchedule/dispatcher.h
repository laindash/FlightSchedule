#ifndef DISPATCHER_H
#define DISPATCHER_H


#include <QtWidgets/QMainWindow>
#include "ui_dispatcher.h"
#include "query_result.h"
#include "dispatcher_queries.h"


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


private:
    Ui::DispatcherClass* _ui;
    QueryResult* _query_result;
    DispatcherQueries* _dispatcher_queries;

};

#endif