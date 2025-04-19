#ifndef USERSREPORT_H
#define USERSREPORT_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>

namespace Ui {
class UsersReport;
}

class UsersReport : public QDialog
{
    Q_OBJECT

public:
    explicit UsersReport(QWidget *parent = nullptr);
    ~UsersReport();

    void showUsersReport();

private:
    Ui::UsersReport *ui;
    QSqlDatabase db;
    void logout();
};

#endif // USERSREPORT_H
