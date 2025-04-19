#include "carpformanager.h"
#include "ui_carpformanager.h"

CarpForManager::CarpForManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CarpForManager)
{
    ui->setupUi(this);
    connect(ui->logout, &QPushButton::clicked, this, &CarpForManager::logout);
    showCarp();
}

CarpForManager::~CarpForManager()
{
    delete ui;
}

void CarpForManager::showCarp()
{
    ui->twCarp->clearContents();
    ui->twCarp->setColumnCount(2);
    ui->twCarp->setAutoScroll(true);
    ui->twCarp->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twCarp->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->twCarp->setHorizontalHeaderItem(0, new QTableWidgetItem("Вид"));
    ui->twCarp->setHorizontalHeaderItem(1, new QTableWidgetItem("Тип"));
    ui->twCarp->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->twCarp->setSortingEnabled(true);
    ui->twCarp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QSqlQuery query(db);
    QString queryString = QString("SELECT species, carp_type FROM carp");
    if (!query.exec(queryString)) {
        qDebug() << "Query execution error:" << query.lastError().text();
        return;
    }
    if (query.isActive())
        ui->twCarp->setRowCount(query.size());
    else
        ui->twCarp->setRowCount(0);
    int i = 0;
    while (query.next())
    {
        ui->twCarp->setItem(i, 0, new QTableWidgetItem(query.value("species").toString()));
        ui->twCarp->setItem(i, 1, new QTableWidgetItem(query.value("carp_type").toString()));
        i++;
    }
}

void CarpForManager::logout()
{
    this->close();
}
