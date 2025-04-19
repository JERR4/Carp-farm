#include "MainWindow/mainwindow.h"
#include "UserWindow/userwindow.h"
#include "SpecialistWindow/specialistwindow.h"
#include "ManagerWindow/managerwindow.h"
#include "AdminWindow/adminwindow.h"

#include <string>
#include <iostream>

#include <QLocale>
#include <QTranslator>
#include <QApplication>
#include <QSqlDatabase>
#include <QtSql>
#include <QDebug>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");

    db.setHostName("127.0.0.1");
    db.setDatabaseName("carp_farm");
    db.setUserName("admin");
    db.setPassword("root");

    if (db.open()) {
        MainWindow w;

        // Обработчик события "Войти"
        QObject::connect(&w, &MainWindow::loginButtonClicked, [&]() {
            // Получение введенного логина и пароля
            QString login = w.getLogin();
            QString password = w.getPassword();
            // Формирование SQL-запроса с вставкой значений
            QString queryStr = "SELECT * FROM Users WHERE user_login = :login AND user_pass = :password";

            // Подготовка SQL-запроса
            QSqlQuery query;
            query.prepare(queryStr);
            query.bindValue(":login", login);
            query.bindValue(":password", password);

            // Выполнение запроса
            if (query.exec()) {
                if (query.next()) {
                    w.hide();
                    if (query.value("user_role").toString() == "Покупатель") {
                        UserWindow userWindow (login);
                        QObject::connect(&userWindow, &UserWindow::finished, [&w]() {
                            w.show(); // Отображаем главное окно при закрытии UserWindow
                        });
                        userWindow.exec();
                    } else if (query.value("user_role").toString() == "Специалист по уходу за рыбой и водоёмом") {
                        SpecialistWindow specialistWindow;
                        QObject::connect(&specialistWindow, &SpecialistWindow::finished, [&w]() {
                            w.show(); // Отображаем главное окно при закрытии SpecialistWindow
                        });
                        specialistWindow.exec();
                    } else if (query.value("user_role").toString() == "Менеджер по закупкам и продажам") {
                        ManagerWindow managerWindow;
                        QObject::connect(&managerWindow, &ManagerWindow::finished, [&w]() {
                            w.show(); // Отображаем главное окно при закрытии ManagerWindow
                        });
                        managerWindow.exec();
                    } else if (query.value("user_role").toString() == "Администратор") {
                        AdminWindow adminWindow;
                        QObject::connect(&adminWindow, &AdminWindow::finished, [&w]() {
                            w.show(); // Отображаем главное окно при закрытии EmployeeWindow
                        });
                        adminWindow.exec();
                    }
                } else {
                    // Обработка ситуации, когда запрос не вернул результатов
                    QMessageBox::warning(&w, "Ошибка авторизации", "Неверный логин или пароль!");
                }
            } else {
                // Вывод сообщения об ошибке в случае неудачи выполнения запроса
                qDebug() << "Query execution failed:" << query.lastError().text();
            }

        });

        w.show();
        return a.exec();

    } else {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
    }

    return 0;
}
