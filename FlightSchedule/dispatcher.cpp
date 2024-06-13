#include "dispatcher.h"
#include "database.h"
#include "qmessagebox.h"
#include <qfiledialog.h>
#include <qtablewidget.h>
#include <QTextCharFormat>


Dispatcher::Dispatcher(QWidget* parent) : QMainWindow(parent), _ui(new Ui::DispatcherClass()) {
	// Инициализация главного окна
	_ui->setupUi(this);

	// Установка флагов размера окна
	setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
	setWindowFlags(windowFlags() & ~(Qt::WindowFullscreenButtonHint | Qt::WindowMaximizeButtonHint));
	setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);
	setWindowModality(Qt::WindowModal);

	airports_add();
	schedule_add();
	planes_add();
	flights_add();

	setupComboBoxes();

	connect(_ui->upd_airports_table_btn, &QPushButton::clicked, this, &Dispatcher::airports_add);
	connect(_ui->upd_schedule_table_btn, &QPushButton::clicked, this, &Dispatcher::schedule_add);
	connect(_ui->upd_planes_table_btn, &QPushButton::clicked, this, &Dispatcher::planes_add);
	connect(_ui->upd_flights_table_btn, &QPushButton::clicked, this, &Dispatcher::flights_add);

	connect(_ui->save_airports_table_btn, &QPushButton::clicked, this, &Dispatcher::airports_save);
	connect(_ui->save_schedule_table_btn, &QPushButton::clicked, this, &Dispatcher::schedule_save);
	connect(_ui->save_planes_table_btn, &QPushButton::clicked, this, &Dispatcher::planes_save);
	connect(_ui->save_flights_table_btn, &QPushButton::clicked, this, &Dispatcher::flights_save);

	connect(_ui->flight_search_btn, &QPushButton::clicked, this, &Dispatcher::flight_search_btn_clicked);
	connect(_ui->flight_create_btn, &QPushButton::clicked, this, &Dispatcher::flight_create_btn_clicked);
	connect(_ui->flight_status_change_btn, &QPushButton::clicked, this, &Dispatcher::flight_status_change_btn_clicked);
	connect(_ui->plane_search_btn, &QPushButton::clicked, this, &Dispatcher::plane_search_btn_clicked);
	connect(_ui->airport_search_btn, &QPushButton::clicked, this, &Dispatcher::airport_search_btn_clicked);
	connect(_ui->schedule_search_btn, &QPushButton::clicked, this, &Dispatcher::schedule_search_btn_clicked);
	connect(_ui->schedule_create_btn, &QPushButton::clicked, this, &Dispatcher::schedule_create_btn_clicked);

	connect(_ui->upd_airports_inputs_btn, &QPushButton::clicked, this, &Dispatcher::setupComboBoxes);
	connect(_ui->upd_schedule_inputs_btn, &QPushButton::clicked, this, &Dispatcher::setupComboBoxes);
	connect(_ui->upd_planes_inputs_btn, &QPushButton::clicked, this, &Dispatcher::setupComboBoxes);
	connect(_ui->upd_flights_inputs_btn, &QPushButton::clicked, this, &Dispatcher::setupComboBoxes);
	connect(_ui->date_input_btn, &QPushButton::clicked, this, &Dispatcher::date_input_btn_clicked);
}

Dispatcher::~Dispatcher() {
	delete _ui;
}


void Dispatcher::saveTableToFile(QTableWidget* tableWidget) {
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


void Dispatcher::airports_save() {
	_ui->save_airports_table_btn->setEnabled(false);
	saveTableToFile(_airports->_ui->output_table);
	_ui->save_airports_table_btn->setEnabled(true);
}

void Dispatcher::schedule_save() {
	_ui->save_schedule_table_btn->setEnabled(false);
	saveTableToFile(_schedule->_ui->output_table);
	_ui->save_schedule_table_btn->setEnabled(true);
}

void Dispatcher::planes_save() {
	_ui->save_planes_table_btn->setEnabled(false);
	saveTableToFile(_planes->_ui->output_table);
	_ui->save_planes_table_btn->setEnabled(true);
}

void Dispatcher::flights_save() {
	_ui->save_flights_table_btn->setEnabled(false);
	saveTableToFile(_flights->_ui->output_table);
	_ui->save_flights_table_btn->setEnabled(true);
}


void Dispatcher::airports_add() {
	_ui->upd_airports_table_btn->setEnabled(false);

	QLayoutItem* child;
	while ((child = _ui->airports_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}

	_airports = new QueryResult(this);

	sql::ResultSet* result = runQuery("SELECT аэропорты.`Код аэропорта`, аэропорты.`Название аэропорта`, аэропорты.`Город` FROM `аэропорты` \
		ORDER BY аэропорты.`id аэропорта`;");
	if (result) {
		_airports->addDataToTable(result);
		delete result;
	}

	_ui->airports_layout->addWidget(_airports);
	_ui->upd_airports_table_btn->setEnabled(true);
}


void Dispatcher::schedule_add() {
	_ui->upd_schedule_table_btn->setEnabled(false);

	QLayoutItem* child;
	while ((child = _ui->schedule_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}

	QString table_name = "`расписание`", primary_key_column_name = "`id расписания`";

	_schedule = new QueryResult(this, table_name, primary_key_column_name, true);

	sql::ResultSet* result = runQuery("SELECT \
		дни_недели.`День недели`, \
		расписание.`Время отправления`, \
		расписание.`Время прибытия`, \
		аэропорты_отправления.`Код аэропорта` AS `Код 1`, \
		аэропорты_отправления.`Название аэропорта` AS `Аэропорт отправления`, \
		аэропорты_отправления.`Город` AS `Город отправления`, \
		аэропорты_прибытия.`Код аэропорта` AS `Код 2`, \
		аэропорты_прибытия.`Название аэропорта` AS `Аэропорт прибытия`, \
		аэропорты_прибытия.`Город` AS `Город прибытия` \
		FROM \
		дни_недели \
		INNER JOIN \
		расписание ON дни_недели.`id дня` = расписание.`День недели` \
		INNER JOIN \
		аэропорты AS аэропорты_отправления ON расписание.`Точка отправления` = аэропорты_отправления.`id аэропорта` \
		INNER JOIN \
		аэропорты AS аэропорты_прибытия ON расписание.`Точка прибытия` = аэропорты_прибытия.`id аэропорта` \
		ORDER BY расписание.`id расписания`;");

	if (result) {
		_schedule->addDataToTable(result);
		delete result;
	}

	_ui->schedule_layout->addWidget(_schedule);
	_ui->upd_schedule_table_btn->setEnabled(true);
}


void Dispatcher::planes_add() {
	_ui->upd_planes_table_btn->setEnabled(false);

	QLayoutItem* child;
	while ((child = _ui->planes_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}

	_planes = new QueryResult(this);

	sql::ResultSet* result = runQuery("SELECT самолёты.`Номер самолёта`, модели_самолётов.`Название модели`, модели_самолётов.`Количество посадочных мест`, модели_самолётов.`Грузоподъёмность, т`, самолёты.`Год выпуска` "
		"FROM `самолёты` INNER JOIN `модели_самолётов` ON самолёты.`Модель` = модели_самолётов.`id модели`;");
	if (result) {
		_planes->addDataToTable(result);
		delete result;
	}

	_ui->planes_layout->addWidget(_planes);
	_ui->upd_planes_table_btn->setEnabled(true);
}


void Dispatcher::flights_add() {
	_ui->upd_flights_table_btn->setEnabled(false);

	QLayoutItem* child;
	while ((child = _ui->flights_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}
	QString table_name = "`рейсы`", primary_key_column_name = "`id рейса`";

	_flights = new QueryResult(this, table_name, primary_key_column_name, true);

	sql::ResultSet* result = runQuery("SELECT \
		рейсы.`Номер рейса` AS `Рейс`, \
		статусы_рейсов.`Статус`, \
		дни_недели.`День недели`, \
		рейсы.`Дата`, \
		расписание.`Время отправления`, \
		расписание.`Время прибытия`, \
		аэропорты_отправления.`Код аэропорта` AS `Код 1`, \
		аэропорты_отправления.`Название аэропорта` AS `Аэропорт отправления`, \
		аэропорты_отправления.`Город` AS `Город отправления`, \
		аэропорты_прибытия.`Код аэропорта` AS `Код 2`, \
		аэропорты_прибытия.`Название аэропорта` AS `Аэропорт прибытия`, \
		аэропорты_прибытия.`Город` AS `Город прибытия`, \
		модели_самолётов.`Название модели` AS `Самолёт` \
		FROM \
		дни_недели \
		INNER JOIN \
		расписание ON дни_недели.`id дня` = расписание.`День недели` \
		INNER JOIN \
		рейсы ON рейсы.`Расписание` = расписание.`id расписания` \
		INNER JOIN \
		самолёты ON рейсы.`Самолёт` = самолёты.`id самолёта` \
		INNER JOIN \
		модели_самолётов ON самолёты.`Модель` = модели_самолётов.`id модели` \
		INNER JOIN \
		аэропорты AS аэропорты_отправления ON расписание.`Точка отправления` = аэропорты_отправления.`id аэропорта` \
		INNER JOIN \
		аэропорты AS аэропорты_прибытия ON расписание.`Точка прибытия` = аэропорты_прибытия.`id аэропорта` \
		INNER JOIN \
		статусы_рейсов ON рейсы.`Статус` = статусы_рейсов.`id статуса` \
		ORDER BY рейсы.`id Рейса`;");

	if (result) {
		_flights->addDataToTable(result);
		delete result;
	}

	_ui->flights_layout->addWidget(_flights);
	_ui->upd_flights_table_btn->setEnabled(true);
}


void Dispatcher::flight_search_btn_clicked() {
	_ui->flight_search_btn->setEnabled(false);

	std::string sql_query("SELECT \
		рейсы.`Номер рейса` AS `Рейс`, \
		статусы_рейсов.`Статус`, \
		дни_недели.`День недели`, \
		рейсы.`Дата`, \
		расписание.`Время отправления`, \
		расписание.`Время прибытия`, \
		аэропорты_отправления.`Код аэропорта` AS `Код 1`, \
		аэропорты_отправления.`Название аэропорта` AS `Аэропорт отправления`, \
		аэропорты_отправления.`Город` AS `Город отправления`, \
		аэропорты_прибытия.`Код аэропорта` AS `Код 2`, \
		аэропорты_прибытия.`Название аэропорта` AS `Аэропорт прибытия`, \
		аэропорты_прибытия.`Город` AS `Город прибытия`, \
		модели_самолётов.`Название модели` AS `Самолёт` \
		FROM \
		дни_недели \
		INNER JOIN \
		расписание ON дни_недели.`id дня` = расписание.`День недели` \
		INNER JOIN \
		рейсы ON рейсы.`Расписание` = расписание.`id расписания` \
		INNER JOIN \
		самолёты ON рейсы.`Самолёт` = самолёты.`id самолёта` \
		INNER JOIN \
		модели_самолётов ON самолёты.`Модель` = модели_самолётов.`id модели` \
		INNER JOIN \
		аэропорты AS аэропорты_отправления ON расписание.`Точка отправления` = аэропорты_отправления.`id аэропорта` \
		INNER JOIN \
		аэропорты AS аэропорты_прибытия ON расписание.`Точка прибытия` = аэропорты_прибытия.`id аэропорта` \
		INNER JOIN \
		статусы_рейсов ON рейсы.`Статус` = статусы_рейсов.`id статуса` ");
	std::string condition = "WHERE ";

	if (!_ui->from->currentText().isEmpty()) {
		QString from = _ui->from->currentText();

		if (!from.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "аэропорты_отправления.`Город` LIKE '" + from.toStdString() + "%'";
		}
	}

	if (!_ui->to->currentText().isEmpty()) {
		QString to = _ui->to->currentText();

		if (!to.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "аэропорты_прибытия.`Город` LIKE '" + to.toStdString() + "%'";
		}
	}

	if (!_ui->when->currentText().isEmpty()) {
		QString when = _ui->when->currentText();

		if (!when.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "дни_недели.`День недели` LIKE '" + when.toStdString() + "%'";
		}
	}

	if (!_ui->flight_date->currentText().isEmpty()) {
		QString flight_date = _ui->flight_date->currentText();

		if (!flight_date.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "рейсы.`Дата` LIKE '" + flight_date.toStdString() + "%'";
		}
	}

	if (!_ui->status->currentText().isEmpty()) {
		QString status = _ui->status->currentText();

		if (!status.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "статусы_рейсов.`Статус` LIKE '" + status.toStdString() + "%'";
		}
	}

	if (!_ui->model_2->currentText().isEmpty()) {
		QString model_2 = _ui->model_2->currentText();

		if (!model_2.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "модели_самолётов.`Название модели` LIKE '" + model_2.toStdString() + "%'";
		}
	}

	if (!_ui->flight_num->currentText().isEmpty()) {
		QString flight_num = _ui->flight_num->currentText();

		if (!flight_num.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "рейсы.`Номер рейса` LIKE '" + flight_num.toStdString() + "%'";
		}
	}

	if (!(_ui->from->currentText().isEmpty() && _ui->to->currentText().isEmpty() && _ui->when->currentText().isEmpty() && 
		_ui->flight_date->currentText().isEmpty() && _ui->status->currentText().isEmpty() &&
		_ui->model_2->currentText().isEmpty() && _ui->flight_num->currentText().isEmpty())) {
		sql_query += condition + ';';
	}
	else {
		sql_query += "ORDER BY рейсы.`id Рейса`;";
	}

	QLayoutItem* child;
	while ((child = _ui->flights_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}

	_flights = new QueryResult(this);

	sql::ResultSet* result = runQuery(sql_query);

	if (result) {
		_flights->addDataToTable(result);
		delete result;
	}

	_ui->flights_layout->addWidget(_flights);
	_ui->flight_search_btn->setEnabled(true);
}


void Dispatcher::flight_create_btn_clicked() {
	_ui->flight_create_btn->setEnabled(false);

	QString flight_num = _ui->flight_num->currentText();
	QString schedule = _ui->schedule->currentText();
	QString flight_date = _ui->flight_date->currentText();
	QString model = _ui->model_2->currentText();
	QString status = _ui->status->currentText();

	if (flight_num.isEmpty() || schedule.isEmpty() || flight_date.isEmpty() ||
		model.isEmpty() || status.isEmpty()) {
		QMessageBox::warning(this, "Ошибка", "Пожалуйста, заполните все поля перед вставкой данных!");
		_ui->flight_create_btn->setEnabled(true);
		return;
	}

	try {
		QLayoutItem* child;

		while ((child = _ui->flights_layout->takeAt(0)) != nullptr) {
			delete child->widget(); // Удаляем виджеты
			delete child; // Освобождаем память после элемента компоновки
		}

		_flights = new QueryResult(this);

		std::unique_ptr<sql::ResultSet> result(runQuery(
			"SELECT \
			рейсы.`Номер рейса` AS `Рейс`, \
			статусы_рейсов.`Статус`, \
			дни_недели.`День недели`, \
			рейсы.`Дата`, \
			расписание.`Время отправления`, \
			расписание.`Время прибытия`, \
			аэропорты_отправления.`Код аэропорта` AS `Код 1`, \
			аэропорты_отправления.`Название аэропорта` AS `Аэропорт отправления`, \
			аэропорты_отправления.`Город` AS `Город отправления`, \
			аэропорты_прибытия.`Код аэропорта` AS `Код 2`, \
			аэропорты_прибытия.`Название аэропорта` AS `Аэропорт прибытия`, \
			аэропорты_прибытия.`Город` AS `Город прибытия`, \
			модели_самолётов.`Название модели` AS `Самолёт` \
			FROM \
			дни_недели \
			INNER JOIN \
			расписание ON дни_недели.`id дня` = расписание.`День недели` \
			INNER JOIN \
			рейсы ON рейсы.`Расписание` = расписание.`id расписания` \
			INNER JOIN \
			самолёты ON рейсы.`Самолёт` = самолёты.`id самолёта` \
			INNER JOIN \
			модели_самолётов ON самолёты.`Модель` = модели_самолётов.`id модели` \
			INNER JOIN \
			аэропорты AS аэропорты_отправления ON расписание.`Точка отправления` = аэропорты_отправления.`id аэропорта` \
			INNER JOIN \
			аэропорты AS аэропорты_прибытия ON расписание.`Точка прибытия` = аэропорты_прибытия.`id аэропорта` \
			INNER JOIN \
			статусы_рейсов ON рейсы.`Статус` = статусы_рейсов.`id статуса` "
		));

		if (result) {
			_flights->addDataToTable(result.get());
		}

		_ui->flights_layout->addWidget(_flights);

		QStringList schedule_values = schedule.split(' ');

		// Получаем ID существующего дня недели
		std::string day_of_week_query = "SELECT `id дня` FROM `дни_недели` WHERE `День недели` = '" + schedule_values[0].toStdString() + "';";
		std::unique_ptr<sql::ResultSet> day_of_week_result(runQuery(day_of_week_query));
		int day_of_week_id = 0;
		if (day_of_week_result->next()) {
			day_of_week_id = day_of_week_result->getInt(1);
		}
		else {
			QMessageBox::critical(this, "Ошибка", "День недели не найден!");
			_ui->flight_create_btn->setEnabled(true);
			return;
		}

		// Получаем точку отправления
		std::string departure_point_query = "SELECT `id аэропорта` FROM `аэропорты` WHERE `Код аэропорта` = '" + schedule_values[3].toStdString() + "';";
		std::unique_ptr<sql::ResultSet> departure_point_result(runQuery(departure_point_query));
		int departure_point_id = 0;
		if (departure_point_result->next()) {
			departure_point_id = departure_point_result->getInt(1);
		}
		else {
			QMessageBox::critical(this, "Ошибка", "Аэропорт отправления не найден!");
			_ui->flight_create_btn->setEnabled(true);
			return;
		}

		// Получаем точку прибытия
		std::string arrival_point_query = "SELECT `id аэропорта` FROM `аэропорты` WHERE `Код аэропорта` = '" + schedule_values[6].toStdString() + "';";
		std::unique_ptr<sql::ResultSet> arrival_point_result(runQuery(arrival_point_query));
		int arrival_point_id = 0;
		if (arrival_point_result->next()) {
			arrival_point_id = arrival_point_result->getInt(1);
		}
		else {
			QMessageBox::critical(this, "Ошибка", "Аэропорт прибытия не найден!");
			_ui->flight_create_btn->setEnabled(true);
			return;
		}

		// Получаем ID расписания
		std::string schedule_query = "SELECT `id расписания` FROM `расписание` WHERE `День недели` = '" + std::to_string(day_of_week_id) +
		"' AND `Время отправления` = '" + schedule_values[1].toStdString() + "' AND `Время прибытия` = '" + schedule_values[2].toStdString() +
		"' AND `Точка отправления` = '" + std::to_string(departure_point_id) + "' AND `Точка прибытия` = '" + std::to_string(arrival_point_id) + "';";
		std::unique_ptr<sql::ResultSet> schedule_result(runQuery(schedule_query));
		int schedule_id = 0;
		if (schedule_result->next()) {
			schedule_id = schedule_result->getInt(1);
		}
		else {
			QMessageBox::critical(this, "Ошибка", "Расписание не найдено!");
			_ui->flight_create_btn->setEnabled(true);
			return;
		}

		// Получаем дату
		std::string plane_query = "SELECT `id самолёта` FROM `самолёты` WHERE `Модель` = (SELECT `id модели` FROM `модели_самолётов` WHERE \
		`Название модели` = '" + model.toStdString() + "');";
		std::unique_ptr<sql::ResultSet> plane_result(runQuery(plane_query));
		int plane_id = 0;
		if (plane_result->next()) {
			plane_id = plane_result->getInt(1);
		}
		else {
			QMessageBox::critical(this, "Ошибка", "Самолёт не найден!");
			_ui->flight_create_btn->setEnabled(true);
			return;
		}

		// Получаем статус
		std::string status_query = "SELECT `id статуса` FROM `статусы_рейсов` WHERE `Статус` = '" + status.toStdString() + "';";
		std::unique_ptr<sql::ResultSet> status_result(runQuery(status_query));
		int status_id = 0;
		if (status_result->next()) {
			status_id = status_result->getInt(1);
		}
		else {
			QMessageBox::critical(this, "Ошибка", "Статус не выбран!");
			_ui->flight_create_btn->setEnabled(true);
			return;
		}

		// Получаем количество строк в таблице `рейсы`
		int row_count = _flights->_ui->output_table->rowCount();

		// Определяем следующий ID
		int next_id = row_count + 1;

		// Вставляем данные в таблицу `рейсы`
		std::string flights_insert_query =
			"INSERT INTO `рейсы` (`id рейса`, `Номер рейса`, `Расписание`, `Дата`, \
			`Самолёт`, `Статус`) "
			"VALUES (" + std::to_string(next_id) + ", '" + flight_num.toStdString() + "', " + std::to_string(schedule_id) + ", '"
			+ flight_date.toStdString() + "', " + std::to_string(plane_id) + ", " + std::to_string(status_id) + ");";

		runQuery(flights_insert_query);

		flights_add();

		QMessageBox::information(this, "Успех", "Рейс добавлен!");
		_ui->flight_create_btn->setEnabled(true);
	}
	catch (const sql::SQLException& e) {
		QMessageBox::critical(this, "Ошибка", "Ошибка при выполнении запроса: " + QString::fromStdString(e.what()));
		_ui->flight_create_btn->setEnabled(true);
	}
}


void Dispatcher::flight_status_change_btn_clicked() {
	_ui->flight_status_change_btn->setEnabled(false);

	int currentRow = _flights->_ui->output_table->currentRow();

	QString status = _ui->status->currentText();
	std::string status_query = "SELECT `id статуса` FROM `статусы_рейсов` WHERE `Статус` = '" + status.toStdString() + "';";
	std::unique_ptr<sql::ResultSet> status_result(runQuery(status_query));
	int status_id = 0;
	if (status_result->next()) {
		status_id = status_result->getInt(1);
	}
	else {
		QMessageBox::critical(this, "Ошибка", "Статус не выбран!");
		_ui->flight_status_change_btn->setEnabled(true);
		return;
	}

	if (currentRow >= 0) { // Проверка, что выбрана какая-то строка
		QString insertQuery = "UPDATE " + _flights->_table_name + " SET `Статус` = " + QString::number(status_id)
		+= " WHERE (" + _flights->_primary_key_column_name + " = " + "'" + QString::number(currentRow + 1) + "'" + ");";

		// Выполняем SQL-запрос на добавление записи в базу данных
		runQuery(insertQuery.toStdString());
	}
	flights_add();
	_ui->flight_status_change_btn->setEnabled(true);
}


void Dispatcher::plane_search_btn_clicked() {
	_ui->plane_search_btn->setEnabled(false);

	std::string sql_query("SELECT самолёты.`Номер самолёта`, модели_самолётов.`Название модели`, модели_самолётов.`Количество посадочных мест`, модели_самолётов.`Грузоподъёмность, т`, самолёты.`Год выпуска` "
		"FROM `самолёты` INNER JOIN `модели_самолётов` ON самолёты.`Модель` = модели_самолётов.`id модели` ");

	std::string condition = "WHERE ";

	if (!_ui->plane_number->currentText().isEmpty()) {
		QString plane_number = _ui->plane_number->currentText();

		if (!plane_number.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "самолёты.`Номер самолёта` LIKE '" + plane_number.toStdString() + "%'";
		}
	}

	if (!_ui->model->currentText().isEmpty()) {
		QString model = _ui->model->currentText();

		if (!model.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "модели_самолётов.`Название модели` LIKE '" + model.toStdString() + "%'";
		}
	}

	if (!_ui->places->text().isEmpty()) {
		QString places = _ui->places->text();

		if (!places.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "модели_самолётов.`Количество посадочных мест` BETWEEN 0 AND " + places.toStdString() + " ";
		}
	}

	if (!_ui->payload->text().isEmpty()) {
		QString payload = _ui->payload->text();

		if (!payload.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "модели_самолётов.`Грузоподъёмность, т` BETWEEN 0 AND " + payload.toStdString() + " ";
		}
	}

	if (!_ui->production_year->text().isEmpty()) {
		QString production_year = _ui->production_year->text();

		if (!production_year.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "самолёты.`Год выпуска` BETWEEN " + production_year.toStdString() + " AND 3000 ";
		}
	}

	if (!(_ui->plane_number->currentText().isEmpty() && _ui->model->currentText().isEmpty() && _ui->places->text().isEmpty() &&
		_ui->payload->text().isEmpty() && _ui->production_year->text().isEmpty())) {
		sql_query += condition + ';';
	}
	else {
		sql_query += "ORDER BY самолёты.`id самолёта`;";
	}

	QLayoutItem* child;
	while ((child = _ui->planes_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}

	_planes = new QueryResult(this);

	sql::ResultSet* result = runQuery(sql_query);

	if (result) {
		_planes->addDataToTable(result);
		delete result;
	}

	_ui->planes_layout->addWidget(_planes);
	_ui->plane_search_btn->setEnabled(true);
}


void Dispatcher::airport_search_btn_clicked() {
	_ui->airport_search_btn->setEnabled(false);

	std::string sql_query("SELECT аэропорты.`Код аэропорта`, аэропорты.`Название аэропорта`, аэропорты.`Город` FROM `аэропорты` ");
	std::string condition = "WHERE ";

	if (!_ui->code->currentText().isEmpty()) {
		QString code = _ui->code->currentText();

		if (!code.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "аэропорты.`Код аэропорта` LIKE '" + code.toStdString() + "%'";
		}
	}

	if (!_ui->airport->currentText().isEmpty()) {
		QString airport = _ui->airport->currentText();

		if (!airport.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "аэропорты.`Название аэропорта` LIKE '" + airport.toStdString() + "%'";
		}
	}

	if (!_ui->city->currentText().isEmpty()) {
		QString city = _ui->city->currentText();

		if (!city.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "аэропорты.`Город` LIKE '" + city.toStdString() + "%'";
		}
	}


	if (!(_ui->code->currentText().isEmpty() && _ui->airport->currentText().isEmpty() && _ui->city->currentText().isEmpty())) {
		sql_query += condition + ';';
	}
	else {
		sql_query += "ORDER BY аэропорты.`id аэропорта`;";
	}

	QLayoutItem* child;
	while ((child = _ui->airports_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}

	_airports = new QueryResult(this);

	sql::ResultSet* result = runQuery(sql_query);

	if (result) {
		_airports->addDataToTable(result);
		delete result;
	}

	_ui->airports_layout->addWidget(_airports);
	_ui->airport_search_btn->setEnabled(true);
}


void Dispatcher::schedule_search_btn_clicked() {
	_ui->schedule_search_btn->setEnabled(false);

	std::string sql_query("SELECT \
		дни_недели.`День недели`, \
		расписание.`Время отправления`, \
		расписание.`Время прибытия`, \
		аэропорты_отправления.`Код аэропорта` AS `Код 1`, \
		аэропорты_отправления.`Название аэропорта` AS `Аэропорт отправления`, \
		аэропорты_отправления.`Город` AS `Город отправления`, \
		аэропорты_прибытия.`Код аэропорта` AS `Код 2`, \
		аэропорты_прибытия.`Название аэропорта` AS `Аэропорт прибытия`, \
		аэропорты_прибытия.`Город` AS `Город прибытия` \
		FROM \
		дни_недели \
		INNER JOIN \
		расписание ON дни_недели.`id дня` = расписание.`День недели` \
		INNER JOIN \
		аэропорты AS аэропорты_отправления ON расписание.`Точка отправления` = аэропорты_отправления.`id аэропорта` \
		INNER JOIN \
		аэропорты AS аэропорты_прибытия ON расписание.`Точка прибытия` = аэропорты_прибытия.`id аэропорта` ");
	std::string condition = "WHERE ";

	if (!_ui->day_of_week->currentText().isEmpty()) {
		QString day_of_week = _ui->day_of_week->currentText();

		if (!day_of_week.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "дни_недели.`День недели` LIKE '" + day_of_week.toStdString() + "%'";
		}
	}

	if (!_ui->departure_time->currentText().isEmpty()) {
		QString departure_time = _ui->departure_time->currentText();

		if (!departure_time.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "расписание.`Время отправления` LIKE '" + departure_time.toStdString() + "%'";
		}
	}

	if (!_ui->arrival_time->currentText().isEmpty()) {
		QString arrival_time = _ui->arrival_time->currentText();

		if (!arrival_time.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "расписание.`Время прибытия` LIKE '" + arrival_time.toStdString() + "%'";
		}
	}

	if (!_ui->departure_point->currentText().isEmpty()) {
		QString departure_point = _ui->departure_point->currentText();

		if (!departure_point.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "аэропорты_отправления.`Код аэропорта` LIKE '" + departure_point.toStdString() + "%'";
		}
	}

	if (!_ui->arrival_point->currentText().isEmpty()) {
		QString arrival_point = _ui->arrival_point->currentText();

		if (!arrival_point.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "аэропорты_прибытия.`Код аэропорта` LIKE '" + arrival_point.toStdString() + "%'";
		}
	}

	if (!(_ui->day_of_week->currentText().isEmpty() && _ui->departure_time->currentText().isEmpty() && _ui->arrival_time->currentText().isEmpty() 
		&& _ui->departure_point->currentText().isEmpty() && _ui->arrival_point->currentText().isEmpty())) {
		sql_query += condition + ';';
	}
	else {
		sql_query += "ORDER BY расписание.`id расписания`;";
	}

	QLayoutItem* child;
	while ((child = _ui->schedule_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}

	_schedule = new QueryResult(this);

	sql::ResultSet* result = runQuery(sql_query);

	if (result) {
		_schedule->addDataToTable(result);
		delete result;
	}

	_ui->schedule_layout->addWidget(_schedule);
	_ui->schedule_search_btn->setEnabled(true);
}


void Dispatcher::schedule_create_btn_clicked() {
	_ui->schedule_create_btn->setEnabled(false);

	QString day_of_week = _ui->day_of_week->currentText();
	QString departure_time = _ui->departure_time->currentText();
	QString arrival_time = _ui->arrival_time->currentText();
	QString departure_point = _ui->departure_point->currentText();
	QString arrival_point = _ui->arrival_point->currentText();

	if (day_of_week.isEmpty() || departure_time.isEmpty() || arrival_time.isEmpty() ||
		departure_point.isEmpty() || arrival_point.isEmpty()) {
		QMessageBox::warning(this, "Ошибка", "Пожалуйста, заполните все поля перед вставкой данных!");
		_ui->schedule_create_btn->setEnabled(true);
		return;
	}

	try {
		QLayoutItem* child;

		while ((child = _ui->schedule_layout->takeAt(0)) != nullptr) {
			delete child->widget(); // Удаляем виджеты
			delete child; // Освобождаем память после элемента компоновки
		}

		_schedule = new QueryResult(this);

		std::unique_ptr<sql::ResultSet> result(runQuery(
			"SELECT \
			дни_недели.`День недели`, \
			расписание.`Время отправления`, \
			расписание.`Время прибытия`, \
			аэропорты_отправления.`Код аэропорта` AS `Код 1`, \
			аэропорты_отправления.`Название аэропорта` AS `Аэропорт отправления`, \
			аэропорты_отправления.`Город` AS `Город отправления`, \
			аэропорты_прибытия.`Код аэропорта` AS `Код 2`, \
			аэропорты_прибытия.`Название аэропорта` AS `Аэропорт прибытия`, \
			аэропорты_прибытия.`Город` AS `Город прибытия` \
			FROM \
			дни_недели \
			INNER JOIN \
			расписание ON дни_недели.`id дня` = расписание.`День недели` \
			INNER JOIN \
			аэропорты AS аэропорты_отправления ON расписание.`Точка отправления` = аэропорты_отправления.`id аэропорта` \
			INNER JOIN \
			аэропорты AS аэропорты_прибытия ON расписание.`Точка прибытия` = аэропорты_прибытия.`id аэропорта` \
			ORDER BY расписание.`id расписания`;"
		));

		if (result) {
			_schedule->addDataToTable(result.get());
		}

		_ui->schedule_layout->addWidget(_schedule);

		// Получаем ID существующего дня недели
		std::string day_of_week_query = "SELECT `id дня` FROM `дни_недели` WHERE `День недели` = '" + day_of_week.toStdString() + "';";
		std::unique_ptr<sql::ResultSet> day_of_week_result(runQuery(day_of_week_query));
		int day_of_week_id = 0;
		if (day_of_week_result->next()) {
			day_of_week_id = day_of_week_result->getInt(1);
		}
		else {
			QMessageBox::critical(this, "Ошибка", "День недели не найден!");
			_ui->schedule_create_btn->setEnabled(true);
			return;
		}

		// Получаем точку отправления
		std::string departure_point_query = "SELECT `id аэропорта` FROM `аэропорты` WHERE `Код аэропорта` = '" + departure_point.toStdString() + "';";
		std::unique_ptr<sql::ResultSet> departure_point_result(runQuery(departure_point_query));
		int departure_point_id = 0;
		if (departure_point_result->next()) {
			departure_point_id = departure_point_result->getInt(1);
		}
		else {
			QMessageBox::critical(this, "Ошибка", "Аэропорт не найден!");
			_ui->schedule_create_btn->setEnabled(true);
			return;
		}

		// Получаем точку прибытия
		std::string arrival_point_query = "SELECT `id аэропорта` FROM `аэропорты` WHERE `Код аэропорта` = '" + arrival_point.toStdString() + "';";
		std::unique_ptr<sql::ResultSet> arrival_point_result(runQuery(arrival_point_query));
		int arrival_point_id = 0;
		if (arrival_point_result->next()) {
			arrival_point_id = arrival_point_result->getInt(1);
		}
		else {
			QMessageBox::critical(this, "Ошибка", "Аэропорт не найден!");
			_ui->schedule_create_btn->setEnabled(true);
			return;
		}

		// Получаем количество строк в таблице `расписание`
		int row_count = _schedule->_ui->output_table->rowCount();

		// Определяем следующий ID
		int next_id = row_count + 1;

		// Вставляем данные в таблицу `расписание`
		std::string schedule_query =
			"INSERT INTO `расписание` (`id расписания`, `День недели`, `Время отправления`, `Время прибытия`, \
			`Точка отправления`, `Точка прибытия`) "
			"VALUES (" + std::to_string(next_id) + ", " + std::to_string(day_of_week_id) + ", '" + departure_time.toStdString() + "', '"
			+ arrival_time.toStdString() + "', " + std::to_string(departure_point_id) + ", " + std::to_string(arrival_point_id) + ");";

		runQuery(schedule_query);

		schedule_add();

		QMessageBox::information(this, "Успех", "Новое расписание добавлено!");
		_ui->schedule_create_btn->setEnabled(true);
	}
	catch (const sql::SQLException& e) {
		QMessageBox::critical(this, "Ошибка", "Ошибка при выполнении запроса: " + QString::fromStdString(e.what()));
		_ui->schedule_create_btn->setEnabled(true);
	}
}


void Dispatcher::setupComboBoxes() {
	_ui->flight_num->clearEditText();
	_ui->schedule->clearEditText();
	_ui->city->clearEditText();
	_ui->from->clearEditText();
	_ui->to->clearEditText();
	_ui->day_of_week->clearEditText();
	_ui->when->clearEditText();
	_ui->model->clearEditText();
	_ui->model_2->clearEditText();
	_ui->code->clearEditText();
	_ui->departure_point->clearEditText();
	_ui->arrival_point->clearEditText();
	_ui->airport->clearEditText();
	_ui->plane_number->clearEditText();
	_ui->status->clearEditText();
	_ui->departure_time->clearEditText();
	_ui->arrival_time->clearEditText();
	_ui->flight_date->clearEditText();
	_ui->places->clear();
	_ui->payload->clear();
	_ui->production_year->clear();

	std::string sql_query = "SELECT DISTINCT `Номер рейса` FROM `рейсы`;";
	QStringList dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->flight_num, dataList);

	dataList.clear();
	sql_query = "SELECT \
			дни_недели.`День недели`, \
			расписание.`Время отправления`, \
			расписание.`Время прибытия`, \
			аэропорты_отправления.`Код аэропорта` AS `Код 1`, \
			аэропорты_отправления.`Название аэропорта` AS `Аэропорт отправления`, \
			аэропорты_отправления.`Город` AS `Город отправления`, \
			аэропорты_прибытия.`Код аэропорта` AS `Код 2`, \
			аэропорты_прибытия.`Название аэропорта` AS `Аэропорт прибытия`, \
			аэропорты_прибытия.`Город` AS `Город прибытия` \
			FROM \
			дни_недели \
			INNER JOIN \
			расписание ON дни_недели.`id дня` = расписание.`День недели` \
			INNER JOIN \
			аэропорты AS аэропорты_отправления ON расписание.`Точка отправления` = аэропорты_отправления.`id аэропорта` \
			INNER JOIN \
			аэропорты AS аэропорты_прибытия ON расписание.`Точка прибытия` = аэропорты_прибытия.`id аэропорта` \
			ORDER BY расписание.`id расписания`;";
	sql::ResultSet* result = runQuery(sql_query);
	if (result) {
		while (result->next()) {
			QString data = 
				QString::fromStdString(result->getString(1)) + " " +
				QString::fromStdString(result->getString(2)) + " " +
				QString::fromStdString(result->getString(3)) + " " +
				QString::fromStdString(result->getString(4)) + " " +
				QString::fromStdString(result->getString(5)) + " " +
				QString::fromStdString(result->getString(6)) + " " +
				QString::fromStdString(result->getString(7)) + " " +
				QString::fromStdString(result->getString(8)) + " " +
				QString::fromStdString(result->getString(9));
			dataList << data;
		}
		delete result;
	}
	setupComboBox(_ui->schedule, dataList);

	sql_query = "SELECT DISTINCT `Город` FROM `аэропорты`;";
	dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->city, dataList);
	setupComboBox(_ui->from, dataList);
	setupComboBox(_ui->to, dataList);

	sql_query = "SELECT DISTINCT `День недели` FROM `дни_недели`;";
	dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->day_of_week, dataList);
	setupComboBox(_ui->when, dataList);

	sql_query = "SELECT DISTINCT `Название модели` FROM `модели_самолётов`;";
	dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->model, dataList);
	setupComboBox(_ui->model_2, dataList);

	sql_query = "SELECT DISTINCT `Код аэропорта` FROM `аэропорты`;";
	dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->code, dataList);
	setupComboBox(_ui->departure_point, dataList);
	setupComboBox(_ui->arrival_point, dataList);

	sql_query = "SELECT DISTINCT `Название аэропорта` FROM `аэропорты`;";
	dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->airport, dataList);

	sql_query = "SELECT DISTINCT `Номер самолёта` FROM `самолёты`;";
	dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->plane_number, dataList);

	sql_query = "SELECT DISTINCT `Статус` FROM `статусы_рейсов`;";
	dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->status, dataList);

	dataList.clear();
	for (int hour = 0; hour < 24; ++hour) {
		for (int minute = 0; minute < 60; ++minute) {
			QString timeString = QString("%1:%2")
				.arg(hour, 2, 10, QChar('0'))
				.arg(minute, 2, 10, QChar('0'));
			dataList.append(timeString);
		}
	}
	setupComboBox(_ui->departure_time, dataList);
	setupComboBox(_ui->arrival_time, dataList);

	sql_query = "SELECT DISTINCT `Дата` FROM `рейсы`;";
	dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->flight_date, dataList);
}


void Dispatcher::date_input_btn_clicked() {
	_ui->date_input_btn->setEnabled(false);

	_calendar = std::make_unique<QCalendarWidget>(this);
	_calendar->setWindowFlags(Qt::Popup);

	_calendar->setGeometry(this->geometry().x(),
		this->geometry().y() + this->height(),
		_calendar->sizeHint().width(),
		_calendar->sizeHint().height());
	_calendar->show();

	std::string sql_query = "SELECT DISTINCT `Дата` FROM `рейсы`;";
	QStringList dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->flight_date, dataList);

	QTextCharFormat highlightFormat;
	highlightFormat.setBackground(Qt::green);

	for (const QString& dateStr : dataList) {
		QDate date = QDate::fromString(dateStr, "yyyy-MM-dd");
		_calendar->setDateTextFormat(date, highlightFormat);
	}

	connect(_calendar.get(), &QCalendarWidget::clicked, this, &Dispatcher::set_flight_date);
	_ui->date_input_btn->setEnabled(true);
}


void Dispatcher::set_flight_date(const QDate& date) {
	_ui->flight_date->setEditText(date.toString("yyyy-MM-dd"));
}
