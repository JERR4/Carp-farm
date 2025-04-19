#ifndef NOMENCLATUREINSTOCK_H
#define NOMENCLATUREINSTOCK_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>

namespace Ui {
class NomenclatureInStock;
}

class NomenclatureInStock : public QDialog
{
    Q_OBJECT

public:
    explicit NomenclatureInStock(QWidget *parent = nullptr);
    ~NomenclatureInStock();

    void showNomenclatureInStock();

private:
    Ui::NomenclatureInStock *ui;
    QSqlDatabase db;
    void logout();
};

#endif // NOMENCLATUREINSTOCK_H
