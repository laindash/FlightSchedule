#include "airport_schedule.h"
#include <iostream>
#include <qmessagebox.h>


AirportSchedule::AirportSchedule(QWidget *parent) : QMainWindow(parent), _ui(new Ui::AirportScheduleClass()), 
	_dispatcher(nullptr), _admin(nullptr), _passenger(nullptr), _cashier(nullptr) {

	// Инициализация главного окна
	_ui->setupUi(this);

	// Установка флагов размера окна
	setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
	setWindowFlags(windowFlags() & ~(Qt::WindowFullscreenButtonHint | Qt::WindowMaximizeButtonHint));


	// Подключение сигналов к слотам для кнопок
	connect(_ui->dispatcher_btn, &QPushButton::clicked, this, &AirportSchedule::dispatcher_btn_clicked);
	connect(_ui->admin_btn, &QPushButton::clicked, this, &AirportSchedule::admin_btn_clicked);
	connect(_ui->passenger_btn, &QPushButton::clicked, this, &AirportSchedule::passenger_btn_clicked);
	connect(_ui->cashier_btn, &QPushButton::clicked, this, &AirportSchedule::cashier_btn_clicked);
}

AirportSchedule::~AirportSchedule() {
    delete _ui;
}


void AirportSchedule::dispatcher_btn_clicked() {
	if (!_dispatcher) {
		_dispatcher = std::make_unique<Dispatcher>(this);
	}
	_dispatcher->show();
}


void AirportSchedule::admin_btn_clicked() {
	if (!_admin) {
		_admin = std::make_unique<Admin>(this);
	}
	_admin->show();
}


void AirportSchedule::passenger_btn_clicked() {
	if (!_passenger) {
		_passenger = std::make_unique<Passenger>(this);
	}
	_passenger->show();
}


void AirportSchedule::cashier_btn_clicked() {
	if (!_cashier) {
		_cashier = std::make_unique<Cashier>(this);
	}
	_cashier->show();
}

