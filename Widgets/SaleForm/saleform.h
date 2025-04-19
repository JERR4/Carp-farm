#ifndef SALEFORM_H
#define SALEFORM_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>
#include "../SaleReport/salereport.h"


namespace Ui {
class SaleForm;
}

struct Sale {
    int sale_id;
    QString sale_date;
    int carp_in_pond_id;
    int counterparty_id;
    int employee_id;
    QString weight;
    QString price_per_kilo;
    QString carp_quantity;
};

class SaleForm : public QDialog
{
    Q_OBJECT

public:
    explicit SaleForm(QWidget *parent = nullptr);
    void showSale();
    void loadCarpSpecies();
    void loadCounterparties();
    void loadEmployees();
    ~SaleForm();

private:
    Ui::SaleForm *ui;
    QSqlDatabase db;
    SaleReport *saleReportForm;
    QVector<Sale> sales;
    int currentSaleIndex;
    int currentSaleId;
    int previousCursorPosition;
    QVector<QPair<int, QString>> carpSpecies;
    QVector<QPair<int, QString>> counterparties;
    QVector<QPair<int, QString>> employees;

public slots:
    void next();
    void previous();
    void display();
    void add();
    void save();
    void delete_this();
    void setupValidators(const QString &text);
    void logout();
private slots:
    void on_dateEditMin_dateChanged(const QDate &date);
    void on_dateEditMax_dateChanged(const QDate &date);
    void showSaleReport();
    void SaleReportClosed();
    void getQueue(int sale_id, QString sale_date, QString species, QString counterparty, QString employee, QString weight, QString price_per_kilo, QString carp_quantity);
};

#endif // SALEFORM_H
