#include "dispatcher_queries.h"
#include "database.h"
#include "qmessagebox.h"
#include <qinputdialog.h>


DispatcherQueries::DispatcherQueries(QWidget* parent) : QMainWindow(parent), _ui(new Ui::DispatcherQueriesClass()), _query_result(nullptr) {
	// Инициализация главного окна
	_ui->setupUi(this);

	// Установка флагов размера окна
	setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
	setWindowFlags(windowFlags() & ~(Qt::WindowFullscreenButtonHint | Qt::WindowMaximizeButtonHint));
	setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);

	//connect(_ui->stone_availability_btn, &QPushButton::clicked, this, &DispatcherQueries::stone_availability_btn_clicked);
	//connect(_ui->metal_availability_btn, &QPushButton::clicked, this, &DispatcherQueries::metal_availability_btn_clicked);
	//connect(_ui->param_search_btn, &QPushButton::clicked, this, &DispatcherQueries::param_search_btn_clicked);
}

DispatcherQueries::~DispatcherQueries() {
	delete _ui;
}


void DispatcherQueries::stone_availability_btn_clicked() {
	_ui->stone_availability_btn->setEnabled(false);
	this->hide();
	std::string sql_query =
		"SELECT Камни.`Камень`, Камни.`Карат`, Огранка.`Вид огранки` "
		"FROM Камни INNER JOIN Огранка ON Камни.`Вид огранки` = Огранка.`Код` ";	
	std::string condition = "WHERE ";

	bool ok;
	QString stone = QInputDialog::getText(this, "Введите камень", "Название камня:", QLineEdit::Normal, "", &ok);

	if (ok && !stone.isEmpty()) {
		if (condition != "WHERE ") {
			condition += " AND ";
		}
		condition += "Камни.`Камень` LIKE '" + stone.toStdString() + "%'";
	}
	else if (!ok) {
		_ui->stone_availability_btn->setEnabled(true);
		this->show();
		return;
	}
	else if (stone.isEmpty()) {
		QMessageBox::critical(this, "Ошибка", "Вы ввели пустое значение!");
		_ui->stone_availability_btn->setEnabled(true);
		this->show();
		return;
	}

	sql_query += condition + ';';
	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &DispatcherQueries::enableStoneAvailabilityButton);
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
		_ui->stone_availability_btn->setEnabled(true);
		this->show();
	}
}

void DispatcherQueries::enableStoneAvailabilityButton() {
	_ui->stone_availability_btn->setEnabled(true);
	this->show();
}


void DispatcherQueries::metal_availability_btn_clicked() {
	_ui->metal_availability_btn->setEnabled(false);
	this->hide();
	std::string sql_query =
		"SELECT Металл.`Вид металла`, Металл.`Количество на складе, гр` "
		"FROM Металл ";
	std::string condition = "WHERE ";

	bool ok;
	QString metal = QInputDialog::getText(this, "Введите металл", "Вид металла:", QLineEdit::Normal, "", &ok);

	if (ok && !metal.isEmpty()) {
		if (condition != "WHERE ") {
			condition += " AND ";
		}
		condition += "Металл.`Вид металла` LIKE '" + metal.toStdString() + "%'";
	}
	else if (!ok) {
		_ui->metal_availability_btn->setEnabled(true);
		this->show();
		return;
	}
	else if (metal.isEmpty()) {
		QMessageBox::critical(this, "Ошибка", "Вы ввели пустое значение!");
		_ui->metal_availability_btn->setEnabled(true);
		this->show();
		return;
	}

	sql_query += condition + ';';
	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &DispatcherQueries::enableStoneAvailabilityButton);
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
		_ui->metal_availability_btn->setEnabled(true);
		this->show();
	}
}

void DispatcherQueries::enableMetalAvailabilityButton() {
	_ui->metal_availability_btn->setEnabled(true);
	this->show();
}


void DispatcherQueries::param_search_btn_clicked() {
	_ui->param_search_btn->setEnabled(false);
	this->hide();
	std::string sql_query =
		"SELECT изделие.`Название изделия`, стадии.`Стадия`, оборудование.`Оборудование`, технология.`Время, ч`, "
		"технология.`Мощность, ватт`, технология.`Температура, °C`, технология.`Цена производства, руб` "
		"FROM `технология` INNER JOIN `изделие` ON изделие.`Код изделия` = технология.`Изделие` "
		"INNER JOIN `стадии` ON стадии.`Номер` = технология.`Стадия` "
		"INNER JOIN `оборудование` ON оборудование.`Код` = технология.`Оборудование` ";
	std::string condition = "WHERE ";

	if (_ui->product_check->isChecked()) {
		bool ok;
		QString product = QInputDialog::getText(this, "Введите изделие", "Название изделия:", QLineEdit::Normal, "", &ok);

		if (ok && !product.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "Изделие.`Название изделия` LIKE '" + product.toStdString() + "%'";
		}
		else if (!ok) {
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}
		else if (product.isEmpty()) {
			QMessageBox::critical(this, "Ошибка", "Вы ввели пустое значение!");
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}
	}

	if (_ui->stage_check->isChecked()) {
		bool ok;
		QString stage = QInputDialog::getText(this, "Введите стадию", "Название стадии:", QLineEdit::Normal, "", &ok);

		if (ok && !stage.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "стадии.`Стадия` LIKE '" + stage.toStdString() + "%'";
		}
		else if (!ok) {
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}
		else if (stage.isEmpty()) {
			QMessageBox::critical(this, "Ошибка", "Вы ввели пустое значение!");
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}
	}

	if (_ui->equipment_check->isChecked()) {
		bool ok;
		QString equipment = QInputDialog::getText(this, "Введите оборудование", "Название оборудования:", QLineEdit::Normal, "", &ok);

		if (ok && !equipment.isEmpty()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "оборудование.`Оборудование` LIKE '" + equipment.toStdString() + "%'";
		}
		else if (!ok) {
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}
		else if (equipment.isEmpty()) {
			QMessageBox::critical(this, "Ошибка", "Вы ввели пустое значение!");
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}
	}

	if (_ui->time_check->isChecked()) {
		bool ok;
		QString lower_time = QInputDialog::getText(this, "Введите минимальное время", "Минимальное время завершения, ч:", QLineEdit::Normal, "", &ok);
		if (!ok) {
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}
		else if (lower_time.isEmpty()) {
			QMessageBox::critical(this, "Ошибка", "Вы ввели пустое значение!");
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}

		QString higher_time = QInputDialog::getText(this, "Введите максимальное время", "Максимальное время завершения, ч:", QLineEdit::Normal, "", &ok);
		if (!ok) {
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}
		else if (higher_time.isEmpty()) {
			QMessageBox::critical(this, "Ошибка", "Вы ввели пустое значение!");
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}

		if (lower_time.toDouble() <= higher_time.toDouble()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "технология.`Время, ч` BETWEEN " + lower_time.toStdString() + " AND " + higher_time.toStdString();
		}
		else {
			QMessageBox::critical(this, "Ошибка", "Введённое минимальное время завершения больше чем максимальное!");
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}
	}

	if (_ui->power_check->isChecked()) {
		bool ok;
		QString lower_power = QInputDialog::getText(this, "Введите минимальную мощность", "Минимальная мощность, ватт:", QLineEdit::Normal, "", &ok);
		if (!ok) {
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}
		else if (lower_power.isEmpty()) {
			QMessageBox::critical(this, "Ошибка", "Вы ввели пустое значение!");
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}

		QString higher_power = QInputDialog::getText(this, "Введите максимальную мощность", "Максимальная мощность, ватт:", QLineEdit::Normal, "", &ok);
		if (!ok) {
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}
		else if (higher_power.isEmpty()) {
			QMessageBox::critical(this, "Ошибка", "Вы ввели пустое значение!");
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}

		if (lower_power.toDouble() <= higher_power.toDouble()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "технология.`Мощность, ватт` BETWEEN " + lower_power.toStdString() + " AND " + higher_power.toStdString();
		}
		else {
			QMessageBox::critical(this, "Ошибка", "Введённая минимальная мощность больше чем максимальная!");
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}
	}

	if (_ui->temp_check->isChecked()) {
		bool ok;

		QString lower_temp = QInputDialog::getText(this, "Введите минимальную температуру", "Минимальная температура, °C:", QLineEdit::Normal, "", &ok);
		if (!ok) {
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}
		else if (lower_temp.isEmpty()) {
			QMessageBox::critical(this, "Ошибка", "Вы ввели пустое значение!");
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}

		QString higher_temp = QInputDialog::getText(this, "Введите максимальную температуру", "Максимальная температура, °C:", QLineEdit::Normal, "", &ok);
		if (!ok) {
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}
		else if (higher_temp.isEmpty()) {
			QMessageBox::critical(this, "Ошибка", "Вы ввели пустое значение!");
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}

		if (lower_temp.toDouble() <= higher_temp.toDouble()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "технология.`Температура, °C` BETWEEN " + lower_temp.toStdString() + " AND " + higher_temp.toStdString();
		}
		else {
			QMessageBox::critical(this, "Ошибка", "Введённая минимальная температура больше чем максимальная!");
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}
	}

	if (_ui->price_check->isChecked()) {
		bool ok;

		QString lower_price = QInputDialog::getText(this, "Введите минимальную цену", "Минимальная цена, руб:", QLineEdit::Normal, "", &ok);
		if (!ok) {
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}
		else if (lower_price.isEmpty()) {
			QMessageBox::critical(this, "Ошибка", "Вы ввели пустое значение!");
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}

		QString higher_price = QInputDialog::getText(this, "Введите максимальную цену", "Максимальная цена, руб:", QLineEdit::Normal, "", &ok);
		if (!ok) {
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}
		else if (higher_price.isEmpty()) {
			QMessageBox::critical(this, "Ошибка", "Вы ввели пустое значение!");
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}

		if (lower_price.toDouble() <= higher_price.toDouble()) {
			if (condition != "WHERE ") {
				condition += " AND ";
			}
			condition += "технология.`Цена производства, руб` BETWEEN " + lower_price.toStdString() + " AND " + higher_price.toStdString();
		}
		else {
			QMessageBox::critical(this, "Ошибка", "Введённая минимальная цена производства больше чем максимальная!");
			_ui->param_search_btn->setEnabled(true);
			this->show();
			return;
		}
	}

	if (!(_ui->product_check->isChecked() || _ui->stage_check->isChecked() || _ui->equipment_check->isChecked() || _ui->time_check->isChecked()
		|| _ui->power_check->isChecked() || _ui->temp_check->isChecked() || _ui->price_check->isChecked())) {
		
		QMessageBox::warning(this, "Предупреждение", "Вы не выбрали ни один из параметров для поиска!");
		_ui->param_search_btn->setEnabled(true);
		this->show();
		return;
	}

	sql_query += condition + ';';
	_query_result = new QueryResult(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_query_result, &QObject::destroyed, this, &DispatcherQueries::enableParamSearchButton);
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
		_ui->param_search_btn->setEnabled(true);
		this->show();
	}
}

void DispatcherQueries::enableParamSearchButton() {
	_ui->param_search_btn->setEnabled(true);
	this->show();
}