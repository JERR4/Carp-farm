#ifndef COUNTERPARTYFORM_H
#define COUNTERPARTYFORM_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>
#include "../CounterpartyReport/counterpartyreport.h"

namespace Ui {
class CounterpartyForm;
}

// Структура для представления контрагента
struct Counterparty {
    int counterparty_id;
    QString counterparty_name;
    QString counterparty_type;
    QString phone_number;
    QString email;
    QString address;
};

class CounterpartyForm : public QDialog
{
    Q_OBJECT

public:
    explicit CounterpartyForm(QWidget *parent = nullptr);
    void showCounterparty();
    void loadCounterpartyTypes();
    ~CounterpartyForm();

private slots:
    void next();
    void previous();
    void display();
    void add();
    void save();
    void delete_this();
    void openGoogleMail();
    void searchCounterparty(const QString &text);
    void updatePhoneNumberMask(const QString &text);
    void updateEmailMask(const QString &tex);
    void showCounterpartyReport();
    void CounterpartyReportClosed();

private:
    Ui::CounterpartyForm *ui;
    QSqlDatabase db;
    QVector<Counterparty> counterparties;
    int currentCounterpartyIndex;
    int currentCounterpartyId;
    QStringList counterpartyTypes;
    QString searchFilter;
    int previousCursorPosition;
    CounterpartyReport *counterpartyReportForm;
    void logout();
};

#endif // COUNTERPARTYFORM_H
