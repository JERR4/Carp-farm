#ifndef EMPLOYEEGROUPFORM_H
#define EMPLOYEEGROUPFORM_H

#include <QDialog>
#include <QSqlDatabase>
#include <QVector>
#include <QStringList>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include "../employeeGroupsForUser/employeegroupsforuser.h"

namespace Ui {
class EmployeeGroupForm;
}

struct EmployeeGroup {
    int employee_group_id;
    QString authority_limitation;
    QString group_number;
};

class EmployeeGroupForm : public QDialog
{
    Q_OBJECT

public:
    explicit EmployeeGroupForm(QWidget *parent = nullptr);
    void showEmployeeGroup();
    void loadAuthorityLimitations();
    ~EmployeeGroupForm();

private slots:
    void next();
    void previous();
    void display();
    void add();
    void save();
    void deleteThis();
    void searchEmployeeGroup(const QString &text);
    void showEmployeeGroupForUser();
    void EmployeeGroupForUserClosed();
private:
    Ui::EmployeeGroupForm *ui;
    QSqlDatabase db;
    QVector<EmployeeGroup> employeeGroups;
    int currentEmployeeGroupIndex;
    int currentEmployeeGroupId;
    QStringList authorityLimitations;
    QString searchFilter;
    employeeGroupsForUser *employeegr;
    void logout();
};

#endif // EMPLOYEEGROUPFORM_H
