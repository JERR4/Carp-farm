#ifndef GISTOGRAMMTOTALPURCHASE_H
#define GISTOGRAMMTOTALPURCHASE_H

#include <QDialog>
#include <QtSql>
#include <QtCharts>
#include <QWidget>
#include <QRegularExpression>

namespace Ui {
class GistogrammTotalPurchase;
}

class GistogrammTotalPurchase : public QDialog
{
    Q_OBJECT

public:
    explicit GistogrammTotalPurchase(QWidget *parent = nullptr);
    ~GistogrammTotalPurchase();

    void drawGistogramm();

private:
    Ui::GistogrammTotalPurchase *ui;
    QSqlDatabase db;
    void logout();
};

#endif // GISTOGRAMMTOTALPURCHASE_H
