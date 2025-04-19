#ifndef CUSTOMERINFO_H
#define CUSTOMERINFO_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>
#include "../GistogrammTotalPurchase/gistogrammtotalpurchase.h"

namespace Ui {
class CustomerInfo;
}

class CustomerInfo : public QDialog
{
    Q_OBJECT

public:
    explicit CustomerInfo(QWidget *parent = nullptr);
    ~CustomerInfo();

    void showCustomerInfo();
    void showGistogram();
    void closeGistogram();

private:
    Ui::CustomerInfo *ui;
    QSqlDatabase db;
    GistogrammTotalPurchase *gistogram;
    void logout();
};

#endif // CUSTOMERINFO_H
