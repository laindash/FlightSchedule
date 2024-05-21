#ifndef ADMIN_QUERIES_H
#define ADMIN_QUERIES_H


#include <QtWidgets/QMainWindow>
#include "ui_admin_queries.h"
#include "query_result.h"


QT_BEGIN_NAMESPACE
namespace Ui { class AdminQueriesClass; };
QT_END_NAMESPACE

class AdminQueries : public QMainWindow
{
    Q_OBJECT

public:
    AdminQueries(QWidget* parent = nullptr);
    ~AdminQueries();

private slots:
    void metal_price_btn_clicked();
    void stones_price_btn_clicked();
    void products_price_btn_clicked();
    void upd_metal_price_btn_clicked();
    void upd_stones_price_btn_clicked();
    void upd_products_price_btn_clicked();

private:
    Ui::AdminQueriesClass* _ui;
    QueryResult* _query_result;

    void enableMetalPriceButton();
    void enableStonesPriceButton();
    void enableProductsPriceButton();
    void enableUpdMetalPriceButton();
    void enableUpdStonesPriceButton();
    void enableUpdProductsPriceButton();
};

#endif