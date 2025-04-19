#include "usersreport.h"
#include "ui_usersreport.h"
#include <iostream>
#include <QDebug>

UsersReport::UsersReport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UsersReport)
{
    ui->setupUi(this);
    connect(ui->logoutButton, &QPushButton::clicked, this, &UsersReport::logout);
    showUsersReport();
}

UsersReport::~UsersReport()
{
    delete ui;
}

void UsersReport::showUsersReport()
{
    // Очистка таблицы перед выводом новых данных
    ui->twUsers->clearContents();
    // Установка ширины столбцов
    ui->twUsers->setColumnCount(3);
    // Возможность прокрутки
    ui->twUsers->setAutoScroll(true);
    // Режим выделения ячеек - только одна строка
    ui->twUsers->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twUsers->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Заголовки таблицы
    ui->twUsers->setHorizontalHeaderItem(0, new QTableWidgetItem("Роль пользователя"));
    ui->twUsers->setHorizontalHeaderItem(1, new QTableWidgetItem("Логин"));
    ui->twUsers->setHorizontalHeaderItem(2, new QTableWidgetItem("Пароль"));
    // Последний столбец растягивается при изменении размера формы
    ui->twUsers->horizontalHeader()->setStretchLastSection(true);
    // Разрешаем сортировку пользователю
    ui->twUsers->setSortingEnabled(true);
    //ui->twUsers->sortByColumn(0);
    // Запрет на изменение ячеек таблицы при отображении
    ui->twUsers->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Очистить содержимое компонента
    ui->twUsers->clearContents();
    // Если соединение не открыто, то вызвать нашу функцию для открытия
    // если подключиться не удалось, то вывести сообщение об ошибке и
    // выйти из функции
    // Создать объект запроса с привязкой к установленному соединению
    QSqlQuery query(db);
    // Создать строку запроса на выборку данных
    QString queryString = QString("SELECT users.user_role, users.user_login, users.user_pass "
                                  "FROM users");
    // Выполнить запрос и проверить его успешность
    if (!query.exec(queryString)) {
        qDebug() << "Query execution error:" << query.lastError().text();
        return;
    }
    // Если запрос активен (успешно завершен),
    // то вывести сообщение о прочитанном количестве строк в окно вывода
    // и установить количество строк для компонента таблицы
    if (query.isActive())
        ui->twUsers->setRowCount(query.size());
    else
        ui->twUsers->setRowCount(0);
    // Прочитать в цикле все строки результата (курсора)
    // и вывести их в компонент таблицы
    int i = 0;
    while (query.next()) {
        ui->twUsers->setItem(i, 0, new QTableWidgetItem(query.value("user_role").toString()));
        ui->twUsers->setItem(i, 1, new QTableWidgetItem(query.value("user_login").toString()));
        ui->twUsers->setItem(i, 2, new QTableWidgetItem(query.value("user_pass").toString()));
        i++;
    }
    ui->twUsers->resizeColumnsToContents();
    ui->twUsers->resizeRowsToContents();
}

void UsersReport::logout()
{
    this->close();
}
