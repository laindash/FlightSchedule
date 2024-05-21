#include "admin.h"
#include "database.h"
#include "qmessagebox.h"


Admin::Admin(QWidget* parent) : QMainWindow(parent), _ui(new Ui::AdminClass()), _query_result(nullptr), _admin_queries(nullptr) {
	// Инициализация главного окна
	_ui->setupUi(this);

	// Установка флагов размера окна
	setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
	setWindowFlags(windowFlags() & ~(Qt::WindowFullscreenButtonHint | Qt::WindowMaximizeButtonHint));
	setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);

	connect(_ui->airports_btn, &QPushButton::clicked, this, &Admin::airports_btn_clicked);
	connect(_ui->days_of_week_btn, &QPushButton::clicked, this, &Admin::days_of_week_btn_clicked);
	connect(_ui->schedule_btn, &QPushButton::clicked, this, &Admin::schedule_btn_clicked);
	connect(_ui->planes_btn, &QPushButton::clicked, this, &Admin::planes_btn_clicked);
	connect(_ui->statuses_btn, &QPushButton::clicked, this, &Admin::statuses_btn_clicked);
	connect(_ui->sales_btn, &QPushButton::clicked, this, &Admin::sales_btn_clicked);
	connect(_ui->models_btn, &QPushButton::clicked, this, &Admin::models_btn_clicked);
	connect(_ui->flights_btn, &QPushButton::clicked, this, &Admin::flights_btn_clicked);
	connect(_ui->benefits_btn, &QPushButton::clicked, this, &Admin::benefits_btn_clicked);
	connect(_ui->admin_queries_btn, &QPushButton::clicked, this, &Admin::admin_queries_btn_clicked);
}

Admin::~Admin() {
	delete _ui;
}


void Admin::airports_btn_clicked() {
	_ui->airports_btn->setEnabled(false);
	this->hide();
	QString table_name = "`аэропорты`", primary_key_column_name = "`id аэропорта`";

	_query_result = new QueryResult(this, table_name, primary_key_column_name);
	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Admin::enableAirportsButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT * FROM `аэропорты`;");

	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->airports_btn->setEnabled(true);
		this->show();
	}
}

void Admin::enableAirportsButton() {
	_ui->airports_btn->setEnabled(true);
	this->show();
}


void Admin::days_of_week_btn_clicked() {
	_ui->days_of_week_btn->setEnabled(false);
	this->hide();
	QString table_name = "`дни_недели`", primary_key_column_name = "`id дня`";

	_query_result = new QueryResult(this, table_name, primary_key_column_name);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Admin::enableDaysOfWeekButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT * FROM `дни_недели`;");
	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->days_of_week_btn->setEnabled(true);
		this->show();
	}
}

void Admin::enableDaysOfWeekButton() {
	_ui->days_of_week_btn->setEnabled(true);
	this->show();
}


void Admin::schedule_btn_clicked() {
	_ui->schedule_btn->setEnabled(false);
	this->hide();
	QString table_name = "`расписание`", primary_key_column_name = "`id расписания`";

	_query_result = new QueryResult(this, table_name, primary_key_column_name);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Admin::enableScheduleButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT * FROM `расписание`;");
	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->schedule_btn->setEnabled(true);
		this->show();
	}
}

void Admin::enableScheduleButton() {
	_ui->schedule_btn->setEnabled(true);
	this->show();
}

void Admin::planes_btn_clicked() {
	_ui->planes_btn->setEnabled(false);
	this->hide();
	QString table_name = "`самолёты`", primary_key_column_name = "`id самолёта`";

	_query_result = new QueryResult(this, table_name, primary_key_column_name);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Admin::enablePlanesButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT * FROM `самолёты`;");
	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->planes_btn->setEnabled(true);
		this->show();
	}
}

void Admin::enablePlanesButton() {
	_ui->planes_btn->setEnabled(true);
	this->show();
}

void Admin::statuses_btn_clicked() {
	_ui->statuses_btn->setEnabled(false);
	this->hide();
	QString table_name = "`статусы_рейсов`", primary_key_column_name = "`id статуса`";

	_query_result = new QueryResult(this, table_name, primary_key_column_name);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Admin::enableStatusesButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT * FROM `статусы_рейсов`;");
	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->statuses_btn->setEnabled(true);
		this->show();
	}
}

void Admin::enableStatusesButton() {
	_ui->statuses_btn->setEnabled(true);
	this->show();
}

void Admin::sales_btn_clicked() {
	_ui->sales_btn->setEnabled(false);
	this->hide();
	QString table_name = "`ведомость_продаж_билетов`", primary_key_column_name = "`id билета`";

	_query_result = new QueryResult(this, table_name, primary_key_column_name);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Admin::enableSalesButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT * FROM `ведомость_продаж_билетов`;");
	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->sales_btn->setEnabled(true);
		this->show();
	}
}

void Admin::enableSalesButton() {
	_ui->sales_btn->setEnabled(true);
	this->show();
}

void Admin::models_btn_clicked() {
	_ui->models_btn->setEnabled(false);
	this->hide();
	QString table_name = "`модели_самолётов`", primary_key_column_name = "`id модели`";

	_query_result = new QueryResult(this, table_name, primary_key_column_name);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Admin::enableModelsButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT * FROM `модели_самолётов`;");
	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->models_btn->setEnabled(true);
		this->show();
	}
}

void Admin::enableModelsButton() {
	_ui->models_btn->setEnabled(true);
	this->show();
}

void Admin::flights_btn_clicked() {
	_ui->flights_btn->setEnabled(false);
	this->hide();
	QString table_name = "`рейсы`", primary_key_column_name = "`id рейса`";

	_query_result = new QueryResult(this, table_name, primary_key_column_name);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Admin::enableFlightsButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT * FROM `рейсы`;");

	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->flights_btn->setEnabled(true);
		this->show();
	}
}

void Admin::enableFlightsButton() {
	_ui->flights_btn->setEnabled(true);
	this->show();
}

void Admin::benefits_btn_clicked() {
	_ui->benefits_btn->setEnabled(false);
	this->hide();
	QString table_name = "`список_льгот`", primary_key_column_name = "`id льготы`";

	_query_result = new QueryResult(this, table_name, primary_key_column_name);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Admin::enableBenefitsButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT * FROM `список_льгот`;");

	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->benefits_btn->setEnabled(true);
		this->show();
	}
}

void Admin::enableBenefitsButton() {
	_ui->benefits_btn->setEnabled(true);
	this->show();
}


void Admin::admin_queries_btn_clicked() {
	_ui->admin_queries_btn->setEnabled(false);
	this->hide();
	_admin_queries = new AdminQueries(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_admin_queries, &QObject::destroyed, this, &Admin::enableAdminQueriesButton);
	_admin_queries->setAttribute(Qt::WA_DeleteOnClose); //clear memory
	_admin_queries->show();
}

void Admin::enableAdminQueriesButton() {
	_ui->admin_queries_btn->setEnabled(true);
	this->show();
}