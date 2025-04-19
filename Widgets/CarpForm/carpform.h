#ifndef CARPFORM_H
#define CARPFORM_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>
#include "../CarpForManager/carpformanager.h"

namespace Ui {
class CarpForm;
}

struct Carp {
    int carp_id;
    QString species;
    QString carp_type;
};

class CarpForm : public QDialog
{
    Q_OBJECT

public:
    explicit CarpForm(QWidget *parent = nullptr);
    void showCarp();
    void loadCarpTypes();
    ~CarpForm();

private slots:
    void next();
    void previous();
    void display();
    void add();
    void save();
    void delete_this();
    void searchSpecies(const QString &text);
    void showCarpForUser();
    void CarpForUserClosed();

private:
    Ui::CarpForm *ui;
    QSqlDatabase db;
    QVector<Carp> carps;
    int currentCarpIndex;
    int currentCarpId;
    QStringList carpTypes;
    QString searchFilter;
    CarpForManager *carpforuser;
    void logout();
};

#endif // CARPFORM_H
