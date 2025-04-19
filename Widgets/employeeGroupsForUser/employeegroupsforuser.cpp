#include "employeeGroupsForUser.h"
#include "ui_employeeGroupsForUser.h"
#include <iostream>
#include <QDebug>

employeeGroupsForUser::employeeGroupsForUser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::employeeGroupsForUser)
{
    ui->setupUi(this);
    connect(ui->logout, &QPushButton::clicked, this, &employeeGroupsForUser::logout);
    showEmployeeGroups();
}

employeeGroupsForUser::~employeeGroupsForUser()
{
    delete ui;
}

void employeeGroupsForUser::showEmployeeGroups()
{
    ui->twOrg->clearContents();
    ui->twOrg->setColumnCount(2);
    ui->twOrg->setAutoScroll(true);
    ui->twOrg->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twOrg->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->twOrg->setHorizontalHeaderItem(0,new QTableWidgetItem("Ограничение полномочий"));
    ui->twOrg->setHorizontalHeaderItem(1,new QTableWidgetItem("Номер группы"));
    ui->twOrg->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->twOrg->setSortingEnabled(true);
    ui->twOrg->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->twOrg->clearContents();
    QSqlQuery query(db);
    QString queryString = QString("SELECT employee_group_id, authority_limitation, group_number FROM employee_group");
    if (!query.exec(queryString)) {
        qDebug() << "Query execution error:" << query.lastError().text();
        return;
    }
    if( query.isActive())
        ui->twOrg->setRowCount(query.size());
    else
        ui->twOrg->setRowCount(0);
    int i=0;
    while(query.next())
    {
        ui->twOrg->setItem(i,0,new QTableWidgetItem(query.value("authority_limitation").toString()));
        ui->twOrg->setItem(i,1,new QTableWidgetItem(query.value("group_number").toString()));
        i++;
    }
}

void employeeGroupsForUser::logout()
{
    this->close();
}
