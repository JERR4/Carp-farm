#ifndef USERFORM_H
#define USERFORM_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVector>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include "../UsersReport/usersreport.h"

namespace Ui {
class UserForm;
}

struct User {
    int user_id;
    QString user_role;
    QString user_login;
    QString user_pass;
};

class UserForm : public QDialog
{
    Q_OBJECT

public:
    explicit UserForm(QWidget *parent = nullptr);
    void showUser();
    void loadUserRoles();
    ~UserForm();

private slots:
    void next();
    void previous();
    void display();
    void add();
    void save();
    void delete_this();
    void searchUser(const QString &text);
    void showUsersReport();
    void UsersReportClosed();

private:
    Ui::UserForm *ui;
    QSqlDatabase db;
    QVector<User> users;
    int currentUserIndex;
    int currentUserID;
    QStringList userRoles;
    QString searchFilter;
    UsersReport *usersReportForm;
    void logout();
};

#endif // USERFORM_H
