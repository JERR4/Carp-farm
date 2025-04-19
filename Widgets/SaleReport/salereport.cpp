#include "salereport.h"
#include "ui_salereport.h"
#include <iostream>
#include <QDebug>

SaleReport::SaleReport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaleReport)
{
    ui->setupUi(this);
    connect(ui->lineEditSearch, &QLineEdit::textChanged, this, &SaleReport::search);
    connect(ui->lineEditSearchCounterparty, &QLineEdit::textChanged, this, &SaleReport::searchCounterparty);
    connect(ui->logoutButton, &QPushButton::clicked, this, &SaleReport::logout);
    showSalesReport();
}

SaleReport::~SaleReport()
{
    delete ui;
}

void SaleReport::showSalesReport()
{
    // Очистка таблицы перед выводом новых данных
    ui->twSaleReport->clearContents();
    // Установка ширины столбцов
    ui->twSaleReport->setColumnCount(7);
    // Возможность прокрутки
    ui->twSaleReport->setAutoScroll(true);
    // Режим выделения ячеек - только одна строка
    ui->twSaleReport->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twSaleReport->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Заголовки таблицы
    ui->twSaleReport->setHorizontalHeaderItem(0, new QTableWidgetItem("Дата продажи"));
    ui->twSaleReport->setHorizontalHeaderItem(1, new QTableWidgetItem("Карп"));
    ui->twSaleReport->setHorizontalHeaderItem(2, new QTableWidgetItem("Контрагент"));
    ui->twSaleReport->setHorizontalHeaderItem(3, new QTableWidgetItem("Сотрудник"));
    ui->twSaleReport->setHorizontalHeaderItem(4, new QTableWidgetItem("Масса (кг)"));
    ui->twSaleReport->setHorizontalHeaderItem(5, new QTableWidgetItem("Цена за кг"));
    ui->twSaleReport->setHorizontalHeaderItem(6, new QTableWidgetItem("Кол-во карпов"));
    // Последний столбец растягивается при изменении размера формы
    ui->twSaleReport->horizontalHeader()->setStretchLastSection(true);
    // Разрешаем сортировку пользователю
    ui->twSaleReport->setSortingEnabled(true);
    // Запрет на изменение ячеек таблицы при отображении
    ui->twSaleReport->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Очистить содержимое компонента
    ui->twSaleReport->clearContents();
    // Если соединение не открыто, то вызвать нашу функцию для открытия
    // если подключиться не удалось, то вывести сообщение об ошибке и
    // выйти из функции
    // Создать объект запроса с привязкой к установленному соединению
    QSqlQuery query(db);
    // Создать строку запроса на выборку данных
    QString queryString = QString("SELECT sale.sale_date, "
                                  "CONCAT(carp.species, ' - ', carp.carp_type, ' - ', pond.pond_name) AS carpy, "
                                  "counterparty.counterparty_name, "
                                  "employee.full_name, "
                                  "sale.weight, sale.price_per_kilo, sale.carp_quantity "
                                  "FROM sale "
                                  "JOIN carp_in_pond ON sale.carp_in_pond_id = carp_in_pond.carp_in_pond_id "
                                  "JOIN carp ON carp_in_pond.carp_id = carp.carp_id "
                                  "JOIN pond ON carp_in_pond.pond_id = pond.pond_id "
                                  "JOIN counterparty ON sale.counterparty_id = counterparty.counterparty_id "
                                  "JOIN employee ON sale.employee_id = employee.employee_id");
    if (!searchFilterSpecies.isEmpty() && searchFilterCounterparty.isEmpty()) {
        queryString += " WHERE carp.species LIKE '" + searchFilterSpecies + "%'";
    }
    if (searchFilterSpecies.isEmpty() && !searchFilterCounterparty.isEmpty()) {
        queryString += " WHERE counterparty.counterparty_name LIKE '" + searchFilterCounterparty + "%'";
    }
    if (!searchFilterSpecies.isEmpty() && !searchFilterCounterparty.isEmpty()) {
        queryString += " WHERE counterparty.counterparty_name LIKE '" + searchFilterCounterparty + "%' AND carp.species LIKE '"+ searchFilterSpecies + "%'";
    }
    // Выполнить запрос и поверить его успешность
    if (!query.exec(queryString)) {
        qDebug() << "Query execution error:" << query.lastError().text();
        return;
    }
    // Если запрос активен (успешно завершен),
    // то вывести сообщение о прочитанном количестве строк в окно вывода
    // и установить количество строк для компонента таблицы
    if (query.isActive())
        ui->twSaleReport->setRowCount(query.size());
    else
        ui->twSaleReport->setRowCount(0);
    // Прочитать в цикле все строки результата (курсора)
    // и вывести их в компонент таблицы
    int i = 0;
    while (query.next())
    {
        ui->twSaleReport->setItem(i, 0, new QTableWidgetItem(query.value("sale_date").toString()));
        ui->twSaleReport->setItem(i, 1, new QTableWidgetItem(query.value("carpy").toString()));
        ui->twSaleReport->setItem(i, 2, new QTableWidgetItem(query.value("counterparty_name").toString()));
        ui->twSaleReport->setItem(i, 3, new QTableWidgetItem(query.value("full_name").toString()));
        ui->twSaleReport->setItem(i, 4, new QTableWidgetItem(query.value("weight").toString()));
        ui->twSaleReport->setItem(i, 5, new QTableWidgetItem(query.value("price_per_kilo").toString()));
        ui->twSaleReport->setItem(i, 6, new QTableWidgetItem(query.value("carp_quantity").toString()));
        i++;
    }
    ui->twSaleReport->resizeColumnsToContents();
}

void SaleReport::search(const QString &text) {
    searchFilterSpecies = text;
    showSalesReport();
}

void SaleReport::searchCounterparty(const QString &text) {
    searchFilterCounterparty = text;
    showSalesReport();
}

void SaleReport::logout()
{
    this->close();
}
