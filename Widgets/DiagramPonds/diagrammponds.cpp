#include "diagrammponds.h"
#include "ui_diagrammponds.h"

DiagrammPonds::DiagrammPonds(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DiagrammPonds)
{
    ui->setupUi(this);
    db = QSqlDatabase::database(); // Получение существующего соединения с базой данных
    if (!db.isValid()) {
        qDebug() << "Error: Failed to get database connection.";
        return;
    }
    showDiagramm();
}

DiagrammPonds::~DiagrammPonds()
{
    delete ui;
}

void DiagrammPonds::showDiagramm() {
    QSqlQuery query;
    query.exec("SELECT * FROM pond");

    QPieSeries *series = new QPieSeries();
    while (query.next()) {
        QString purpose = query.value("pond_name").toString();
        double totalVolume = query.value("volume").toDouble();

        series->append(purpose, totalVolume);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitleFont(QFont("Arial", 16));
    chart->setTitle("Объем прудов");
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);
    // Установка цвета фона диаграммы
    chart->setBackgroundBrush(QBrush(QColor(QColorConstants::Svg::lightgreen)));

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Макет для диаграммы
    QVBoxLayout *chartLayout = new QVBoxLayout;
    chartLayout->addWidget(chartView);

    // Создание кнопки logout и установка стилей
    QPushButton *logoutButton = new QPushButton("Закрыть");
    logoutButton->setStyleSheet("background-color: lightgreen;"); // Установка цвета фона
    logoutButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed); // Установка политики размеров
    connect(logoutButton, &QPushButton::clicked, this, &DiagrammPonds::logout);

    // Макет для кнопки logout
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(logoutButton); // Добавление кнопки в макет
    buttonLayout->setContentsMargins(0, 10, 0, 0); // Установка отступов

    // Основной макет, содержащий макеты диаграммы и кнопки logout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(chartLayout);
    mainLayout->addLayout(buttonLayout);
}


void DiagrammPonds::logout()
{
    this->close();
}

