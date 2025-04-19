#ifndef PONDFORM_H
#define PONDFORM_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>
#include "../PondsForUser/pondsforuser.h"

namespace Ui {
class PondForm;
}

struct Pond {
    int pond_id;
    QString pond_name;
    QString purpose;
    QString volume;
};

class PondForm : public QDialog
{
    Q_OBJECT

public:
    explicit PondForm(QWidget *parent = nullptr);
    void showPond();
    void loadPondPurposes();
    ~PondForm();

private slots:
    void next();
    void previous();
    void display();
    void add();
    void save();
    void delete_this();
    void searchPond(const QString &text);
    void ShowPondReport();
    void closePondReport();

private:
    Ui::PondForm *ui;
    QSqlDatabase db;
    QVector<Pond> ponds;
    int currentPondIndex;
    int currentPondId;
    QStringList pondPurposes;
    QString searchFilter;
    pondsforuser *pondforuser;
    void logout();
};

#endif // PONDFORM_H
