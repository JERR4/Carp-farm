#ifndef PURCHASEREPORT_H
#define PURCHASEREPORT_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>

namespace Ui {
class PurchaseReport;
}

class PurchaseReport : public QDialog
{
    Q_OBJECT

public:
    explicit PurchaseReport(QWidget *parent = nullptr);
    ~PurchaseReport();

    void showPurchaseReport();

private:
    Ui::PurchaseReport *ui;
    QSqlDatabase db;
    QString searchFilter;
    void logout();
private slots:
    void search(const QString &text);
};

#endif // PURCHASEREPORT_H
