#include "dispatcher.h"
#include "database.h"
#include "qmessagebox.h"


Dispatcher::Dispatcher(QWidget* parent) : QMainWindow(parent), _ui(new Ui::DispatcherClass()), 
	_query_result(nullptr), _dispatcher_queries(nullptr) {
	
	// Инициализация главного окна
	_ui->setupUi(this);

	// Установка флагов размера окна
	setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
	setWindowFlags(windowFlags() & ~(Qt::WindowFullscreenButtonHint | Qt::WindowMaximizeButtonHint));
	setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);

	
}

Dispatcher::~Dispatcher() {
	delete _ui;
}

/*
void Dispatcher::product_types_btn_clicked() {
	_ui->product_types_btn->setEnabled(false);
	this->hide();
	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Dispatcher::enableProductTypesButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT вид_изделия.`Вид изделия` FROM `вид_изделия`;");

	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->product_types_btn->setEnabled(true);
		this->show();
	}
}

void Dispatcher::enableProductTypesButton() {
	_ui->product_types_btn->setEnabled(true);
	this->show();
}


void Dispatcher::materials_btn_clicked() {
	_ui->materials_btn->setEnabled(false);
	this->hide();
	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Dispatcher::enableMaterialsButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT дополнительные_материалы.`Дополнительный материал` FROM `дополнительные_материалы`;");
	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->materials_btn->setEnabled(true);
		this->show();
	}
}

void Dispatcher::enableMaterialsButton() {
	_ui->materials_btn->setEnabled(true);
	this->show();
}


void Dispatcher::metal_btn_clicked() {
	_ui->metal_btn->setEnabled(false);
	this->hide();
	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Dispatcher::enableMetalButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT металл.`Вид металла`, металл.`Количество на складе, гр`, металл.`Цена за грамм, руб` FROM `металл`;");
	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->metal_btn->setEnabled(true);
		this->show();
	}
}

void Dispatcher::enableMetalButton() {
	_ui->metal_btn->setEnabled(true);
	this->show();
}

void Dispatcher::faceting_btn_clicked() {
	_ui->faceting_btn->setEnabled(false);
	this->hide();
	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Dispatcher::enableFacetingButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT огранка.`Вид огранки` FROM `огранка`;");
	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->faceting_btn->setEnabled(true);
		this->show();
	}
}

void Dispatcher::enableFacetingButton() {
	_ui->faceting_btn->setEnabled(true);
	this->show();
}

void Dispatcher::pam_com_btn_clicked() {
	_ui->pam_com_btn->setEnabled(false);
	this->hide();
	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Dispatcher::enablePamComButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT изделие.`Название изделия`, дополнительные_материалы.`Дополнительный материал`, связь_изделие_и_дополнительные_материалы.`Количество, гр` "
		"FROM `изделие` INNER JOIN `связь_изделие_и_дополнительные_материалы` ON изделие.`Код изделия` = связь_изделие_и_дополнительные_материалы.`Изделие` "
		"INNER JOIN `дополнительные_материалы` ON дополнительные_материалы.`Код материала` = связь_изделие_и_дополнительные_материалы.`Дополнительный материал`;");
	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->pam_com_btn->setEnabled(true);
		this->show();
	}
}

void Dispatcher::enablePamComButton() {
	_ui->pam_com_btn->setEnabled(true);
	this->show();
}

void Dispatcher::stages_btn_clicked() {
	_ui->stages_btn->setEnabled(false);
	this->hide();
	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Dispatcher::enableStagesButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT стадии.`Стадия` FROM `стадии`;");
	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->stages_btn->setEnabled(true);
		this->show();
	}
}

void Dispatcher::enableStagesButton() {
	_ui->stages_btn->setEnabled(true);
	this->show();
}

void Dispatcher::products_btn_clicked() {
	_ui->products_btn->setEnabled(false);
	this->hide();
	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Dispatcher::enableProductsButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT Изделие.`Название изделия`, вид_изделия.`Вид изделия`, Изделие.`Цена, руб`, Металл.`Вид металла`, Изделие.`Вес металла, гр`, Камни.`Камень`, "
		"Камни.`Карат`, связь_изделие_и_камень.`Количество камней, шт`, Изделие.`Фото изделия` "
		"FROM Металл INNER JOIN Изделие ON Металл.`Код` = Изделие.`Металл` "
		"INNER JOIN вид_изделия ON вид_изделия.`Код` = изделие.`Вид изделия` "
		"INNER JOIN связь_изделие_и_камень ON связь_изделие_и_камень.`Изделие` = Изделие.`Код изделия` "
		"INNER JOIN камни ON связь_изделие_и_камень.`Камень` = камни.`Код`;");
	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->products_btn->setEnabled(true);
		this->show();
	}
}

void Dispatcher::enableProductsButton() {
	_ui->products_btn->setEnabled(true);
	this->show();
}

void Dispatcher::technologies_btn_clicked() {
	_ui->technologies_btn->setEnabled(false);
	this->hide();
	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Dispatcher::enableTechnologiesButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT изделие.`Название изделия`, стадии.`Стадия`, оборудование.`Оборудование`, технология.`Время, ч`, технология.`Мощность, ватт`, технология.`Температура, °C`, технология.`Цена производства, руб` "
		"FROM `технология` INNER JOIN `изделие` ON изделие.`Код изделия` = технология.`Изделие` "
		"INNER JOIN `стадии` ON стадии.`Номер` = технология.`Стадия` "
		"INNER JOIN `оборудование` ON оборудование.`Код` = технология.`Оборудование`;");

	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->technologies_btn->setEnabled(true);
		this->show();
	}
}

void Dispatcher::enableTechnologiesButton() {
	_ui->technologies_btn->setEnabled(true);
	this->show();
}

void Dispatcher::pas_com_btn_clicked() {
	_ui->pas_com_btn->setEnabled(false);
	this->hide();
	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Dispatcher::enablePasComButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT изделие.`Название изделия`, камни.`Камень`, связь_изделие_и_камень.`Количество камней, шт` "
		"FROM `изделие` INNER JOIN `связь_изделие_и_камень` ON изделие.`Код изделия` = связь_изделие_и_камень.`Изделие` "
		"INNER JOIN `камни` ON камни.`Код` = связь_изделие_и_камень.`Камень`;");

	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->pas_com_btn->setEnabled(true);
		this->show();
	}
}

void Dispatcher::enablePasComButton() {
	_ui->pas_com_btn->setEnabled(true);
	this->show();
}

void Dispatcher::equipment_btn_clicked() {
	_ui->equipment_btn->setEnabled(false);
	this->hide();
	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Dispatcher::enableEquipmentButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT оборудование.`Оборудование` FROM `оборудование`;");

	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->equipment_btn->setEnabled(true);
		this->show();
	}
}

void Dispatcher::enableEquipmentButton() {
	_ui->equipment_btn->setEnabled(true);
	this->show();
}

void Dispatcher::stones_btn_clicked() {
	_ui->stones_btn->setEnabled(false);
	this->hide();
	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Dispatcher::enableStonesButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT камни.`Камень`, камни.`Карат`, камни.`Цена за карат, руб`, огранка.`Вид огранки` "
		"FROM `огранка` INNER JOIN `камни` ON огранка.`Код` = камни.`Вид огранки`;");

	if (result) {
		_query_result->addDataToTable(result);
		delete result;
		_query_result->show();
	}
	else {
		QMessageBox::information(this, "Информация", "Данные не найдены!");
		delete _query_result;
		delete result;
		_ui->stones_btn->setEnabled(true);
		this->show();
	}
}

void Dispatcher::enableStonesButton() {
	_ui->stones_btn->setEnabled(true);
	this->show();
}


void Dispatcher::dispatcher_queries_btn_clicked() {
	_ui->dispatcher_queries_btn->setEnabled(false);
	this->hide();
	_dispatcher_queries = new DispatcherQueries(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_dispatcher_queries, &QObject::destroyed, this, &Dispatcher::enableDispatcherQueriesButton);
	_dispatcher_queries->setAttribute(Qt::WA_DeleteOnClose); //clear memory
	_dispatcher_queries->show();
}

void Dispatcher::enableDispatcherQueriesButton() {
	_ui->dispatcher_queries_btn->setEnabled(true);
	this->show();
}
*/