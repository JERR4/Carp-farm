#ifndef SALEREPORT_H
#define SALEREPORT_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>

namespace Ui {
class SaleReport;
}

class SaleReport : public QDialog
{
    Q_OBJECT

public:
    explicit SaleReport(QWidget *parent = nullptr);
    ~SaleReport();

    void showSalesReport();

private:
    Ui::SaleReport *ui;
    QSqlDatabase db;
    QString searchFilterSpecies;
    QString searchFilterCounterparty;

    void logout();
private slots:
    void search(const QString &text);
    void searchCounterparty(const QString &text);
};

#endif // SALEREPORT_H
