#include "counterpartyreport.h"
#include "ui_counterpartyreport.h"
#include <QDebug>

CounterpartyReport::CounterpartyReport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CounterpartyReport)
{
    ui->setupUi(this);
    connect(ui->logoutButton, &QPushButton::clicked, this, &CounterpartyReport::logout);
    showCounterparties();
}

CounterpartyReport::~CounterpartyReport()
{
    delete ui;
}

void CounterpartyReport::showCounterparties()
{
    ui->counterpartyTable->clearContents();
    ui->counterpartyTable->setColumnCount(5);
    ui->counterpartyTable->setAutoScroll(true);
    ui->counterpartyTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->counterpartyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->counterpartyTable->setHorizontalHeaderItem(1, new QTableWidgetItem("Тип"));
    ui->counterpartyTable->setHorizontalHeaderItem(2, new QTableWidgetItem("Название"));
    ui->counterpartyTable->setHorizontalHeaderItem(3, new QTableWidgetItem("Телефон"));
    ui->counterpartyTable->setHorizontalHeaderItem(4, new QTableWidgetItem("Почта"));
    ui->counterpartyTable->setHorizontalHeaderItem(5, new QTableWidgetItem("Адрес"));
    ui->counterpartyTable->horizontalHeader()->setStretchLastSection(true);
    ui->counterpartyTable->setSortingEnabled(true);
    ui->counterpartyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QSqlQuery query(db);
    QString queryString = QString("SELECT * FROM counterparty");
    if (!query.exec(queryString)) {
        qDebug() << "Query execution error:" << query.lastError().text();
        return;
    }

    if (query.isActive())
        ui->counterpartyTable->setRowCount(query.size());
    else
        ui->counterpartyTable->setRowCount(0);

    int row = 0;
    while(query.next())
    {
        ui->counterpartyTable->setItem(row, 0, new QTableWidgetItem(query.value("counterparty_type").toString()));
        ui->counterpartyTable->setItem(row, 1, new QTableWidgetItem(query.value("counterparty_name").toString()));
        ui->counterpartyTable->setItem(row, 2, new QTableWidgetItem(query.value("phone_number").toString()));
        ui->counterpartyTable->setItem(row, 3, new QTableWidgetItem(query.value("email").toString()));
        ui->counterpartyTable->setItem(row, 4, new QTableWidgetItem(query.value("address").toString()));
        row++;
    }
    ui->counterpartyTable->resizeColumnsToContents();
    ui->counterpartyTable->resizeRowsToContents();
}

void CounterpartyReport::logout()
{
    this->close();
}
