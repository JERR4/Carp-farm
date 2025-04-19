#include "customerinfo.h"
#include "ui_customerinfo.h"
#include <QDebug>

CustomerInfo::CustomerInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomerInfo),
    gistogram(nullptr)
{
    ui->setupUi(this);
    connect(ui->gistogram, &QPushButton::clicked, this, &CustomerInfo::showGistogram);
    connect(ui->logout, &QPushButton::clicked, this, &CustomerInfo::logout);
    showCustomerInfo();
}

CustomerInfo::~CustomerInfo()
{
    db.close(); // Закрываем соединение с базой данных
    delete ui;
}

void CustomerInfo::showCustomerInfo()
{
    // Очистка таблицы перед выводом новых данных
    ui->twOrg->clearContents();
    // Установка ширины столбцов
    ui->twOrg->setColumnCount(5); // Учитывая дополнительные столбцы
    // Возможность прокрутки
    ui->twOrg->setAutoScroll(true);
    // Режим выделения ячеек - только одна строка
    ui->twOrg->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twOrg->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Заголовки таблицы
    ui->twOrg->setHorizontalHeaderItem(0, new QTableWidgetItem("Покупатель"));
    ui->twOrg->setHorizontalHeaderItem(1, new QTableWidgetItem("Кол-во покупок"));
    ui->twOrg->setHorizontalHeaderItem(2, new QTableWidgetItem("Средняя цена за кг"));
    ui->twOrg->setHorizontalHeaderItem(3, new QTableWidgetItem("Средняя цена покупки"));
    ui->twOrg->setHorizontalHeaderItem(4, new QTableWidgetItem("Общий выкуп"));
    // Последний столбец растягивается при изменении размера формы
    ui->twOrg->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Разрешаем сортировку пользователю
    ui->twOrg->setSortingEnabled(true);
    // Запрет на изменение ячеек таблицы при отображении
    ui->twOrg->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Очистить содержимое компонента
    ui->twOrg->clearContents();
    QSqlQuery query(db);
    QString queryString = "SELECT "
                          "c.counterparty_name AS Customer_Name, "
                          "COUNT(s.sale_id) AS Number_of_Purchases, "
                          "ROUND(AVG(s.price_per_kilo), 2) AS Average_Price_per_Kilo, "
                          "ROUND(AVG(s.price_per_kilo * s.weight), 2) AS Average_Purchase_Price, "
                          "ROUND(SUM(s.weight * s.price_per_kilo), 2) AS Total_Purchase_Amount "
                          "FROM sale s "
                          "JOIN counterparty c ON s.counterparty_id = c.counterparty_id "
                          "GROUP BY c.counterparty_name";

    if (!query.exec(queryString)) {
        qDebug() << "Query execution error:" << query.lastError().text();
        return;
    }

    ui->twOrg->clearContents();
    ui->twOrg->setColumnCount(5);
    ui->twOrg->setRowCount(query.size());

    int row = 0;
    while (query.next()) {
        ui->twOrg->setItem(row, 0, new QTableWidgetItem(query.value("Customer_Name").toString()));
        ui->twOrg->setItem(row, 1, new QTableWidgetItem(query.value("Number_of_Purchases").toString()));
        ui->twOrg->setItem(row, 2, new QTableWidgetItem(query.value("Average_Price_per_Kilo").toString()));
        ui->twOrg->setItem(row, 3, new QTableWidgetItem(query.value("Average_Purchase_Price").toString()));
        ui->twOrg->setItem(row, 4, new QTableWidgetItem(query.value("Total_Purchase_Amount").toString()));
        ++row;
    }

    ui->twOrg->resizeColumnsToContents();
}

void CustomerInfo::showGistogram()
{
    if (!gistogram)
    {
        gistogram = new GistogrammTotalPurchase;
        connect(gistogram, &GistogrammTotalPurchase::finished, this, &CustomerInfo::closeGistogram);
    }
    gistogram->exec();
}

void CustomerInfo::closeGistogram()
{
    delete gistogram;
    gistogram = nullptr;
}

void CustomerInfo::logout()
{
    this->close();
}
