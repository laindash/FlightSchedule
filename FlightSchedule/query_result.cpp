#include "query_result.h"
#include <iostream>
#include "database.h"
#include <qmessagebox.h>
#include <qtablewidget.h>
#include <qlabel.h>
#include <QByteArray>
#include <QToolBar>
#include <QContextMenuEvent>
#include <QInputDialog>


QueryResult::QueryResult(QWidget* parent) : QMainWindow(parent), _ui(new Ui::QueryResultClass()) {
	// Инициализация главного окна
	_ui->setupUi(this);

	// Установка флагов размера окна
	setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
	setWindowFlags(windowFlags() & ~(Qt::WindowFullscreenButtonHint | Qt::WindowMaximizeButtonHint));
	setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);
	
	// Установка режима изменения размеров секций горизонтального заголовка
	_ui->output_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	_ui->output_table->verticalHeader()->setVisible(false);
}

QueryResult::QueryResult(QWidget* parent, QString& table_name, QString& primary_key_column_name) : QMainWindow(parent), _ui(new Ui::QueryResultClass()) {
	// Инициализация главного окна
	_ui->setupUi(this);

	// Установка флагов размера окна
	setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
	setWindowFlags(windowFlags() & ~(Qt::WindowFullscreenButtonHint | Qt::WindowMaximizeButtonHint));
	setWindowFlags(windowFlags() & ~Qt::WindowMinMaxButtonsHint);

	_ui->output_table->verticalHeader()->setVisible(false);
	_ui->output_table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
	_ui->output_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	_ui->output_table->setSelectionMode(QAbstractItemView::SingleSelection);

	// Создание действия (акции) "Save"
	QAction* saveAction = new QAction(tr("&Сохранить данные"), this);
	QAction* addRowAction = new QAction(tr("&Добавить строку"), this);
	QAction* deleteRowAction = new QAction(tr("&Удалить строку"), this);

	// Добавление действия в верхнюю панель
	QToolBar* toolBar = addToolBar(tr("Сохранить данные"));
	toolBar->addAction(saveAction);
	toolBar->addAction(addRowAction);
	toolBar->addAction(deleteRowAction);

	// Подключение сигнала triggered() к пользовательской функции
	connect(saveAction, &QAction::triggered, this, &QueryResult::saveDataInDB);
	connect(addRowAction, &QAction::triggered, this, &QueryResult::addNewRow);
	connect(deleteRowAction, &QAction::triggered, this, &QueryResult::deleteRow);
	toolBar->setMovable(false);

	_table_name = table_name;
	_primary_key_column_name = primary_key_column_name;
}

QueryResult::~QueryResult() {
	delete _ui;
}


void QueryResult::addNewRow() {
	int rowCount = _ui->output_table->rowCount();
	QString insertQuery = "INSERT INTO " + _table_name + " (" + _primary_key_column_name + ") VALUES ('" + QString::number(rowCount + 1) + "');";
	runQuery(insertQuery.toStdString());

	_ui->output_table->insertRow(rowCount);

	// Установка значения в первом столбце равным номеру строки
	QTableWidgetItem* item = new QTableWidgetItem(QString::number(rowCount + 1));
	item->setTextAlignment(Qt::AlignCenter);
	item->setFlags(item->flags() & ~Qt::ItemIsEditable);
	_ui->output_table->setItem(rowCount, 0, item);
	
	editSize();
}


void QueryResult::deleteRow() {
	QMessageBox::StandardButton reply = QMessageBox::question(this, "Подтверждение", "Вы уверены, что хотите удалить эту строку?",
		QMessageBox::Yes | QMessageBox::No);
	if (reply == QMessageBox::Yes) {
		int currentRow = _ui->output_table->currentRow();
		if (currentRow >= 0) { // Проверка, что выбрана какая-то строка
			QTableWidgetItem* primaryKeyItem = _ui->output_table->item(currentRow, 0); // Предполагается, что первый столбец содержит первичные ключи
			if (primaryKeyItem) {
				bool ok;
				QString primaryKeyValue = primaryKeyItem->text(); // Получение значения первичного ключа
				QString deleteQuery = "DELETE FROM " + _table_name + " WHERE " + _primary_key_column_name + " = '" + primaryKeyValue + "';";

				runQuery(deleteQuery.toStdString());
				_ui->output_table->removeRow(currentRow);

				int rowCount = _ui->output_table->rowCount();
				for (int i = 0; i < rowCount; ++i) {
					QTableWidgetItem* idItem = _ui->output_table->item(i, 0); // First column contains IDs
					if (idItem) {
						QString newId = QString::number(i + 1); // New sequential ID
						QString oldId = idItem->text();

						// Update the ID in the database
						QString updateQuery = "UPDATE " + _table_name + " SET " + _primary_key_column_name + " = '" + newId + "' WHERE " + _primary_key_column_name + " = '" + oldId + "';";
						runQuery(updateQuery.toStdString());

						// Update the ID in the table widget
						idItem->setText(newId);
					}
				}
				editSize();
			}
		}
	}
}


void QueryResult::addDataToTable(sql::ResultSet* resultSet) {
	int columnCount = resultSet->getMetaData()->getColumnCount();
	_ui->output_table->setColumnCount(columnCount);

	// Добавление заголовков столбцов и установка минимальной ширины
	for (int column = 0; column < columnCount; ++column) {
		QString columnName = QString::fromStdString(resultSet->getMetaData()->getColumnLabel(column + 1));
		QTableWidgetItem* headerItem = new QTableWidgetItem(columnName);
		headerItem->setFont(QFont("Segoe UI", 12, QFont::Bold));
		_ui->output_table->setHorizontalHeaderItem(column, headerItem);
	}

	// Добавление данных в каждый столбец и пересчет размеров
	_ui->output_table->setRowCount(0); // Очистить таблицу перед заполнением
	int row = 0;
	int imageHeight = 0;
	while (resultSet->next()) {
		_ui->output_table->insertRow(row);
		for (int column = 0; column < columnCount; ++column) {
			if (resultSet->getMetaData()->getColumnTypeName(column + 1) == "BLOB") {
				std::istream* blobStream = resultSet->getBlob(column + 1);
				if (blobStream) {
					QByteArray blobData;
					blobData = QByteArray::fromStdString(std::string((std::istreambuf_iterator<char>(*blobStream)), std::istreambuf_iterator<char>()));
					delete blobStream;

					QPixmap pixmap;
					pixmap.loadFromData(blobData);
					QLabel* label = new QLabel();
					label->setPixmap(pixmap.scaled(150, 150)); // Установка размеров изображения
					imageHeight = 150;
					_ui->output_table->setCellWidget(row, column, label);
					_ui->output_table->setRowHeight(row, imageHeight);
				}
			}
			else {
				QString data = QString::fromStdString(resultSet->getString(column + 1));
				QTableWidgetItem* item = new QTableWidgetItem(data);
				item->setTextAlignment(Qt::AlignCenter);
				if (column == 0) {
					item->setFlags(item->flags() & ~Qt::ItemIsEditable);
				}
				_ui->output_table->setItem(row, column, item);
			}
		}
		++row;
	}
	editSize();
}


void QueryResult::saveDataInDB() {
	// Получаем количество строк и столбцов в таблице
	int rowCount = _ui->output_table->rowCount();
	int columnCount = _ui->output_table->columnCount();

	// Проходим по каждой строке таблицы
	for (int row = 0; row < rowCount; ++row) {
		// Формируем SQL-запрос на добавление новой записи в таблицу базы данных
		QString insertQuery;
		QString columnName;
		// Формируем список названий столбцов
		for (int column = 0; column < columnCount; ++column) {
			columnName = _ui->output_table->horizontalHeaderItem(column)->text();

			insertQuery = "UPDATE " + _table_name + " SET `" + columnName + "` = ";

			if (columnName != "Фото изделия") {
				QTableWidgetItem* item = _ui->output_table->item(row, column);		
				if (item) {
					QString value = item->text();
					item->setTextAlignment(Qt::AlignCenter);
					insertQuery.append("'" + value + "' ");
				}
				else {
					// Если ячейка пустая, добавляем NULL
					insertQuery.append("NULL ");
				}
				insertQuery += "WHERE (" + _primary_key_column_name + " = " + "'" + QString::number(row + 1) + "'" + ");";
				// Выполняем SQL-запрос на добавление записи в базу данных
				runQuery(insertQuery.toStdString());
			}
		}
	}
	editSize();
}


void QueryResult::contextMenuEvent(QContextMenuEvent* event) {
	event->ignore(); // Предотвращаем обработку контекстного меню
}


void QueryResult::editSize() {
	_ui->output_table->resizeColumnsToContents();

	const int maxWidth = 1400;
	const int maxHeight = 720;
	const int additionalWidth = 5; // Примерное значение для увеличения ширины
	const int additionalHeight = 40; // Примерное значение для увеличения ширины
	// Подгоняем размер таблицы под содержимое
	int tableWidth = _ui->output_table->horizontalHeader()->length() + additionalWidth;
	int tableHeight = _ui->output_table->verticalHeader()->length() + additionalHeight;

	if (tableWidth > maxWidth) {
		tableWidth = maxWidth;
		tableHeight += 20;
	}
	if (tableHeight > maxHeight) {
		tableHeight = maxHeight;
		tableWidth += 20;
	}
	_ui->output_table->resize(tableWidth, tableHeight);

	if (!_table_name.isEmpty()) {
		tableHeight += 15;
	}
	// Подгоняем размер окна под размер таблицы
	resize(tableWidth, tableHeight);
}