#include "airport_schedule.h"
#include <iostream>
#include <qmessagebox.h>


AirportSchedule::AirportSchedule(QWidget *parent) : QMainWindow(parent), _ui(new Ui::AirportScheduleClass()), 
	_dispatcher(nullptr), _admin(nullptr), _passenger(nullptr) {

	// Инициализация главного окна
	_ui->setupUi(this);

	// Установка флагов размера окна
	setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
	setWindowFlags(windowFlags() & ~(Qt::WindowFullscreenButtonHint | Qt::WindowMaximizeButtonHint));


	// Подключение сигналов к слотам для кнопок
	connect(_ui->dispatcher_btn, &QPushButton::clicked, this, &AirportSchedule::dispatcher_btn_clicked);
	connect(_ui->admin_btn, &QPushButton::clicked, this, &AirportSchedule::admin_btn_clicked);
	connect(_ui->passenger_btn, &QPushButton::clicked, this, &AirportSchedule::passenger_btn_clicked);
}

AirportSchedule::~AirportSchedule() {
    delete _ui;
}


void AirportSchedule::dispatcher_btn_clicked() {
	_ui->dispatcher_btn->setEnabled(false);
	this->hide();
	_dispatcher = new Dispatcher(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_dispatcher, &QObject::destroyed, this, &AirportSchedule::enableDispatcherButton);
	_dispatcher->setAttribute(Qt::WA_DeleteOnClose); //clear memory
	_dispatcher->show();
}

void AirportSchedule::enableDispatcherButton() {
	_ui->dispatcher_btn->setEnabled(true);
	this->show();
}


void AirportSchedule::admin_btn_clicked() {
	_ui->admin_btn->setEnabled(false);
	this->hide();
	_admin = new Admin(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_admin, &QObject::destroyed, this, &AirportSchedule::enableAdminButton);
	_admin->setAttribute(Qt::WA_DeleteOnClose); //clear memory
	_admin->show();
}

void AirportSchedule::enableAdminButton() {
	_ui->admin_btn->setEnabled(true);
	this->show();
}


void AirportSchedule::passenger_btn_clicked() {
	_ui->passenger_btn->setEnabled(false);
	this->hide();
	_passenger = new Passenger(this);

	// Подключение сигнала destroyed() к слоту, который включает кнопку
	connect(_passenger, &QObject::destroyed, this, &AirportSchedule::enablePassengerButton);
	_passenger->setAttribute(Qt::WA_DeleteOnClose); //clear memory
	_passenger->show();
}

void AirportSchedule::enablePassengerButton() {
	_ui->passenger_btn->setEnabled(true);
	this->show();
}
