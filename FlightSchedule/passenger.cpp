#include "passenger.h"
#include "database.h"
#include "qmessagebox.h"
#include <QInputDialog>


Passenger::Passenger(QWidget* parent) : QMainWindow(parent), _ui(new Ui::PassengerClass()), _query_result(nullptr) {
	// Инициализация главного окна
	_ui->setupUi(this);

	// Установка флагов размера окна
	setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
	setWindowFlags(windowFlags() & ~(Qt::WindowFullscreenButtonHint | Qt::WindowMaximizeButtonHint));
	setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);

	connect(_ui->airports_btn, &QPushButton::clicked, this, &Passenger::airports_btn_clicked);
	connect(_ui->planes_btn, &QPushButton::clicked, this, &Passenger::planes_btn_clicked);
	connect(_ui->flights_btn, &QPushButton::clicked, this, &Passenger::flights_btn_clicked);
	connect(_ui->benefits_btn, &QPushButton::clicked, this, &Passenger::benefits_btn_clicked);
}

Passenger::~Passenger() {
	delete _ui;
}

void Passenger::airports_btn_clicked() {
	_ui->airports_btn->setEnabled(false);
	this->hide();

	_query_result = new QueryResult(this);
	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Passenger::enableAirportsButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT аэропорты.`Код аэропорта`, аэропорты.`Название аэропорта`, аэропорты.`Город` FROM `аэропорты`;");

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

void Passenger::enableAirportsButton() {
	_ui->airports_btn->setEnabled(true);
	this->show();
}


void Passenger::planes_btn_clicked() {
	_ui->planes_btn->setEnabled(false);
	this->hide();

	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Passenger::enablePlanesButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT самолёты.`Номер самолёта`, модели_самолётов.`Название модели`, модели_самолётов.`Количество посадочных мест`, модели_самолётов.`Грузоподъёмность, т`, самолёты.`Год выпуска` "
		"FROM `самолёты` INNER JOIN `модели_самолётов` ON самолёты.`Модель` = модели_самолётов.`id модели`;");
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

void Passenger::enablePlanesButton() {
	_ui->planes_btn->setEnabled(true);
	this->show();
}


void Passenger::flights_btn_clicked() {
	_ui->flights_btn->setEnabled(false);
	this->hide();

	std::string sql_query("SELECT \
		самолёты.`Номер самолёта` AS `Рейс`, \
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
		статусы_рейсов.`Статус` \
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

	/*if (_ui->root_check->isChecked()) {
		bool ok;
		QString day = QInputDialog::getText(this, "Введите маршрут", "Маршрут, по которому посмотреть расписание:", QLineEdit::Normal, "", &ok);

		if (ok && !day.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "дни_недели.`День недели` LIKE '" + day.toStdString() + "%'";
		}
		else if (!ok) {
			_ui->flights_btn->setEnabled(true);
			this->show();
			return;
		}
		else if (day.isEmpty()) {
			QMessageBox::critical(this, "Ошибка", "Вы ввели пустое значение!");
			_ui->flights_btn->setEnabled(true);
			this->show();
			return;
		}
	}
	*/

	if (_ui->day_check->isChecked()) {
		bool ok;
		QString day = QInputDialog::getText(this, "Введите день недели", "День недели, по которому посмотреть расписание:", QLineEdit::Normal, "", &ok);

		if (ok && !day.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "дни_недели.`День недели` LIKE '" + day.toStdString() + "%'";
		}
		else if (!ok) {
			_ui->flights_btn->setEnabled(true);
			this->show();
			return;
		}
		else if (day.isEmpty()) {
			QMessageBox::critical(this, "Ошибка", "Вы ввели пустое значение!");
			_ui->flights_btn->setEnabled(true);
			this->show();
			return;
		}
	}

	if (!(_ui->root_check->isChecked() || _ui->day_check->isChecked() || _ui->date_check->isChecked() || _ui->model_check->isChecked())) {
		QMessageBox::warning(this, "Предупреждение", "Вы не выбрали ни один из параметров для поиска!");
		_ui->flights_btn->setEnabled(true);
		this->show();
		return;
	}

	sql_query += condition + ';';
	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Passenger::enableFlightsButton);
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
		_ui->flights_btn->setEnabled(true);
		this->show();
	}
}

void Passenger::enableFlightsButton() {
	_ui->flights_btn->setEnabled(true);
	this->show();
}


void Passenger::benefits_btn_clicked() {
	_ui->benefits_btn->setEnabled(false);
	this->hide();
	QString table_name = "`список_льгот`", primary_key_column_name = "`id льготы`";

	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &Passenger::enableBenefitsButton);
	_query_result->setAttribute(Qt::WA_DeleteOnClose); //clear memory

	sql::ResultSet* result = runQuery("SELECT список_льгот.`Наименование льготы`, список_льгот.`Скидка, %` FROM `список_льгот`;");

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

void Passenger::enableBenefitsButton() {
	_ui->benefits_btn->setEnabled(true);
	this->show();
}
