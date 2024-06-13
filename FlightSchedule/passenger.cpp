﻿#include "passenger.h"
#include "database.h"
#include "qmessagebox.h"
#include <qfiledialog.h>
#include <QTextCharFormat>


Passenger::Passenger(QWidget* parent) : QMainWindow(parent), _ui(new Ui::PassengerClass()) {
	_ui->setupUi(this);

	// Установка флагов размера окна
	setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
	setWindowFlags(windowFlags() & ~(Qt::WindowFullscreenButtonHint | Qt::WindowMaximizeButtonHint));
	setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);
	setWindowModality(Qt::WindowModal);

	flights_add();
	setupComboBoxes();

	connect(_ui->upd_flights_table_btn, &QPushButton::clicked, this, &Passenger::flights_add);
	connect(_ui->benefits_btn, &QPushButton::clicked, this, &Passenger::benefits_add);
	connect(_ui->save_flights_table_btn, &QPushButton::clicked, this, &Passenger::flights_save);

	connect(_ui->upd_flights_inputs_btn, &QPushButton::clicked, this, &Passenger::setupComboBoxes);
	connect(_ui->date_input_btn, &QPushButton::clicked, this, &Passenger::date_input_btn_clicked);

	connect(_ui->flight_search_btn, &QPushButton::clicked, this, &Passenger::flight_search_btn_clicked);
}

Passenger::~Passenger() {
	delete _ui;
}


void Passenger::saveTableToFile(QTableWidget* tableWidget) {
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


void Passenger::flights_save() {
	saveTableToFile(_flights->_ui->output_table);
}


void Passenger::flights_add() {
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


void Passenger::benefits_add() {
	QLayoutItem* child;
	while ((child = _ui->flights_layout->takeAt(0)) != nullptr) {
		delete child->widget(); // Удаляем виджеты
		delete child; // Освобождаем память после элемента компоновки
	}

	_flights = new QueryResult(this);

	sql::ResultSet* result = runQuery("SELECT список_льгот.`Наименование льготы`, список_льгот.`Скидка, %` FROM `список_льгот`;");
	if (result) {
		_flights->addDataToTable(result);
		delete result;
	}

	_ui->flights_layout->addWidget(_flights);
}


void Passenger::flight_search_btn_clicked() {
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


void Passenger::setupComboBoxes() {
	_ui->flight_num->clearEditText();
	_ui->from->clearEditText();
	_ui->to->clearEditText();
	_ui->when->clearEditText();
	_ui->model->clearEditText();
	_ui->flight_date->clearEditText();

	std::string sql_query = "SELECT DISTINCT `Город` FROM `аэропорты`;";
	QStringList dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->from, dataList);
	setupComboBox(_ui->to, dataList);

	sql_query = "SELECT DISTINCT `День недели` FROM `дни_недели`;";
	dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->when, dataList);

	sql_query = "SELECT DISTINCT `Название модели` FROM `модели_самолётов`;";
	dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->model, dataList);

	sql_query = "SELECT DISTINCT `Номер рейса` FROM `рейсы`;";
	dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->flight_num, dataList);

	sql_query = "SELECT DISTINCT `Дата` FROM `рейсы`;";
	dataList = getDataFromTable(sql_query);
	setupComboBox(_ui->flight_date, dataList);
}


void Passenger::date_input_btn_clicked() {
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

	connect(_calendar.get(), &QCalendarWidget::clicked, this, &Passenger::set_flight_date);
	_ui->date_input_btn->setEnabled(true);
}


void Passenger::set_flight_date(const QDate& date) {
	_ui->flight_date->setEditText(date.toString("yyyy-MM-dd"));
}