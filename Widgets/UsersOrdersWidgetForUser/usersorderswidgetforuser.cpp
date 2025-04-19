#include "usersorderswidgetforuser.h"
#include "ui_usersorderswidgetforuser.h"
#include <iostream>
#include <QDebug>

UsersOrdersWidgetForUser::UsersOrdersWidgetForUser(QString login_param, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UsersOrdersWidgetForUser),
    login(login_param)
{
    ui->setupUi(this);
    connect(ui->logout, &QPushButton::clicked, this, &UsersOrdersWidgetForUser::logout);
    showOrders();
}

UsersOrdersWidgetForUser::~UsersOrdersWidgetForUser()
{
    delete ui;
}

void UsersOrdersWidgetForUser::showOrders()
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
    ui->twOrg->setHorizontalHeaderItem(0,new QTableWidgetItem("Дата"));
    ui->twOrg->setHorizontalHeaderItem(1,new QTableWidgetItem("Вид карпа"));
    ui->twOrg->setHorizontalHeaderItem(2,new QTableWidgetItem("Масса"));
    ui->twOrg->setHorizontalHeaderItem(3,new QTableWidgetItem("Цена за кг"));
    ui->twOrg->setHorizontalHeaderItem(4,new QTableWidgetItem("Кол-во"));
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
    QString queryString = QString("select * from usersordersforuser('%1')").arg(login);
    query.prepare(queryString);

    qDebug() << "Executing SQL query:" << query.executedQuery();
    qDebug() << "Bound values:" << query.boundValues();
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
                           QTableWidgetItem(query.value("sale_date").toString()));
        ui->twOrg->setItem(i,1,new
                           QTableWidgetItem(query.value("species").toString()));
        ui->twOrg->setItem(i,2,new
                           QTableWidgetItem(query.value("weight").toString()));
        ui->twOrg->setItem(i,3,new
                           QTableWidgetItem(query.value("price_per_kilo").toString()));
        ui->twOrg->setItem(i,4,new
                           QTableWidgetItem(query.value("carp_quantity").toString()));
        i++;
    }
}

void UsersOrdersWidgetForUser::logout()
{
    this->close();
}

