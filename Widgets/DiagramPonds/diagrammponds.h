#ifndef DIAGRAMMPONDS_H
#define DIAGRAMMPONDS_H

#include <QDialog>
#include <QtCharts>
#include <QChartView>
#include <QPieSeries>
#include <QVBoxLayout>
#include <QtSql>

namespace Ui {
class DiagrammPonds;
}

class DiagrammPonds : public QDialog
{
    Q_OBJECT

public:
    explicit DiagrammPonds(QWidget *parent = nullptr);
    ~DiagrammPonds();

private:
    Ui::DiagrammPonds *ui;
    QSqlDatabase db;

    void showDiagramm();
    void logout();
};

#endif // DIAGRAMMPONDS_H
