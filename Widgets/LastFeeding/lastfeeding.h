#ifndef LASTFEEDING_H
#define LASTFEEDING_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>

namespace Ui {
class LastFeeding;
}

class LastFeeding : public QDialog
{
    Q_OBJECT

public:
    explicit LastFeeding(QWidget *parent = nullptr);
    ~LastFeeding();
    void showLastFeeding();

private:
    Ui::LastFeeding *ui;
    QSqlDatabase db;
    void logout();
};

#endif // LASTFEEDING_H
