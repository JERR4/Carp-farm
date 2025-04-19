#ifndef FEEDINGREPORT_H
#define FEEDINGREPORT_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>


namespace Ui {
class FeedingReport;
}

class FeedingReport : public QDialog
{
    Q_OBJECT

public:
    explicit FeedingReport(QWidget *parent = nullptr);
    ~FeedingReport();

    void showFeedingRecords();

private:
    Ui::FeedingReport *ui;
    QSqlDatabase db;
    void logout();
};

#endif // FEEDINGREPORT_H
