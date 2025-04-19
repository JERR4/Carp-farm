#ifndef USERSORDERSWIDGETFORUSER_H
#define USERSORDERSWIDGETFORUSER_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>

namespace Ui {
class UsersOrdersWidgetForUser;
}

class UsersOrdersWidgetForUser : public QDialog
{
    Q_OBJECT

public:
    explicit UsersOrdersWidgetForUser(QString login, QWidget *parent = nullptr);
    ~UsersOrdersWidgetForUser();

    void showOrders();

private:
    Ui::UsersOrdersWidgetForUser *ui;
    QSqlDatabase db;
    QString login;
    void logout();
};

#endif // USERSORDERSWIDGETFORUSER_H
