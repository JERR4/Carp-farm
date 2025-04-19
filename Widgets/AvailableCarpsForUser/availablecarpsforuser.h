#ifndef AVAILABLECARPSFORUSER_H
#define AVAILABLECARPSFORUSER_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>

namespace Ui {
class availablecarpsforuser;
}

class availablecarpsforuser : public QDialog
{
    Q_OBJECT

public:
    explicit availablecarpsforuser(QWidget *parent = nullptr);
    ~availablecarpsforuser();

    void showCarpsInStock();

private:
    Ui::availablecarpsforuser *ui;
    QSqlDatabase db;
    void logout();
};

#endif // availablecarpsforuser_H
