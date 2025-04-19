#include "gistogrammtotalpurchase.h"
#include "ui_gistogrammtotalpurchase.h"
#include <QDebug>

GistogrammTotalPurchase::GistogrammTotalPurchase(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GistogrammTotalPurchase)
{
    ui->setupUi(this);
    db = QSqlDatabase::database(); // Получение существующего соединения с базой данных
    if (!db.isValid()) {
        qDebug() << "Error: Failed to get database connection.";
        return;
    }

    drawGistogramm();
}

GistogrammTotalPurchase::~GistogrammTotalPurchase()
{
    delete ui;
}

void GistogrammTotalPurchase::drawGistogramm() {
    QSqlQuery query;
    query.exec("SELECT c.counterparty_name, SUM(s.weight * s.price_per_kilo) AS total_purchase_amount "
               "FROM sale s "
               "JOIN counterparty c ON s.counterparty_id = c.counterparty_id "
               "GROUP BY c.counterparty_name");

    QStringList categories; // Список категорий для оси X
    QBarSeries *series = new QBarSeries();

    while (query.next()) {
        QString counterpartyName = query.value(0).toString();
        qreal totalPurchaseAmount = query.value(1).toDouble();

        QBarSet *set = new QBarSet(counterpartyName);
        *set << totalPurchaseAmount;
        series->append(set);
        // Добавляем название покупателя в список категорий
        categories.append(counterpartyName);
    }

    QChart *chart = new QChart();
    chart->setTitleFont(QFont("Arial", 16)); // Устанавливаем шрифт и размер для заголовка
    chart->setTitle("Общий выкуп каждого покупателя"); // Устанавливаем новый заголовок
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Создание оси Y
    QValueAxis *axisY = new QValueAxis();

    // Добавление осей на график
    chart->addAxis(axisY, Qt::AlignLeft);

    // Добавление серии на график
    chart->addSeries(series);

    // Привязка серии к осям
    series->attachAxis(axisY);

    // Создание виджета для отображения графика
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Создание кнопки logout и установка стилей
    QPushButton *logoutButton = new QPushButton("Закрыть");
    logoutButton->setStyleSheet("background-color: lightgreen;"); // Установка цвета фона
    logoutButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed); // Установка политики размеров
    connect(logoutButton, &QPushButton::clicked, this, &GistogrammTotalPurchase::logout);

    // Макет для кнопки logout
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(logoutButton); // Добавление кнопки в макет
    buttonLayout->setContentsMargins(0, 10, 0, 0); // Установка отступов

    // Основной макет, содержащий макеты диаграммы и кнопки logout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(chartView);
    mainLayout->addLayout(buttonLayout);

    // Установка виджета с графиком в качестве центрального виджета для QDialog
    setLayout(mainLayout);
}

void GistogrammTotalPurchase::logout()
{
    this->close();
}
