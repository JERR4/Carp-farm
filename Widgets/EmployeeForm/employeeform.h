#ifndef EMPLOYEEFORM_H
#define EMPLOYEEFORM_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>
#include "../EmployeeReport/employeereport.h"

namespace Ui {
class EmployeeForm;
}

// Структура для представления сотрудника
struct Employee {
    int employee_id;
    QString full_name;
    QString phone_number;
    QString email;
    int employee_group_id;
};

class EmployeeForm : public QDialog
{
    Q_OBJECT

public:
    explicit EmployeeForm(QWidget *parent = nullptr);
    void showEmployee();
    void loadEmployeeGroups();
    ~EmployeeForm();

private slots:
    void next();
    void previous();
    void display();
    void add();
    void save();
    void delete_this();
    void openGoogleMail();
    void searchEmployee(const QString &text);
    void updatePhoneNumberMask(const QString &text);
    void updateEmailMask(const QString &text);
    void showEmployeeReport();
    void EmployeeReportClosed();

private:
    Ui::EmployeeForm *ui;
    QSqlDatabase db;
    QVector<Employee> employees;
    int currentEmployeeIndex;
    int currentEmployeeId;
    QVector<QPair<int, QString>> employeeGroups;
    QString searchFilter;
    int previousCursorPosition;
    void logout();
    EmployeeReport *employeeReport;
};

#endif // EMPLOYEEFORM_H
