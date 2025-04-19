#include "feedingreport.h"
#include "ui_feedingreport.h"
#include <iostream>
#include <QDebug>

FeedingReport::FeedingReport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FeedingReport)
{
    ui->setupUi(this);
    connect(ui->logoutButton, &QPushButton::clicked, this, &FeedingReport::logout);
    showFeedingRecords();
}

FeedingReport::~FeedingReport()
{
    delete ui;
}

void FeedingReport::showFeedingRecords()
{
    ui->feedingTable->clearContents();
    ui->feedingTable->setColumnCount(5);
    ui->feedingTable->setAutoScroll(true);
    ui->feedingTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->feedingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->feedingTable->setHorizontalHeaderItem(0,new QTableWidgetItem("Дата кормления"));
    ui->feedingTable->setHorizontalHeaderItem(1,new QTableWidgetItem("Пруд"));
    ui->feedingTable->setHorizontalHeaderItem(2,new QTableWidgetItem("Номенклатура"));
    ui->feedingTable->setHorizontalHeaderItem(3,new QTableWidgetItem("Использованное количество"));
    ui->feedingTable->setHorizontalHeaderItem(4,new QTableWidgetItem("Группа сотрудников"));
    ui->feedingTable->horizontalHeader()->setStretchLastSection(true);
    ui->feedingTable->setSortingEnabled(true);
    ui->feedingTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->feedingTable->clearContents();

    QSqlQuery query(db);
    QString queryString = QString("SELECT feeding.feeding_date, pond.pond_name, nomenclature.nomenclature_name, feeding.used_quantity, employee_group.group_number "
                                  "FROM feeding "
                                  "JOIN pond ON pond.pond_id = feeding.pond_id "
                                  "JOIN nomenclature ON nomenclature.nomenclature_id = feeding.nomenclature_id "
                                  "JOIN employee_group ON employee_group.employee_group_id = feeding.employee_group_id");
    if (!query.exec(queryString)) {
        qDebug() << "Query execution error:" << query.lastError().text();
        return;
    }
    if( query.isActive())
        ui->feedingTable->setRowCount(query.size());
    else
        ui->feedingTable->setRowCount(0);

    int i=0;
    while(query.next())
    {
        ui->feedingTable->setItem(i,0,new
                                  QTableWidgetItem(query.value("feeding_date").toString()));
        ui->feedingTable->setItem(i,1,new
                                  QTableWidgetItem(query.value("pond_name").toString()));
        ui->feedingTable->setItem(i,2,new
                                  QTableWidgetItem(query.value("nomenclature_name").toString()));
        ui->feedingTable->setItem(i,3,new
                                  QTableWidgetItem(query.value("used_quantity").toString()));
        ui->feedingTable->setItem(i,4,new
                                  QTableWidgetItem(query.value("group_number").toString()));
        i++;
    }
    ui->feedingTable->resizeColumnsToContents();
    ui->feedingTable->resizeRowsToContents();
}

void FeedingReport::logout()
{
    this->close();
}
