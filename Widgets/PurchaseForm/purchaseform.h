#ifndef PURCHASEFORM_H
#define PURCHASEFORM_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>
#include "../PurchaseReport/purchasereport.h"

namespace Ui {
class PurchaseForm;
}

struct Purchase {
    int purchase_id;
    QString purchase_date;
    int nomenclature_id;
    int counterparty_id;
    int employee_id;
    QString nomenclature_quantity;
    QString unit_price;
};

class PurchaseForm : public QDialog
{
    Q_OBJECT

public:
    explicit PurchaseForm(QWidget *parent = nullptr);
    void showPurchase();
    void loadNomenclatures();
    void loadCounterparties();
    void loadEmployees();
    ~PurchaseForm();

private:
    Ui::PurchaseForm *ui;
    QSqlDatabase db;
    QVector<Purchase> purchases;
    int currentPurchaseIndex;
    int currentPurchaseId;
    int previousCursorPosition;
    QVector<QPair<int, QString>> nomenclatures;
    QVector<QPair<int, QString>> counterparties;
    QVector<QPair<int, QString>> employees;
    PurchaseReport *purchaseReportForm;

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
    void showPurchaseReport(); // Новый слот для открытия отчета о покупках
    void purchaseReportClosed(); // Новый слот для закрытия отчета о покупках
};

#endif // PURCHASEFORM_H
