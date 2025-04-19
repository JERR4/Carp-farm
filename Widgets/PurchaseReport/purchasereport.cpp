#include "purchasereport.h"
#include "ui_purchasereport.h"
#include <iostream>
#include <QDebug>

PurchaseReport::PurchaseReport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PurchaseReport)
{
    ui->setupUi(this);
    connect(ui->lineEditSearch, &QLineEdit::textChanged, this, &PurchaseReport::search);
    connect(ui->logoutButton, &QPushButton::clicked, this, &PurchaseReport::logout);
    showPurchaseReport();
}

PurchaseReport::~PurchaseReport()
{
    delete ui;
}

void PurchaseReport::showPurchaseReport()
{
    ui->purchaseTableWidget->clearContents();
    ui->purchaseTableWidget->setColumnCount(6); // Assuming there are 7 columns in the purchase table
    ui->purchaseTableWidget->setAutoScroll(true);
    ui->purchaseTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->purchaseTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->purchaseTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("Дата"));
    ui->purchaseTableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("Номенклатура"));
    ui->purchaseTableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("Контраент"));
    ui->purchaseTableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem("Сотрудник"));
    ui->purchaseTableWidget->setHorizontalHeaderItem(4,new QTableWidgetItem("Количество"));
    ui->purchaseTableWidget->setHorizontalHeaderItem(5,new QTableWidgetItem("Цена за шт."));

    ui->purchaseTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->purchaseTableWidget->setSortingEnabled(true);
    ui->purchaseTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QSqlQuery query(db);
    QString queryString = QString("SELECT purchase.purchase_id, purchase.purchase_date, nomenclature.nomenclature_name, "
                                  "counterparty.counterparty_name, employee.full_name, purchase.nomenclature_quantity, "
                                  "purchase.unit_price "
                                  "FROM purchase "
                                  "JOIN nomenclature ON purchase.nomenclature_id = nomenclature.nomenclature_id "
                                  "JOIN counterparty ON purchase.counterparty_id = counterparty.counterparty_id "
                                  "JOIN employee ON purchase.employee_id = employee.employee_id ");
    if (!searchFilter.isEmpty()) {
        queryString += " WHERE nomenclature.nomenclature_name LIKE '" + searchFilter + "%'";
    }

    if (!query.exec(queryString)) {
        qDebug() << "Query execution error:" << query.lastError().text();
        return;
    }

    if(query.isActive())
        ui->purchaseTableWidget->setRowCount(query.size());
    else
        ui->purchaseTableWidget->setRowCount(0);

    int i = 0;
    while(query.next())
    {
        ui->purchaseTableWidget->setItem(i,0,new QTableWidgetItem(query.value("purchase_date").toString()));
        ui->purchaseTableWidget->setItem(i,1,new QTableWidgetItem(query.value("nomenclature_name").toString()));
        ui->purchaseTableWidget->setItem(i,2,new QTableWidgetItem(query.value("counterparty_name").toString()));
        ui->purchaseTableWidget->setItem(i,3,new QTableWidgetItem(query.value("full_name").toString()));
        ui->purchaseTableWidget->setItem(i,4,new QTableWidgetItem(query.value("nomenclature_quantity").toString()));
        ui->purchaseTableWidget->setItem(i,5,new QTableWidgetItem(query.value("unit_price").toString()));
        i++;
    }

    // Адаптация ширины purchaseTableWidget под ширину содержимого
    ui->purchaseTableWidget->resizeColumnsToContents();
    ui->purchaseTableWidget->resizeRowsToContents();
}

void PurchaseReport::search(const QString &text) {
    searchFilter = text;
    showPurchaseReport();
}


void PurchaseReport::logout()
{
    this->close();
}
