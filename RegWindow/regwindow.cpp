#include "regwindow.h"
#include "ui_regwindow.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

RegWindow::RegWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegWindow)
{
    ui->setupUi(this);

    // Соединяем сигнал от кнопки с обработчиком
    connect(ui->registration, &QPushButton::clicked, this, &RegWindow::registerUser);
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
    connect(ui->logout, &QPushButton::clicked, this, &RegWindow::logout);
}

RegWindow::~RegWindow()
{
    delete ui;
}

void RegWindow::registerUser()
{
    // Получаем логин и пароль из полей формы
    QString login = ui->loginEdit->text();
    QString password = ui->passwordEdit->text();

    // Проверяем, что поля не пустые
    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Логин и пароль должны быть заполнены");
        return;
    }

    // Подготавливаем SQL-запрос для проверки уникальности логина
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM users WHERE user_login = :login");
    checkQuery.bindValue(":login", login);

    // Выполняем запрос
    if (!checkQuery.exec()) {
        QMessageBox::critical(this, "Ошибка", "Ошибка при выполнении запроса: " + checkQuery.lastError().text());
        return;
    }

    // Получаем результат запроса
    checkQuery.next();
    int userCount = checkQuery.value(0).toInt();

    // Проверяем, что логин уникален
    if (userCount > 0) {
        QMessageBox::warning(this, "Ошибка", "Пользователь с таким логином уже существует");
        return;
    }

    // Подготавливаем SQL-запрос для добавления нового пользователя
    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT INTO users (user_role, user_login, user_pass) VALUES ('Покупатель', :login, :password)");
    insertQuery.bindValue(":login", login);
    insertQuery.bindValue(":password", password);

    // Выполняем запрос на добавление нового пользователя
    if (!insertQuery.exec()) {
        QMessageBox::critical(this, "Ошибка", "Ошибка при добавлении пользователя: " + insertQuery.lastError().text());
        return;
    }

    // Выводим сообщение об успешной регистрации
    QMessageBox::information(this, "Успех", "Пользователь успешно зарегистрирован");
    this->close();
}

void RegWindow::logout()
{
    this->close();
}
