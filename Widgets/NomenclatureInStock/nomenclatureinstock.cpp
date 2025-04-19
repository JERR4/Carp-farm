#include "nomenclatureinstock.h"
#include "ui_nomenclatureinstock.h"
#include <iostream>
#include <QDebug>

NomenclatureInStock::NomenclatureInStock(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NomenclatureInStock)
{
    ui->setupUi(this);
    connect(ui->logout, &QPushButton::clicked, this, &NomenclatureInStock::logout);
    showNomenclatureInStock();
}

NomenclatureInStock::~NomenclatureInStock()
{
    delete ui;
}

void NomenclatureInStock::showNomenclatureInStock()
{
    // Очистка таблицы перед выводом новых данных
    ui->twStock->clearContents();
    // Установка ширины столбцов
    ui->twStock->setColumnCount(5);
    // Возможность прокрутки
    ui->twStock->setAutoScroll(true);
    // Режим выделения ячеек - только одна строка
    ui->twStock->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twStock->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Заголовки таблицы
    ui->twStock->setHorizontalHeaderItem(0,new QTableWidgetItem("Название"));
    ui->twStock->setHorizontalHeaderItem(1,new QTableWidgetItem("Тип"));
    ui->twStock->setHorizontalHeaderItem(2,new QTableWidgetItem("Куплено"));
    ui->twStock->setHorizontalHeaderItem(3,new QTableWidgetItem("Потрачено"));
    ui->twStock->setHorizontalHeaderItem(4,new QTableWidgetItem("Остаток"));
    // Последний столбец растягивается при изменении размера формы
    ui->twStock->horizontalHeader()->setStretchLastSection(true);
    // Разрешаем сортировку пользователю
    ui->twStock->setSortingEnabled(true);
    //ui->twStock->sortByColumn(0);
    // Запрет на изменение ячеек таблицы при отображении
    ui->twStock->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Очистить содержимое компонента
    ui->twStock->clearContents();
    // Если соединение не открыто, то вызвать нашу функцию для открытия
    // если подключиться не удалось, то вывести сообщение об ошибке и
    // выйти из функции
    // Создать объект запроса с привязкой к установленному соединению
    QSqlQuery query(db);
    // Создать строку запроса на выборку данных
    QString queryString = QString("SELECT * FROM calculate_stock();");
    // Выполнить запрос и поверить его успешность
    if (!query.exec(queryString)) {
        qDebug() << "Query execution error:" << query.lastError().text();
        return;
    }
    // Если запрос активен (успешно завершен),
    // то вывести сообщение о прочитанном количестве строк в окно вывода
    // и установить количество строк для компонента таблицы
    if( query.isActive())
        ui->twStock->setRowCount(query.size());
    else
        ui->twStock->setRowCount(0);
    // Прочитать в цикле все строки результата (курсора)
    // и вывести их в компонент таблицы
    int i=0;
    while(query.next())
    {
        ui->twStock->setItem(i,0,new
                             QTableWidgetItem(query.value("nomenclature_name").toString()));
        ui->twStock->setItem(i,1,new
                             QTableWidgetItem(query.value("nomenclature_type").toString()));
        ui->twStock->setItem(i,2,new
                             QTableWidgetItem(query.value("purchased").toString()));
        ui->twStock->setItem(i,3,new
                             QTableWidgetItem(query.value("spent").toString()));
        ui->twStock->setItem(i,4,new
                             QTableWidgetItem(query.value("stock").toString()));
        i++;
    }
    ui->twStock->resizeColumnsToContents();
    ui->twStock->resizeRowsToContents();
}

void NomenclatureInStock::logout()
{
    this->close();
}
