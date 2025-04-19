#ifndef PONDSFORUSER_H
#define PONDSFORUSER_H

#include <QDialog>
#include <QTableWidget>
#include <QtSql>
#include <QTextBrowser>
#include "../DiagramPonds/diagrammponds.h"

namespace Ui {
class pondsforuser;
}

class pondsforuser : public QDialog
{
    Q_OBJECT

public:
    explicit pondsforuser(QWidget *parent = nullptr);
    ~pondsforuser();

    void showPonds();
    void showDiagram();
    void closeDiagram();

private:
    Ui::pondsforuser *ui;
    QSqlDatabase db;
    DiagrammPonds *pond;
    void logout();
};

#endif // PONDSFORUSER_H
