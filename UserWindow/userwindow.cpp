#include "userwindow.h"
#include "ui_userwindow.h"
#include <QtSql>

UserWindow::UserWindow(QString login_param, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserWindow),
    login(login_param),
    ordersWidgetForUser(nullptr),
    pondsWidgetForUser(nullptr),
    carpsWidgetForUser(nullptr)
{
    ui->setupUi(this);

    connect(ui->carps, &QPushButton::clicked, this, &UserWindow::showCarpsInStock);
    connect(ui->feeding, &QPushButton::clicked, this, &UserWindow::showOrders);
    connect(ui->ponds, &QPushButton::clicked, this, &UserWindow::showPonds);
    connect(ui->logout, &QPushButton::clicked, this, &UserWindow::logout);
}

UserWindow::~UserWindow()
{
    delete ui;
}

void UserWindow::showCarpsInStock()
{
    if (!carpsWidgetForUser)
    {
        carpsWidgetForUser = new availablecarpsforuser();
        // Устанавливаем соединение для обработки сигнала о закрытии виджета
        connect(carpsWidgetForUser, &availablecarpsforuser::finished, this, &UserWindow::carpsWidgetClosed);
    }

    carpsWidgetForUser->exec();
}

void UserWindow::carpsWidgetClosed()
{
    // Удаляем виджет карпов
    delete carpsWidgetForUser;
    carpsWidgetForUser = nullptr;
}

void UserWindow::showOrders(){
    if (!ordersWidgetForUser)
    {
        ordersWidgetForUser = new UsersOrdersWidgetForUser(login);
        connect(ordersWidgetForUser, &UsersOrdersWidgetForUser::finished, this, &UserWindow::ordersWidgetClosed);
    }

    ordersWidgetForUser->exec();
}

void UserWindow::ordersWidgetClosed()
{
    delete ordersWidgetForUser;
    ordersWidgetForUser = nullptr;
}

void UserWindow::showPonds(){
    if (!pondsWidgetForUser)
    {
        pondsWidgetForUser = new pondsforuser();
        connect(pondsWidgetForUser, &pondsforuser::finished, this, &UserWindow::pondsWidgetClosed);
    }

    pondsWidgetForUser->exec();
}

void UserWindow::pondsWidgetClosed()
{
    // Удаляем виджет книг
    delete pondsWidgetForUser;
    pondsWidgetForUser = nullptr;
}

void UserWindow::logout()
{
    this->close();
}
