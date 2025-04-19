#ifndef EMPLOYEEREPORT_H
#define EMPLOYEEREPORT_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>

namespace Ui {
class EmployeeReport;
}

class EmployeeReport : public QDialog
{
    Q_OBJECT

public:
    explicit EmployeeReport(QWidget *parent = nullptr);
    ~EmployeeReport();

    void showEmployeeDetails();

private:
    Ui::EmployeeReport *ui;
    QSqlDatabase db;
    void logout();
};

#endif // EMPLOYEEREPORT_H
