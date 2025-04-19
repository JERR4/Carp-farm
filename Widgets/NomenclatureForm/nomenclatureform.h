#ifndef NOMENCLATUREFORM_H
#define NOMENCLATUREFORM_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QLineEdit>
#include <QComboBox>
#include "../NomenclatureInStock/nomenclatureinstock.h"

namespace Ui {
class NomenclatureForm;
}

struct Nomenclature {
    int nomenclature_id;
    QString nomenclature_name;
    QString nomenclature_type;
};

class NomenclatureForm : public QDialog
{
    Q_OBJECT

public:
    explicit NomenclatureForm(QWidget *parent = nullptr);
    void showNomenclature();
    void loadNomenclatureTypes();
    ~NomenclatureForm();

private slots:
    void next();
    void previous();
    void display();
    void showNomenclatureInStock();
    void NomenclatureInStockClosed();
    void add();
    void save();
    void deleteThis();
    void searchNomenclature(const QString &text);

private:
    Ui::NomenclatureForm *ui;
    QSqlDatabase db;
    QVector<Nomenclature> nomenclatures;
    int currentNomenclatureIndex;
    int currentNomenclatureId;
    QStringList nomenclatureTypes;
    NomenclatureInStock *nomenclatureInStockForm;
    QString searchFilter;
    void logout();
};

#endif // NOMENCLATUREFORM_H
