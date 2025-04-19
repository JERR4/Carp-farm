#ifndef EMPLOYEEGROUPSFORUSER_H
#define EMPLOYEEGROUPSFORUSER_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>

namespace Ui {
class employeeGroupsForUser;
}

class employeeGroupsForUser : public QDialog
{
    Q_OBJECT

public:
    explicit employeeGroupsForUser(QWidget *parent = nullptr);
    ~employeeGroupsForUser();

    void showEmployeeGroups();

private:
    Ui::employeeGroupsForUser *ui;
    QSqlDatabase db;
    void logout();
};

#endif // EMPLOYEEGROUPSFORUSER_H
