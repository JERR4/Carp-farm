#ifndef COUNTERPARTYREPORT_H
#define COUNTERPARTYREPORT_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>

namespace Ui {
class CounterpartyReport;
}

class CounterpartyReport : public QDialog
{
    Q_OBJECT

public:
    explicit CounterpartyReport(QWidget *parent = nullptr);
    ~CounterpartyReport();

    void showCounterparties();

private:
    Ui::CounterpartyReport *ui;
    QSqlDatabase db;
    void logout();
};

#endif // COUNTERPARTYREPORT_H
