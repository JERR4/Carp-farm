#include "employeereport.h"
#include "ui_employeereport.h"
#include <iostream>
#include <QDebug>

EmployeeReport::EmployeeReport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EmployeeReport)
{
    ui->setupUi(this);
    connect(ui->logout, &QPushButton::clicked, this, &EmployeeReport::logout);
    showEmployeeDetails();
}

EmployeeReport::~EmployeeReport()
{
    delete ui;
}

void EmployeeReport::showEmployeeDetails()
{
    ui->twEmployees->clearContents();
    ui->twEmployees->setColumnCount(4);
    ui->twEmployees->setAutoScroll(true);
    ui->twEmployees->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twEmployees->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->twEmployees->setHorizontalHeaderItem(0,new QTableWidgetItem("ФИО"));
    ui->twEmployees->setHorizontalHeaderItem(1,new QTableWidgetItem("Телефон"));
    ui->twEmployees->setHorizontalHeaderItem(2,new QTableWidgetItem("Почта"));
    ui->twEmployees->setHorizontalHeaderItem(3,new QTableWidgetItem("Группа"));
    ui->twEmployees->horizontalHeader()->setStretchLastSection(true);
    ui->twEmployees->setSortingEnabled(true);
    ui->twEmployees->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->twEmployees->clearContents();

    QSqlQuery query(db);
    QString queryString = QString("SELECT employee.employee_id, employee.full_name, employee.phone_number, employee.email, "
                                  "employee_group.group_number || ' - ' || employee_group.authority_limitation AS employee_group "
                                  "FROM employee "
                                  "JOIN employee_group ON employee.employee_group_id = employee_group.employee_group_id");
    if (!query.exec(queryString)) {
        qDebug() << "Query execution error:" << query.lastError().text();
        return;
    }
    if( query.isActive())
        ui->twEmployees->setRowCount(query.size());
    else
        ui->twEmployees->setRowCount(0);

    int i=0;
    while(query.next())
    {
        ui->twEmployees->setItem(i,0,new
                                 QTableWidgetItem(query.value("full_name").toString()));
        ui->twEmployees->setItem(i,1,new
                                 QTableWidgetItem(query.value("phone_number").toString()));
        ui->twEmployees->setItem(i,2,new
                                 QTableWidgetItem(query.value("email").toString()));
        ui->twEmployees->setItem(i,3,new
                                 QTableWidgetItem(query.value("employee_group").toString()));
        i++;
    }
    ui->twEmployees->resizeColumnsToContents();
    ui->twEmployees->resizeRowsToContents();
}

void EmployeeReport::logout()
{
    this->close();
}
