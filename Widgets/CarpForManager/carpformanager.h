#ifndef CARPFORMANAGER_H
#define CARPFORMANAGER_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QDebug>

namespace Ui {
class CarpForManager;
}

class CarpForManager : public QDialog
{
    Q_OBJECT

public:
    explicit CarpForManager(QWidget *parent = nullptr);
    ~CarpForManager();

    void showCarp();

private:
    Ui::CarpForManager *ui;
    QSqlDatabase db;
    void logout();
};

#endif // CARPFORMANAGER_H
