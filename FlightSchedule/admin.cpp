#include "admin.h"
#include "database.h"
#include "qmessagebox.h"
#include <qfiledialog.h>


Admin::Admin(QWidget* parent) : QMainWindow(parent), _ui(new Ui::AdminClass()) {
	// Инициализация главного окна
	_ui->setupUi(this);

	// Установка флагов размера окна
	setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
	setWindowFlags(windowFlags() & ~(Qt::WindowFullscreenButtonHint | Qt::WindowMaximizeButtonHint));
	setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);
	setWindowModality(Qt::WindowModal);

	airports_add();
	days_of_week_add();
	schedule_add();
	planes_add();
	statuses_add();
	sales_add();
	models_add();
	flights_add();
	benefits_add();

	connect(_ui->upd_airports_table_btn, &QPushButton::clicked, this, &Admin::airports_add);
	connect(_ui->upd_days_of_week_table_btn, &QPushButton::clicked, this, &Admin::days_of_week_add);
	connect(_ui->upd_schedule_table_btn, &QPushButton::clicked, this, &Admin::schedule_add);
	connect(_ui->upd_planes_table_btn, &QPushButton::clicked, this, &Admin::planes_add);
	connect(_ui->upd_statuses_table_btn, &QPushButton::clicked, this, &Admin::statuses_add);
	connect(_ui->upd_sales_table_btn, &QPushButton::clicked, this, &Admin::sales_add);
	connect(_ui->upd_models_table_btn, &QPushButton::clicked, this, &Admin::models_add);
	connect(_ui->upd_flights_table_btn, &QPushButton::clicked, this, &Admin::flights_add);
	connect(_ui->upd_benefits_table_btn, &QPushButton::clicked, this, &Admin::benefits_add);
}

Admin::~Admin() {
	delete _ui;
}


void Admin::saveTableToFile(QTableWidget* tableWidget) {
	QString fileName = QFileDialog::getSaveFileName(nullptr, "Save Excel File", "", "Excel Files (*.xml)");

	if (fileName.isEmpty()) {
		return;
	}

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::warning(nullptr, "Save Excel File", "Cannot open file for writing: " + fileName);
		return;
	}

	QTextStream out(&file);

	// Запись заголовков столбцов
	out << "<?xml version=\"1.0\"?>\n";
	out << "<?mso-application progid=\"Excel.Sheet\"?>\n";
	out << "<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\"\n";
	out << "    xmlns:o=\"urn:schemas-microsoft-com:office:office\"\n";
	out << "    xmlns:x=\"urn:schemas-microsoft-com:office:excel\"\n";
	out << "    xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\"\n";
	out << "    xmlns:html=\"http://www.w3.org/TR/REC-html40\">\n";
	out << "  <Worksheet ss:Name=\"Sheet1\">\n";
	out << "    <Table>\n";

	// Запись заголовков столбцов, пропуская столбец с названием "фото изделия"
	out << "      <Row>\n";
	for (int col = 0; col < tableWidget->columnCount(); ++col) {
		QString header = tableWidget->horizontalHeaderItem(col)->data(Qt::DisplayRole).toString();
		if (header != "Фото изделия") {
			out << "        <Cell><Data ss:Type=\"String\">" << header << "</Data></Cell>\n";
		}
	}
	out << "      </Row>\n";

	// Запись данных из таблицы
	for (int row = 0; row < tableWidget->rowCount(); ++row) {
		out << "      <Row>\n";
		for (int col = 0; col < tableWidget->columnCount(); ++col) {
			QString header = tableWidget->horizontalHeaderItem(col)->data(Qt::DisplayRole).toString();
			if (header != "Фото изделия") {
				QTableWidgetItem* item = tableWidget->item(row, col);
				if (item && item->data(Qt::DisplayRole).isValid()) {
					QString cellValue = item->data(Qt::DisplayRole).toString();
					out << "        <Cell><Data ss:Type=\"String\">" << cellValue << "</Data></Cell>\n";
				}
				else {
					out << "        <Cell><Data ss:Type=\"String\"></Data></Cell>\n";
				}
			}
		}
		out << "      </Row>\n";
	}

	out << "    </Table>\n";
	out << "  </Worksheet>\n";
	out << "</Workbook>\n";

	file.close();
	QMessageBox::information(nullptr, "Save Excel File", "Data saved to " + fileName);
}


void Admin::airports_save() {
	_ui->save_airports_table_btn->setEnabled(false);
	saveTableToFile(_airports->_ui->output_table);
	_ui->save_airports_table_btn->setEnabled(true);
}

void Admin::days_of_week_save() {
	_ui->save_days_of_week_table_btn->setEnabled(false);
	saveTableToFile(_days_of_week->_ui->output_table);
	_ui->save_days_of_week_table_btn->setEnabled(true);
}

void Admin::schedule_save() {
	_ui->save_schedule_table_btn->setEnabled(false);
	saveTableToFile(_schedule->_ui->output_table);
	_ui->save_schedule_table_btn->setEnabled(true);
}

void Admin::planes_save() {
	_ui->save_planes_table_btn->setEnabled(false);
	saveTableToFile(_planes->_ui->output_table);
	_ui->save_planes_table_btn->setEnabled(true);
}

void Admin::statuses_save() {
	_ui->save_statuses_table_btn->setEnabled(false);
	saveTableToFile(_statuses->_ui->output_table);
	_ui->save_statuses_table_btn->setEnabled(true);
}

void Admin::sales_save() {
	_ui->save_sales_table_btn->setEnabled(false);
	saveTableToFile(_sales->_ui->output_table);
	_ui->save_sales_table_btn->setEnabled(true);
}

void Admin::models_save() {
	_ui->save_models_table_btn->setEnabled(false);
	saveTableToFile(_models->_ui->output_table);
	_ui->save_models_table_btn->setEnabled(true);
}

void Admin::flights_save() {
	_ui->save_flights_table_btn->setEnabled(false);
	saveTableToFile(_flights->_ui->output_table);
	_ui->save_flights_table_btn->setEnabled(true);
}

void Admin::benefits_save() {
	_ui->save_benefits_table_btn->setEnabled(false);
	saveTableToFile(_benefits->_ui->output_table);
	_ui->save_benefits_table_btn->setEnabled(true);
}


void Admin::airports_add() {
	_ui->upd_airports_table_btn->setEnabled(false);

	QLayoutItem* child;
	while ((child = _ui->airports_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}

	QString table_name = "`аэропорты`", primary_key_column_name = "`id аэропорта`";

	_airports = new QueryResult(this, table_name, primary_key_column_name);

	sql::ResultSet* result = runQuery("SELECT * FROM `аэропорты`;");
	if (result) {
		_airports->addDataToTable(result);
		delete result;
	}

	_ui->airports_layout->addWidget(_airports);
	_ui->upd_airports_table_btn->setEnabled(true);
}


void Admin::days_of_week_add() {
	_ui->upd_days_of_week_table_btn->setEnabled(false);

	QLayoutItem* child;
	while ((child = _ui->days_of_week_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}

	QString table_name = "`дни_недели`", primary_key_column_name = "`id дня`";

	_days_of_week = new QueryResult(this, table_name, primary_key_column_name);

	sql::ResultSet* result = runQuery("SELECT * FROM `дни_недели`;");
	if (result) {
		_days_of_week->addDataToTable(result);
		delete result;
	}

	_ui->days_of_week_layout->addWidget(_days_of_week);
	_ui->upd_days_of_week_table_btn->setEnabled(true);
}


void Admin::schedule_add() {
	_ui->upd_schedule_table_btn->setEnabled(false);

	QLayoutItem* child;
	while ((child = _ui->schedule_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}

	QString table_name = "`расписание`", primary_key_column_name = "`id расписания`";

	_schedule = new QueryResult(this, table_name, primary_key_column_name);

	sql::ResultSet* result = runQuery("SELECT * FROM `расписание`;");
	if (result) {
		_schedule->addDataToTable(result);
		delete result;
	}

	_ui->schedule_layout->addWidget(_schedule);
	_ui->upd_schedule_table_btn->setEnabled(true);
}


void Admin::planes_add() {
	_ui->upd_planes_table_btn->setEnabled(false);

	QLayoutItem* child;
	while ((child = _ui->planes_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}

	QString table_name = "`самолёты`", primary_key_column_name = "`id самолёта`";

	_planes = new QueryResult(this, table_name, primary_key_column_name);

	sql::ResultSet* result = runQuery("SELECT * FROM `самолёты`;");
	if (result) {
		_planes->addDataToTable(result);
		delete result;
	}

	_ui->planes_layout->addWidget(_planes);
	_ui->upd_planes_table_btn->setEnabled(true);
}


void Admin::statuses_add() {
	_ui->upd_statuses_table_btn->setEnabled(false);

	QLayoutItem* child;
	while ((child = _ui->statuses_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}

	QString table_name = "`статусы_рейсов`", primary_key_column_name = "`id статуса`";

	_statuses = new QueryResult(this, table_name, primary_key_column_name);

	sql::ResultSet* result = runQuery("SELECT * FROM `статусы_рейсов`;");
	if (result) {
		_statuses->addDataToTable(result);
		delete result;
	}

	_ui->statuses_layout->addWidget(_statuses);
	_ui->upd_statuses_table_btn->setEnabled(true);
}


void Admin::sales_add() {
	_ui->upd_sales_table_btn->setEnabled(false);

	QLayoutItem* child;
	while ((child = _ui->sales_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}

	QString table_name = "`ведомость_продаж_билетов`", primary_key_column_name = "`id продажи`";

	_sales = new QueryResult(this, table_name, primary_key_column_name);

	sql::ResultSet* result = runQuery("SELECT * FROM `ведомость_продаж_билетов`;");
	if (result) {
		_sales->addDataToTable(result);
		delete result;
	}

	_ui->sales_layout->addWidget(_sales);
	_ui->upd_sales_table_btn->setEnabled(true);
}


void Admin::models_add() {
	_ui->upd_models_table_btn->setEnabled(false);

	QLayoutItem* child;
	while ((child = _ui->models_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}

	QString table_name = "`модели_самолётов`", primary_key_column_name = "`id модели`";

	_models = new QueryResult(this, table_name, primary_key_column_name);

	sql::ResultSet* result = runQuery("SELECT * FROM `модели_самолётов`;");
	if (result) {
		_models->addDataToTable(result);
		delete result;
	}

	_ui->models_layout->addWidget(_models);
	_ui->upd_models_table_btn->setEnabled(true);
}


void Admin::flights_add() {
	_ui->upd_flights_table_btn->setEnabled(false);

	QLayoutItem* child;
	while ((child = _ui->flights_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}

	QString table_name = "`рейсы`", primary_key_column_name = "`id рейса`";

	_flights = new QueryResult(this, table_name, primary_key_column_name);

	sql::ResultSet* result = runQuery("SELECT * FROM `рейсы`;");
	if (result) {
		_flights->addDataToTable(result);
		delete result;
	}

	_ui->flights_layout->addWidget(_flights);
	_ui->upd_flights_table_btn->setEnabled(true);
}


void Admin::benefits_add() {
	_ui->upd_benefits_table_btn->setEnabled(false);

	QLayoutItem* child;
	while ((child = _ui->benefits_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}

	QString table_name = "`список_льгот`", primary_key_column_name = "`id льготы`";

	_benefits = new QueryResult(this, table_name, primary_key_column_name);

	sql::ResultSet* result = runQuery("SELECT * FROM `список_льгот`;");
	if (result) {
		_benefits->addDataToTable(result);
		delete result;
	}

	_ui->benefits_layout->addWidget(_benefits);
	_ui->upd_benefits_table_btn->setEnabled(true);
}
