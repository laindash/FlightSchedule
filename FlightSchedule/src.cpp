#include <iostream>
#include <QtWidgets/QApplication>
#include "airport_schedule.h"


int main(int argc, char* argv[]) {
	setlocale(LC_ALL, ".UTF8");

	// Создание объекта приложения
	QApplication a(argc, argv);
	// Создание объекта главной графической формы
	AirportSchedule w;

	// Вывод графической формы
	w.show();

	return a.exec();
}
