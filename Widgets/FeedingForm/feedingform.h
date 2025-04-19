#ifndef FEEDINGFORM_H
#define FEEDINGFORM_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>
#include "../FeedingReport/feedingreport.h"

namespace Ui {
class FeedingForm;
}

struct Feeding {
    int feeding_id;
    QString feeding_date;
    int pond_id;
    int nomenclature_id;
    QString used_quantity;
    int employee_group_id;
};

class FeedingForm : public QDialog
{
    Q_OBJECT

public:
    explicit FeedingForm(QWidget *parent = nullptr);
    void showFeeding();
    void loadPonds();
    void loadNomenclature();
    void loadEmployeeGroup();
    ~FeedingForm();
private:
    Ui::FeedingForm *ui;
    QSqlDatabase db;
    QVector<Feeding> feedings;
    int currentFeedingIndex;
    int currentFeedingId;
    FeedingReport *feedingReport;
    QVector<QPair<int, QString>> ponds;
    QVector<QPair<int, QString>> nomenclatures;
private slots:
    void next();
    void previous();
    void display();
    void add();
    void save();
    void delete_this();
    void on_dateEditMin_dateChanged(const QDate &date);
    void on_dateEditMax_dateChanged(const QDate &date);
    void logout();
    void showFeedingReport();
    void feedingReportClosed();
};

#endif // FEEDINGFORM_H
