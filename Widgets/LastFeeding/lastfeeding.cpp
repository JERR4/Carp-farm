#include "lastfeeding.h"
#include "ui_lastfeeding.h"

LastFeeding::LastFeeding(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LastFeeding)
{
    ui->setupUi(this);
    connect(ui->logout, &QPushButton::clicked, this, &LastFeeding::logout);
    showLastFeeding();
}

void LastFeeding::showLastFeeding()
{
    // Очистка таблицы перед выводом новых данных
    ui->twOrg->clearContents();
    // Возможность прокрутки
    ui->twOrg->setAutoScroll(true);
    // Режим выделения ячеек - только одна строка
    ui->twOrg->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twOrg->setSelectionBehavior(QAbstractItemView::SelectRows);
    QSqlQuery query(db);
    // Создать строку запроса на выборку данных
    QString nomenclatureQueryString = QString("select nomenclature_name from nomenclature");
    if (query.exec(nomenclatureQueryString))
    {
        ui->twOrg->setColumnCount(query.size());
        // Заголовки таблицы
        int i=0;
        while(query.next())
        {
            ui->twOrg->setHorizontalHeaderItem(i,new QTableWidgetItem(query.value("nomenclature_name").toString()));
            ui->twOrg->resizeColumnToContents(i);
            if (ui->twOrg->columnWidth(i) < 90)
                ui->twOrg->setColumnWidth(i, 90);
            i++;
        }
    }
    // Создать строку запроса на выборку данных
    QString pondQueryString = QString("select pond_name from pond");
    if (query.exec(pondQueryString))
    {
        ui->twOrg->setRowCount(query.size());
        // Заголовки таблицы
        int i=0;
        while(query.next())
        {
            ui->twOrg->setVerticalHeaderItem(i,new QTableWidgetItem(query.value("pond_name").toString()));
            i++;
        }
    }
    for (int i = 0; i < ui->twOrg->rowCount(); i++) {
        for (int j = 0; j < ui->twOrg->columnCount(); j++) {
            QString pondName = ui->twOrg->verticalHeaderItem(i)->text();
            QString nomenclatureName = ui->twOrg->horizontalHeaderItem(j)->text();
            QString feedingQueryString = QString("SELECT MAX(feeding_date) AS last_feeding_date "
                                                 "FROM feeding "
                                                 "WHERE pond_id = (SELECT pond_id FROM pond WHERE pond_name = '%1') "
                                                 "AND nomenclature_id = (SELECT nomenclature_id FROM nomenclature WHERE nomenclature_name = '%2');")
                                             .arg(pondName).arg(nomenclatureName);
            // Выполнить запрос и проверить его успешность
            if (!query.exec(feedingQueryString)) {
                qDebug() << "Query execution error:" << query.lastError().text();
            }
            else {
                while(query.next())
                {
                    QTableWidgetItem *item = new QTableWidgetItem(query.value(0).toString());
                    item->setTextAlignment(Qt::AlignCenter); // Установка выравнивания по центру
                    ui->twOrg->setItem(i,j, item);
                }
            }
        }
    }
}


void LastFeeding::logout()
{
    this->close();
}

LastFeeding::~LastFeeding()
{
    delete ui;
}
