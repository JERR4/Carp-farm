#include "pondsforuser.h"
#include "ui_pondsforuser.h"
#include <iostream>
#include <QDebug>

pondsforuser::pondsforuser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::pondsforuser),
    pond(nullptr)
{
    ui->setupUi(this);
    connect(ui->diagram, &QPushButton::clicked, this, &pondsforuser::showDiagram);
    connect(ui->logout, &QPushButton::clicked, this, &pondsforuser::logout);
    showPonds();
}

pondsforuser::~pondsforuser()
{
    delete ui;
}

void pondsforuser::showPonds()
{
    // Очистка таблицы перед выводом новых данных
    ui->twOrg->clearContents();
    // Установка ширины столбцов
    ui->twOrg->setColumnCount(3);
    // Возможность прокрутки
    ui->twOrg->setAutoScroll(true);
    // Режим выделения ячеек - только одна строка
    ui->twOrg->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->twOrg->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Заголовки таблицы
    ui->twOrg->setHorizontalHeaderItem(0,new QTableWidgetItem("Название пруда"));
    ui->twOrg->setHorizontalHeaderItem(1,new QTableWidgetItem("Назначение"));
    ui->twOrg->setHorizontalHeaderItem(2,new QTableWidgetItem("Объём"));
    // Последний столбец растягивается при изменении размера формы
    ui->twOrg->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Разрешаем сортировку пользователю
    ui->twOrg->setSortingEnabled(true);
    //ui->twOrg->sortByColumn(0);
    // Запрет на изменение ячеек таблицы при отображении
    ui->twOrg->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Очистить содержимое компонента
    ui->twOrg->clearContents();
    // Если соединение не открыто, то вызвать нашу функцию для открытия
    // если подключиться не удалось, то вывести сообщение об ошибке и
    // выйти из функции
    // Создать объект запроса с привязкой к установленному соединению
    QSqlQuery query(db);
    // Создать строку запроса на выборку данных
    QString queryString = QString("select pond_name, purpose, volume from pond");
    // Выполнить запрос и поверить его успешность
    if (!query.exec(queryString)) {
        qDebug() << "Query execution error:" << query.lastError().text();
        return;
    }
    // Если запрос активен (успешно завершен),
    // то вывести сообщение о прочитанном количестве строк в окно вывода
    // и установить количество строк для компонента таблицы
    if( query.isActive())
        ui->twOrg->setRowCount(query.size());
    else
        ui->twOrg->setRowCount(0);
    // Прочитать в цикле все строки результата (курсора)
    // и вывести их в компонент таблицы
    int i=0;
    while(query.next())
    {
        ui->twOrg->setItem(i,0,new
                           QTableWidgetItem(query.value("pond_name").toString()));
        ui->twOrg->setItem(i,1,new
                           QTableWidgetItem(query.value("purpose").toString()));
        ui->twOrg->setItem(i,2,new
                           QTableWidgetItem(query.value("volume").toString()));
        i++;
    }
}
void pondsforuser::showDiagram()
{
    if (!pond)
    {
        pond = new DiagrammPonds();
        // Устанавливаем соединение для обработки сигнала о закрытии виджета
        connect(pond, &DiagrammPonds::finished, this, &pondsforuser::closeDiagram);
    }
    pond->exec();
}

void pondsforuser::closeDiagram()
{
    // Удаляем виджет карпов
    delete pond;
    pond = nullptr;
}

void pondsforuser::logout()
{
    this->close();
}

