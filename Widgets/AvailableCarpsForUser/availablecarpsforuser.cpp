#include "availablecarpsforuser.h"
#include "ui_availablecarpsforuser.h"
#include <iostream>
#include <QDebug>

availablecarpsforuser::availablecarpsforuser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::availablecarpsforuser)
{
    ui->setupUi(this);
    connect(ui->logout, &QPushButton::clicked, this, &availablecarpsforuser::logout);
    showCarpsInStock();
}

availablecarpsforuser::~availablecarpsforuser()
{
    delete ui;
}

void availablecarpsforuser::showCarpsInStock()
{
    // Очистка таблицы перед выводом новых данных
    ui->twOrg->clearContents();
    // Установка ширины столбцов
    ui->twOrg->setColumnCount(3);
    // Возможность прокрутки
    ui->twOrg->setAutoScroll(true);
    // Режим выделения ячеек - только одна строка
    ui->twOrg->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twOrg->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Заголовки таблицы
    ui->twOrg->setHorizontalHeaderItem(0,new QTableWidgetItem("Вид карпа"));
    ui->twOrg->setHorizontalHeaderItem(1,new QTableWidgetItem("Тип карпа"));
    ui->twOrg->setHorizontalHeaderItem(2,new QTableWidgetItem("Количество"));
    // Последний столбец растягивается при изменении размера формы
    ui->twOrg->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
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
    QString queryString = QString("SELECT carp.species, carp.carp_type, SUM(carp_in_pond.quantity) AS total_quantity "
                                  "FROM carp_in_pond "
                                  "JOIN carp ON carp.carp_id = carp_in_pond.carp_id "
                                  "GROUP BY carp.species, carp.carp_type;");
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
                           QTableWidgetItem(query.value("total_quantity").toString()));
        i++;
    }
}

void availablecarpsforuser::logout()
{
    this->close();
}



