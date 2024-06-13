#include "cashier.h"
#include "database.h"
#include "qmessagebox.h"
#include <qfiledialog.h>
#include <QTextCharFormat>


Cashier::Cashier(QWidget* parent) : QMainWindow(parent), _ui(new Ui::CashierClass()) {
	// Инициализация главного окна
	_ui->setupUi(this);

	// Установка флагов размера окна
	setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
	setWindowFlags(windowFlags() & ~(Qt::WindowFullscreenButtonHint | Qt::WindowMaximizeButtonHint));
	setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);
	setWindowModality(Qt::WindowModal);

	sales_add();
	flights_add();
	benefits_add();

	setupComboBoxes();

	connect(_ui->upd_sales_table_btn, &QPushButton::clicked, this, &Cashier::sales_add);
	connect(_ui->upd_flights_table_btn, &QPushButton::clicked, this, &Cashier::flights_add);
	connect(_ui->upd_benefits_table_btn, &QPushButton::clicked, this, &Cashier::benefits_add);

	connect(_ui->upd_sales_inputs_btn, &QPushButton::clicked, this, &Cashier::setupComboBoxes);
	connect(_ui->upd_flights_inputs_btn, &QPushButton::clicked, this, &Cashier::setupComboBoxes);
	connect(_ui->upd_benefits_inputs_btn, &QPushButton::clicked, this, &Cashier::setupComboBoxes);
	connect(_ui->sell_date_input_btn, &QPushButton::clicked, this, &Cashier::sell_date_input_btn_clicked);
	connect(_ui->date_input_btn, &QPushButton::clicked, this, &Cashier::date_input_btn_clicked);

	connect(_ui->save_sales_table_btn, &QPushButton::clicked, this, &Cashier::sales_save);
	connect(_ui->save_flights_table_btn, &QPushButton::clicked, this, &Cashier::flights_save);
	connect(_ui->save_benefits_table_btn, &QPushButton::clicked, this, &Cashier::benefits_save);

	connect(_ui->passenger_search_btn, &QPushButton::clicked, this, &Cashier::passenger_search_btn_clicked);
	connect(_ui->passenger_create_btn, &QPushButton::clicked, this, &Cashier::passenger_create_btn_clicked);
	connect(_ui->flight_search_btn, &QPushButton::clicked, this, &Cashier::flight_search_btn_clicked);
}

Cashier::~Cashier() {
	delete _ui;
}


void Cashier::saveTableToFile(QTableWidget* tableWidget) {
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


void Cashier::sales_save() {
	_ui->save_sales_table_btn->setEnabled(false);
	saveTableToFile(_sales->_ui->output_table);
	_ui->save_sales_table_btn->setEnabled(true);
}

void Cashier::flights_save() {
	_ui->save_flights_table_btn->setEnabled(false);
	saveTableToFile(_flights->_ui->output_table);
	_ui->save_flights_table_btn->setEnabled(true);
}

void Cashier::benefits_save() {
	_ui->save_benefits_table_btn->setEnabled(false);
	saveTableToFile(_benefits->_ui->output_table);
	_ui->save_benefits_table_btn->setEnabled(true);
}


void Cashier::sales_add() {
	_ui->upd_sales_table_btn->setEnabled(false);

	QLayoutItem* child;
	while ((child = _ui->sales_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}
	QString table_name = "`ведомость_продаж_билетов`", primary_key_column_name = "`id продажи`";

	_sales = new QueryResult(this, table_name, primary_key_column_name, true);

	sql::ResultSet* result = runQuery("SELECT \
		ведомость_продаж_билетов.`Дата и время продажи`, \
		ведомость_продаж_билетов.`ФИО пассажира`, \
		ведомость_продаж_билетов.`Паспортные данные`, \
		рейсы.`Номер рейса` AS `Рейс`, \
		список_льгот.`Наименование льготы` AS `Наличие льготы`, \
		ведомость_продаж_билетов.`Багаж, кг`, \
		ведомость_продаж_билетов.`Стоимость, руб` \
		FROM \
		ведомость_продаж_билетов \
		INNER JOIN \
		рейсы ON ведомость_продаж_билетов.`Номер рейса` = рейсы.`id рейса` \
		INNER JOIN \
		список_льгот ON ведомость_продаж_билетов.`Наличие льгот` = список_льгот.`id льготы` \
		ORDER BY ведомость_продаж_билетов.`id продажи`;");

	if (result) {
		_sales->addDataToTable(result);
		delete result;
	}

	_ui->sales_layout->addWidget(_sales);
	_ui->upd_sales_table_btn->setEnabled(true);
}


void Cashier::flights_add() {
	_ui->upd_flights_table_btn->setEnabled(false);

	QLayoutItem* child;
	while ((child = _ui->flights_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}

	_flights = new QueryResult(this);

	sql::ResultSet* result = runQuery("SELECT \
		рейсы.`Номер рейса` AS `Рейс`, \
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
		модели_самолётов.`Название модели` AS `Самолёт`, \
		статусы_рейсов.`Статус`, \
		(модели_самолётов.`Количество посадочных мест` - IFNULL((SELECT COUNT(*) FROM ведомость_продаж_билетов WHERE ведомость_продаж_билетов.`номер рейса` = рейсы.`id рейса`), 0)) AS `Доступно билетов` \
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


void Cashier::benefits_add() {
	_ui->upd_benefits_table_btn->setEnabled(false);

	QLayoutItem* child;
	while ((child = _ui->benefits_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}

	_benefits = new QueryResult(this);

	sql::ResultSet* result = runQuery("SELECT список_льгот.`Наименование льготы`, список_льгот.`Скидка, %` FROM список_льгот;");
	if (result) {
		_benefits->addDataToTable(result);
		delete result;
	}

	_ui->benefits_layout->addWidget(_benefits);
	_ui->upd_benefits_table_btn->setEnabled(true);
}

void Cashier::passenger_search_btn_clicked() {
	_ui->passenger_search_btn->setEnabled(false);

	std::string sql_query("SELECT \
		ведомость_продаж_билетов.`Дата и время продажи`, \
		ведомость_продаж_билетов.`ФИО пассажира`, \
		ведомость_продаж_билетов.`Паспортные данные`, \
		рейсы.`Номер рейса` AS `Рейс`, \
		список_льгот.`Наименование льготы` AS `Наличие льготы`, \
		ведомость_продаж_билетов.`Багаж, кг`, \
		ведомость_продаж_билетов.`Стоимость, руб` \
		FROM \
		ведомость_продаж_билетов \
		INNER JOIN \
		рейсы ON ведомость_продаж_билетов.`Номер рейса` = рейсы.`id рейса` \
		INNER JOIN \
		список_льгот ON ведомость_продаж_билетов.`Наличие льгот` = список_льгот.`id льготы` ");
	std::string condition = "WHERE ";

	if (!_ui->sell_date->currentText().isEmpty()) {
		QString sell_date = _ui->sell_date->currentText();

		if (!sell_date.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "ведомость_продаж_билетов.`Дата и время продажи` LIKE '" + sell_date.toStdString() + "%'";
		}
	}

	if (!_ui->fio->currentText().isEmpty()) {
		QString fio = _ui->fio->currentText();

		if (!fio.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "ведомость_продаж_билетов.`ФИО пассажира` LIKE '" + fio.toStdString() + "%'";
		}
	}

	if (!_ui->serial->currentText().isEmpty()) {
		QString serial = _ui->serial->currentText();

		if (!serial.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "ведомость_продаж_билетов.`Паспортные данные` LIKE '" + serial.toStdString() + "%'";
		}
	}

	if (!_ui->flight_number->currentText().isEmpty()) {
		QString flight_number = _ui->flight_number->currentText();

		if (!flight_number.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "рейсы.`Номер рейса` LIKE '" + flight_number.toStdString() + "%'";
		}
	}

	if (!_ui->benefit->currentText().isEmpty()) {
		QString benefit = _ui->benefit->currentText();

		if (!benefit.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "список_льгот.`Наименование льготы` LIKE '" + benefit.toStdString() + "%'";
		}
	}

	if (!(_ui->sell_date->currentText().isEmpty() && _ui->fio->currentText().isEmpty() && _ui->serial->currentText().isEmpty() && _ui->flight_number->currentText().isEmpty() &&
		_ui->benefit->currentText().isEmpty())) {
		sql_query += condition + ';';
	}
	else {
		sql_query += "ORDER BY ведомость_продаж_билетов.`id продажи`;";
	}

	QLayoutItem* child;
	while ((child = _ui->sales_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}

	_sales = new QueryResult(this);

	sql::ResultSet* result = runQuery(sql_query);

	if (result) {
		_sales->addDataToTable(result);
		delete result;
	}

	_ui->sales_layout->addWidget(_sales);
	_ui->passenger_search_btn->setEnabled(true);
}


void Cashier::passenger_create_btn_clicked() {
	_ui->passenger_create_btn->setEnabled(false);

	QString fio = _ui->fio->currentText();
	QString serial = _ui->serial->currentText();
	QString flight_number = _ui->flight_number->currentText();
	QString benefit = _ui->benefit->currentText();
	QString baggage = _ui->baggage->text();
	QString price = _ui->price->text();

	if (fio.isEmpty() || serial.isEmpty() || flight_number.isEmpty() ||
		benefit.isEmpty() || baggage.isEmpty() || price.isEmpty()) {
		QMessageBox::warning(this, "Ошибка", "Пожалуйста, заполните все поля перед вставкой данных!");
		_ui->passenger_create_btn->setEnabled(true);
		return;
	}

	try {
		QLayoutItem* child;

		while ((child = _ui->sales_layout->takeAt(0)) != nullptr) {
			delete child->widget(); // Удаляем виджеты
			delete child; // Освобождаем память после элемента компоновки
		}

		_sales = new QueryResult(this);

		std::unique_ptr<sql::ResultSet> result(runQuery(
			"SELECT \
			ведомость_продаж_билетов.`Дата и время продажи`, \
			ведомость_продаж_билетов.`ФИО пассажира`, \
			ведомость_продаж_билетов.`Паспортные данные`, \
			рейсы.`Номер рейса` AS `Рейс`, \
			список_льгот.`Наименование льготы` AS `Наличие льготы`, \
			ведомость_продаж_билетов.`Багаж, кг`, \
			ведомость_продаж_билетов.`Стоимость, руб` \
			FROM \
			ведомость_продаж_билетов \
			INNER JOIN \
			рейсы ON ведомость_продаж_билетов.`Номер рейса` = рейсы.`id рейса` \
			INNER JOIN \
			список_льгот ON ведомость_продаж_билетов.`Наличие льгот` = список_льгот.`id льготы` \
			ORDER BY ведомость_продаж_билетов.`id продажи`;"
		));

		if (result) {
			_sales->addDataToTable(result.get());
		}

		_ui->sales_layout->addWidget(_sales);

		// Получаем ID существующего рейса
		std::string flight_query = "SELECT `id рейса` FROM `рейсы` WHERE `Номер рейса` = '" + flight_number.toStdString() + "';";
		std::unique_ptr<sql::ResultSet> flight_result(runQuery(flight_query));
		int flight_id = 0;
		if (flight_result->next()) {
			flight_id = flight_result->getInt(1);
		}
		else {
			QMessageBox::critical(this, "Ошибка", "Рейс не найден!");
			_ui->passenger_create_btn->setEnabled(true);
			return;
		}

		// Получаем ID существующей льготы
		std::string benefit_query = "SELECT `id льготы` FROM `список_льгот` WHERE `Наименование льготы` = '" + benefit.toStdString() + "';";
		std::unique_ptr<sql::ResultSet> benefit_result(runQuery(benefit_query));
		int benefit_id = 0;
		if (benefit_result->next()) {
			benefit_id = benefit_result->getInt(1);
		}
		else {
			QMessageBox::critical(this, "Ошибка", "Льгота не найдена!");
			_ui->passenger_create_btn->setEnabled(true);
			return;
		}

		// Получаем количество строк в таблице `ведомость_продаж_билетов`
		int row_count = _sales->_ui->output_table->rowCount();

		// Определяем следующий ID
		int next_id = row_count + 1;

		// Определяем текущее время
		std::string date_and_time = getCurrentDateAndTime();

		// Вставляем данные в таблицу `ведомость продаж билетов`
		std::string sales_query =
			"INSERT INTO `ведомость_продаж_билетов` (`id продажи`, `Дата и время продажи`, `ФИО пассажира`, `Паспортные данные`, \
			`Номер рейса`, `Наличие льгот`, `Багаж, кг`, `Стоимость, руб`) "
			"VALUES (" + std::to_string(next_id) + ", '" + date_and_time + "', '" + fio.toStdString() + "', '"
			+ serial.toStdString() + "', " + std::to_string(flight_id) + ", " + std::to_string(benefit_id) + ", " + baggage.toStdString() + ", " + price.toStdString() + ");";

		runQuery(sales_query);

		sales_add();

		QMessageBox::information(this, "Успех", "Продажа зарегистрирована!");
		_ui->passenger_create_btn->setEnabled(true);
	}
	catch (const sql::SQLException& e) {
		QMessageBox::critical(this, "Ошибка", "Ошибка при выполнении запроса: " + QString::fromStdString(e.what()));
		_ui->passenger_create_btn->setEnabled(true);
	}
}


void Cashier::flight_search_btn_clicked() {
	_ui->flight_search_btn->setEnabled(false);

	std::string sql_query("SELECT \
		рейсы.`Номер рейса` AS `Рейс`, \
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
		модели_самолётов.`Название модели` AS `Самолёт`, \
		статусы_рейсов.`Статус`, \
		(модели_самолётов.`Количество посадочных мест` - IFNULL((SELECT COUNT(*) FROM ведомость_продаж_билетов WHERE ведомость_продаж_билетов.`номер рейса` = рейсы.`id рейса`), 0)) AS `Доступно билетов` \
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

	if (!_ui->model->currentText().isEmpty()) {
		QString model = _ui->model->currentText();

		if (!model.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "модели_самолётов.`Название модели` LIKE '" + model.toStdString() + "%'";
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

	if (!_ui->flight_date->currentText().isEmpty()) {
		QString flight_date = _ui->flight_date->currentText();

		if (!flight_date.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "рейсы.`Дата` LIKE '" + flight_date.toStdString() + "%'";
		}
	}

	if (!(_ui->flight_date->currentText().isEmpty() && _ui->from->currentText().isEmpty() && _ui->to->currentText().isEmpty() && _ui->when->currentText().isEmpty() &&
		_ui->model->currentText().isEmpty() && _ui->flight_num->currentText().isEmpty())) {
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


void Cashier::setupComboBoxes() {
	_ui->fio->clearEditText();
	_ui->serial->clearEditText();
	_ui->flight_number->clearEditText();
	_ui->flight_num->clearEditText();
	_ui->benefit->clearEditText();
	_ui->from->clearEditText();
	_ui->to->clearEditText();
	_ui->when->clearEditText();
	_ui->model->clearEditText();
	_ui->baggage->clear();
	_ui->price->clear();
	_ui->sell_date->clearEditText();
	_ui->flight_date->clearEditText();

	std::string sql_query = "SELECT DISTINCT `ФИО пассажира` FROM `ведомость_продаж_билетов`;";
	QStringList dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->fio, dataList);

	sql_query = "SELECT DISTINCT `Паспортные данные` FROM `ведомость_продаж_билетов`;";
	dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->serial, dataList);

	sql_query = "SELECT DISTINCT `Номер рейса` FROM `рейсы`;";
	dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->flight_number, dataList);
	setupComboBox(_ui->flight_num, dataList);

	sql_query = "SELECT DISTINCT `Наименование льготы` FROM `список_льгот`;";
	dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->benefit, dataList);

	sql_query = "SELECT DISTINCT `Город` FROM `аэропорты`;";
	dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->from, dataList);
	setupComboBox(_ui->to, dataList);

	sql_query = "SELECT DISTINCT `День недели` FROM `дни_недели`;";
	dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->when, dataList);

	sql_query = "SELECT DISTINCT `Название модели` FROM `модели_самолётов`;";
	dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->model, dataList);

	sql_query = "SELECT DISTINCT `Дата и время продажи` FROM `ведомость_продаж_билетов`;";
	dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->sell_date, dataList);

	sql_query = "SELECT DISTINCT `Дата` FROM `рейсы`;";
	dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->flight_date, dataList);
}


void Cashier::sell_date_input_btn_clicked() {
	_ui->sell_date_input_btn->setEnabled(false);

	_calendar = std::make_unique<QCalendarWidget>(this);
	_calendar->setWindowFlags(Qt::Popup);

	_calendar->setGeometry(this->geometry().x(),
		this->geometry().y() + this->height(),
		_calendar->sizeHint().width(),
		_calendar->sizeHint().height());
	_calendar->show();

	std::string sql_query = "SELECT DISTINCT `Дата и время продажи` FROM `ведомость_продаж_билетов`;";
	QStringList dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->sell_date, dataList);

	QTextCharFormat highlightFormat;
	highlightFormat.setBackground(Qt::green);

	for (const QString& dateStr : dataList) {
		QStringList temp = dateStr.split(' ');
		QDate date = QDate::fromString(temp[0], "yyyy-MM-dd");
		_calendar->setDateTextFormat(date, highlightFormat);
	}

	connect(_calendar.get(), &QCalendarWidget::clicked, this, &Cashier::set_sell_date);
	_ui->sell_date_input_btn->setEnabled(true);
}


void Cashier::set_sell_date(const QDate& date) {
	_ui->sell_date->setEditText(date.toString("yyyy-MM-dd"));
}


void Cashier::date_input_btn_clicked() {
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

	connect(_calendar.get(), &QCalendarWidget::clicked, this, &Cashier::set_flight_date);
	_ui->date_input_btn->setEnabled(true);
}


void Cashier::set_flight_date(const QDate& date) {
	_ui->flight_date->setEditText(date.toString("yyyy-MM-dd"));
}