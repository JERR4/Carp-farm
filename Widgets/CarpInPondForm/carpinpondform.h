#ifndef CARPINPONDFORM_H
#define CARPINPONDFORM_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>
#include "../CarpInPondForManager/carpinpondformanager.h"

namespace Ui {
class CarpInPondForm;
}

struct CarpInPond {
    int carp_in_pond_id;
    QString start_date;
    int pond_id;
    int carp_id;
    QString quantity;
};

class CarpInPondForm : public QDialog
{
    Q_OBJECT

public:
    explicit CarpInPondForm(QWidget *parent = nullptr);
    void showCarpInPond();
    void loadPonds();
    void loadCarps();
    ~CarpInPondForm();

private:
    Ui::CarpInPondForm *ui;
    QSqlDatabase db;
    QVector<CarpInPond> carpInPonds;
    int currentCarpInPondIndex;
    int currentCarpInPondId;
    int previousCursorPosition;
    CarpInPondForManager *carpinpond;
    QVector<QPair<int, QString>> ponds;
    QVector<QPair<int, QString>> carps;

public slots:
    void next();
    void previous();
    void display();
    void add();
    void save();
    void delete_this();
    void on_dateEditMin_dateChanged(const QDate &date);
    void on_dateEditMax_dateChanged(const QDate &date);
    void logout();
    void showCarpInPondForm();
    void CarpInPondClosed();
};

#endif // CARPINPONDFORM_H
