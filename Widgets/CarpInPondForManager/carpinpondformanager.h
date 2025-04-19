#ifndef CARPINPONDFORMANAGER_H
#define CARPINPONDFORMANAGER_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>

namespace Ui {
class CarpInPondForManager;
}

class CarpInPondForManager : public QDialog
{
    Q_OBJECT

public:
    explicit CarpInPondForManager(QWidget *parent = nullptr);
    ~CarpInPondForManager();

    void showCarpsInStock();

private:
    Ui::CarpInPondForManager *ui;
    QSqlDatabase db;
    void logout();
};

#endif // CARPINPONDFORMANAGER_H
