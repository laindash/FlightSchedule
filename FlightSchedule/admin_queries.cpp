#include "admin_queries.h"
#include "database.h"
#include "qmessagebox.h"
#include <qinputdialog.h>


AdminQueries::AdminQueries(QWidget* parent) : QMainWindow(parent), _ui(new Ui::AdminQueriesClass()), _query_result(nullptr) {
	// Инициализация главного окна
	_ui->setupUi(this);

	// Установка флагов размера окна
	setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
	setWindowFlags(windowFlags() & ~(Qt::WindowFullscreenButtonHint | Qt::WindowMaximizeButtonHint));
	setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);

	connect(_ui->metal_price_btn, &QPushButton::clicked, this, &AdminQueries::metal_price_btn_clicked);
	connect(_ui->stones_price_btn, &QPushButton::clicked, this, &AdminQueries::stones_price_btn_clicked);
	connect(_ui->products_price_btn, &QPushButton::clicked, this, &AdminQueries::products_price_btn_clicked);
	connect(_ui->upd_metal_price_btn, &QPushButton::clicked, this, &AdminQueries::upd_metal_price_btn_clicked);
	connect(_ui->upd_stones_price_btn, &QPushButton::clicked, this, &AdminQueries::upd_stones_price_btn_clicked);
	connect(_ui->upd_products_price_btn, &QPushButton::clicked, this, &AdminQueries::upd_products_price_btn_clicked);
}

AdminQueries::~AdminQueries() {
	delete _ui;
}


void AdminQueries::metal_price_btn_clicked() {
	_ui->metal_price_btn->setEnabled(false);
	this->hide();
	std::string sql_query =
		"SELECT металл.`Вид металла`, металл.`Количество на складе, гр`, металл.`Цена за грамм, руб`, металл.`Количество на складе, гр` * металл.`Цена за грамм, руб` AS `Общая стоимость` "
		"FROM металл ;";

	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &AdminQueries::enableMetalPriceButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery(sql_query);

	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->metal_price_btn->setEnabled(true);
		this->show();
	}
}

void AdminQueries::enableMetalPriceButton() {
	_ui->metal_price_btn->setEnabled(true);
	this->show();
}


void AdminQueries::stones_price_btn_clicked() {
	_ui->stones_price_btn->setEnabled(false);
	this->hide();
	std::string sql_query =
		"SELECT камни.`Камень`, камни.`Карат`, камни.`Цена за карат, руб`, камни.`Карат` * камни.`Цена за карат, руб` AS `Общая стоимость` "
		"FROM камни ;";

	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &AdminQueries::enableStonesPriceButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery(sql_query);

	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->stones_price_btn->setEnabled(true);
		this->show();
	}
}

void AdminQueries::enableStonesPriceButton() {
	_ui->stones_price_btn->setEnabled(true);
	this->show();
}


void AdminQueries::products_price_btn_clicked() {
	_ui->products_price_btn->setEnabled(false);
	this->hide();
	std::string sql_query =
		"SELECT изделие.`Название изделия`, вид_изделия.`Вид изделия`, изделие.`Цена, руб` "
		"FROM `изделие` INNER JOIN `вид_изделия` ON вид_изделия.`Код` = изделие.`Вид изделия` ;";
	
	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &AdminQueries::enableProductsPriceButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery(sql_query);

	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->products_price_btn->setEnabled(true);
		this->show();
	}
}

void AdminQueries::enableProductsPriceButton() {
	_ui->products_price_btn->setEnabled(true);
	this->show();
}


void AdminQueries::upd_metal_price_btn_clicked() {
	_ui->upd_metal_price_btn->setEnabled(false);
	this->hide();
	std::string sql_query =
		"UPDATE металл SET металл.`Цена за грамм, руб` = металл.`Цена за грамм, руб` *  ";

	bool ok;
	QString multiplier = QInputDialog::getText(this, "Введите множитель", "Введите множитель для цены металла за грамм:", QLineEdit::Normal, "1.0", &ok);

	if (ok && !multiplier.isEmpty()) {
		sql_query += multiplier.toStdString() + ";";
	}
	else if (!ok) {
		_ui->upd_metal_price_btn->setEnabled(true);
		this->show();
		return;
	}
	else if (multiplier.isEmpty()) {
		QMessageBox::critical(this, "Ошибка", "Вы ввели пустое значение!");
		_ui->upd_metal_price_btn->setEnabled(true);
		this->show();
		return;
	}
	runQuery(sql_query);

	sql_query =
		"SELECT металл.`Вид металла`, металл.`Количество на складе, гр`, металл.`Цена за грамм, руб`, металл.`Количество на складе, гр` * металл.`Цена за грамм, руб` AS `Общая стоимость` "
		"FROM металл ;";

	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &AdminQueries::enableUpdMetalPriceButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery(sql_query);

	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->upd_metal_price_btn->setEnabled(true);
		this->show();
	}
}

void AdminQueries::enableUpdMetalPriceButton() {
	_ui->upd_metal_price_btn->setEnabled(true);
	this->show();
}


void AdminQueries::upd_stones_price_btn_clicked() {
	_ui->upd_stones_price_btn->setEnabled(false);
	this->hide();
	std::string sql_query =
		"UPDATE камни SET камни.`Цена за карат, руб` = камни.`Цена за карат, руб` *  ";

	bool ok;
	QString multiplier = QInputDialog::getText(this, "Введите множитель", "Введите множитель для цены камня за грамм:", QLineEdit::Normal, "1.0", &ok);

	if (ok && !multiplier.isEmpty()) {
		sql_query += multiplier.toStdString() + ";";
	}
	else if (!ok) {
		_ui->upd_stones_price_btn->setEnabled(true);
		this->show();
		return;
	}
	else if (multiplier.isEmpty()) {
		QMessageBox::critical(this, "Ошибка", "Вы ввели пустое значение!");
		_ui->upd_stones_price_btn->setEnabled(true);
		this->show();
		return;
	}
	runQuery(sql_query);

	sql_query =
		"SELECT камни.`Камень`, камни.`Карат`, камни.`Цена за карат, руб`, камни.`Карат` * камни.`Цена за карат, руб` AS `Общая стоимость` "
		"FROM камни ;";

	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &AdminQueries::enableUpdStonesPriceButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery(sql_query);

	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->upd_stones_price_btn->setEnabled(true);
		this->show();
	}
}

void AdminQueries::enableUpdStonesPriceButton() {
	_ui->upd_stones_price_btn->setEnabled(true);
	this->show();
}


void AdminQueries::upd_products_price_btn_clicked() {
	_ui->upd_products_price_btn->setEnabled(false);
	this->hide();
	std::string sql_query =
		"UPDATE изделие SET изделие.`Цена, руб` = изделие.`Цена, руб` *  ";

	bool ok;
	QString multiplier = QInputDialog::getText(this, "Введите множитель", "Введите множитель для цены изделия:", QLineEdit::Normal, "1.0", &ok);

	if (ok && !multiplier.isEmpty()) {
		sql_query += multiplier.toStdString() + ";";
	}
	else if (!ok) {
		_ui->upd_products_price_btn->setEnabled(true);
		this->show();
		return;
	}
	else if (multiplier.isEmpty()) {
		QMessageBox::critical(this, "Ошибка", "Вы ввели пустое значение!");
		_ui->upd_products_price_btn->setEnabled(true);
		this->show();
		return;
	}
	runQuery(sql_query);

	sql_query =
		"SELECT изделие.`Название изделия`, вид_изделия.`Вид изделия`, изделие.`Цена, руб` "
		"FROM `изделие` INNER JOIN `вид_изделия` ON вид_изделия.`Код` = изделие.`Вид изделия` ;";

	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &AdminQueries::enableUpdProductsPriceButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery(sql_query);

	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->upd_products_price_btn->setEnabled(true);
		this->show();
	}
}

void AdminQueries::enableUpdProductsPriceButton() {
	_ui->upd_products_price_btn->setEnabled(true);
	this->show();
}