#include "carpinpondformanager.h"
#include "ui_carpinpondformanager.h"
#include <iostream>
#include <QDebug>

CarpInPondForManager::CarpInPondForManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CarpInPondForManager)
{
    ui->setupUi(this);
    connect(ui->logout, &QPushButton::clicked, this, &CarpInPondForManager::logout);
    showCarpsInStock();
}

CarpInPondForManager::~CarpInPondForManager()
{
    delete ui;
}

void CarpInPondForManager::showCarpsInStock()
{
    // Очистка таблицы перед выводом новых данных
    ui->twOrg->clearContents();
    // Установка ширины столбцов
    ui->twOrg->setColumnCount(5);
    // Возможность прокрутки
    ui->twOrg->setAutoScroll(true);
    // Режим выделения ячеек - только одна строка
    ui->twOrg->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twOrg->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Заголовки таблицы
    ui->twOrg->setHorizontalHeaderItem(0,new QTableWidgetItem("Вид карпа"));
    ui->twOrg->setHorizontalHeaderItem(1,new QTableWidgetItem("Тип карпа"));
    ui->twOrg->setHorizontalHeaderItem(2,new QTableWidgetItem("Пруд"));
    ui->twOrg->setHorizontalHeaderItem(3,new QTableWidgetItem("Количество"));
    ui->twOrg->setHorizontalHeaderItem(4,new QTableWidgetItem("Дата запуска"));
    // Последний столбец растягивается при изменении размера формы
    ui->twOrg->horizontalHeader()->setStretchLastSection(true);
    // Разрешаем сортировку пользователю
    ui->twOrg->setSortingEnabled(true);
    //ui->twOrg->sortByColumn(0);
    // Запрет на изменение ячеек таблицы при отображении
    ui->twOrg->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Очистить содержимое компонента
    ui->twOrg->clearContents();
    // Если соединение не открыто, то вызвать нашу функцию для открытия
    // если подключиться не удалось, то вывести сообщение об ошибке и
    // выйти из функции
    // Создать объект запроса с привязкой к установленному соединению
    QSqlQuery query(db);
    // Создать строку запроса на выборку данных
    QString queryString = QString("SELECT carp.species, carp.carp_type, pond.pond_name, carp_in_pond.quantity, carp_in_pond.start_date "
                                  "FROM carp_in_pond "
                                  "JOIN carp ON carp.carp_id = carp_in_pond.carp_id "
                                  "JOIN pond ON pond.pond_id = carp_in_pond.pond_id");
    // Выполнить запрос и поверить его успешность
    if (!query.exec(queryString)) {
        qDebug() << "Query execution error:" << query.lastError().text();
        return;
    }
    // Если запрос активен (успешно завершен),
    // то вывести сообщение о прочитанном количестве строк в окно вывода
    // и установить количество строк для компонента таблицы
    if( query.isActive())
        ui->twOrg->setRowCount(query.size());
    else
        ui->twOrg->setRowCount(0);
    // Прочитать в цикле все строки результата (курсора)
    // и вывести их в компонент таблицы
    int i=0;
    while(query.next())
    {
        ui->twOrg->setItem(i,0,new
                           QTableWidgetItem(query.value("species").toString()));
        ui->twOrg->setItem(i,1,new
                           QTableWidgetItem(query.value("carp_type").toString()));
        ui->twOrg->setItem(i,2,new
                           QTableWidgetItem(query.value("pond_name").toString()));
        ui->twOrg->setItem(i,3,new
                           QTableWidgetItem(query.value("quantity").toString()));
        ui->twOrg->setItem(i,4,new
                           QTableWidgetItem(query.value("start_date").toString()));
        i++;
    }
    ui->twOrg->resizeColumnsToContents();
    ui->twOrg->resizeRowsToContents();
}

void CarpInPondForManager::logout()
{
    this->close();
}




