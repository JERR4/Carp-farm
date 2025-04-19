#include "feedingform.h"
#include "ui_feedingform.h"
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>
#include <QDate>
#include <QIntValidator>

FeedingForm::FeedingForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FeedingForm),
    feedingReport(nullptr)
{
    ui->setupUi(this);
    ui->dateEdit->setCalendarPopup(true); // Устанавливаем календарь для выбора даты
    ui->dateEdit->setDate(QDate::currentDate()); // Устанавливаем текущую дату
    connect(ui->next, &QPushButton::clicked, this, &FeedingForm::next);
    connect(ui->previous, &QPushButton::clicked, this, &FeedingForm::previous);
    connect(ui->add, &QPushButton::clicked, this, &FeedingForm::add);
    connect(ui->save, &QPushButton::clicked, this, &FeedingForm::save);
    connect(ui->cancel, &QPushButton::clicked, this, &FeedingForm::display);
    connect(ui->delete_2, &QPushButton::clicked, this, &FeedingForm::delete_this);
    connect(ui->dateEditMin, &QDateEdit::dateChanged, this, &FeedingForm::on_dateEditMin_dateChanged);
    connect(ui->dateEditMax, &QDateEdit::dateChanged, this, &FeedingForm::on_dateEditMax_dateChanged);
    connect(ui->feedingReport, &QPushButton::clicked, this, &FeedingForm::showFeedingReport);
    connect(ui->logout, &QPushButton::clicked, this, &FeedingForm::logout);
    loadPonds();
    loadNomenclature();
    loadEmployeeGroup();
    QIntValidator *validator = new QIntValidator(1, 999999, this);
    ui->leUsed->setValidator(validator);
    ui->dateEditMin->setCalendarPopup(true); // Устанавливаем календарь для выбора даты
    ui->dateEditMax->setCalendarPopup(true); // Устанавливаем календарь для выбора даты
    ui->dateEditMin->setDate(QDate(2000, 1, 1)); // Устанавливаем начальную дату 2000-01-01
    ui->dateEditMax->setDate(QDate::currentDate());
    showFeeding();
}

FeedingForm::~FeedingForm()
{
    delete ui;
}

void FeedingForm::showFeeding() {
    db = QSqlDatabase::database();
    if (!db.isValid()) {
        qDebug() << "Error: Failed to get database connection.";
        return;
    }

    QSqlQuery query;
    QString queryString = "SELECT * FROM feeding";
    QString condition = "";

    if (ui->dateEditMin->date() != QDate(2000, 1, 1) || ui->dateEditMax->date() != QDate::currentDate()) {
        condition = " WHERE feeding_date BETWEEN :minDate AND :maxDate";
    }

    queryString += condition + " ORDER BY feeding_id";
    query.prepare(queryString);

    if (!condition.isEmpty()) {
        QString minDateFormatted = ui->dateEditMin->date().toString("yyyy-MM-dd");
        QString maxDateFormatted = ui->dateEditMax->date().toString("yyyy-MM-dd");
        query.bindValue(":minDate", minDateFormatted);
        query.bindValue(":maxDate", maxDateFormatted);
        currentFeedingIndex = 0;
    }

    feedings.clear(); // Очищаем список перед загрузкой новых данных
    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        Feeding feed;
        feed.feeding_id = query.value("feeding_id").toInt();
        feed.feeding_date = query.value("feeding_date").toString();
        feed.pond_id = query.value("pond_id").toInt();
        feed.nomenclature_id = query.value("nomenclature_id").toInt();;
        feed.used_quantity = query.value("used_quantity").toString();
        feed.employee_group_id = query.value("employee_group_id").toInt();
        feedings.append(feed);
    }
    if (feedings.size() == 0) {
        QMessageBox::critical(this, "Ошибка", "Ничего не найдено");
        ui->dateEditMin->setDate(QDate(2000, 1, 1)); // Устанавливаем начальную дату 2000-01-01
        ui->dateEditMax->setDate(QDate::currentDate());
        showFeeding();
    }
    display();
}

void FeedingForm::next() {
    if (currentFeedingIndex < feedings.size() - 1) {
        ++currentFeedingIndex;
        display();
    }
}

void FeedingForm::previous() {
    if (currentFeedingIndex > 0) {
        --currentFeedingIndex;
        display();
    }
}

void FeedingForm::add() {
    ui->next->hide();
    ui->previous->hide();
    ui->add->hide();
    ui->logout->hide();
    ui->delete_2->hide();
    ui->label_3->hide();
    ui->cancel->show();
    ui->label_4->show();
    ui->dateEditMin->hide();
    ui->dateEditMax->hide();
    ui->lbMinDate->hide();
    ui->lbMaxDate->hide();
    ui->lbMinDate->hide();
    ui->lbMaxDate->hide();
    ui->dateEdit->setDate(QDate::currentDate());
    ui->leUsed->clear();
    QIntValidator *validator = new QIntValidator(1, 999999, this);
    ui->leUsed->setValidator(validator);
    ui->comboBoxPond->setCurrentIndex(-1);
    ui->comboBoxNomenclature->setCurrentIndex(-1);
    ui->comboBoxEmployeeGroup->setCurrentIndex(-1);
    currentFeedingIndex = -1;
}

void FeedingForm::display() {
    if (currentFeedingIndex == -1)
        currentFeedingIndex = 0;
    ui->cancel->hide();
    ui->label_4->hide();
    ui->next->show();
    ui->logout->show();
    ui->previous->show();
    ui->add->show();
    ui->delete_2->show();
    ui->label_3->show();
    ui->dateEditMin->show();
    ui->dateEditMax->show();
    ui->lbMinDate->show();
    ui->lbMaxDate->show();
    ui->lbMinDate->show();
    ui->lbMaxDate->show();
    const Feeding& currentFeeding = feedings[currentFeedingIndex];
    QDate date = QDate::fromString(currentFeeding.feeding_date, "yyyy-MM-dd");
    ui->dateEdit->setDate(date);
    ui->leUsed->setText(currentFeeding.used_quantity);
    QString pondName;
    for (const auto& pond_id : ponds) {
        if (pond_id.first == currentFeeding.pond_id) {
            pondName = pond_id.second;
            break;
        }
    }
    ui->comboBoxPond->setCurrentText(pondName);
    QString nomenclatureName;
    for (const auto& nomenclature_id : nomenclatures) {
        if (nomenclature_id.first == currentFeeding.nomenclature_id) {
            nomenclatureName = nomenclature_id.second;
            break;
        }
    }
    ui->comboBoxNomenclature->setCurrentText(nomenclatureName);
    int group_id = currentFeeding.employee_group_id;
    for (int i = 0; i < ui->comboBoxEmployeeGroup->count(); ++i) {
        if (ui->comboBoxEmployeeGroup->itemData(i).toInt() == group_id) {
            ui->comboBoxEmployeeGroup->setCurrentIndex(i);
            break;
        }
    }
}

void FeedingForm::save() {
    QString errorMessage;
    if (ui->dateEdit->text().isEmpty()) {
        errorMessage += "Поле 'Дата' не может быть пустым.\n";
    }
    if (ui->comboBoxPond->currentText().isEmpty()) {
        errorMessage += "Поле 'Пруд' не может быть пустым.\n";
    }
    if (ui->comboBoxNomenclature->currentText().isEmpty()) {
        errorMessage += "Поле 'Номенклатура' не может быть пустым.\n";
    }
    if (ui->leUsed->text().isEmpty()) {
        errorMessage += "Поле 'Использованное количество' не может быть пустым.\n";
    }
    if (ui->comboBoxEmployeeGroup->currentText().isEmpty()) {
        errorMessage += "Поле 'Группа сотрудников' не может быть пустым.\n";
    }

    if (!errorMessage.isEmpty()) {
        QMessageBox::critical(this, "Ошибка в полях", errorMessage);
        if (currentFeedingIndex < 0)
            add();
        return;
    }

    if(currentFeedingIndex >= 0) {
            currentFeedingId = feedings[currentFeedingIndex].feeding_id;
    }
    else
        currentFeedingId= -1;

    Feeding savefeed;
    savefeed.feeding_date  = ui->dateEdit->date().toString("yyyy-MM-dd");
    savefeed.used_quantity = ui->leUsed->text();
    savefeed.pond_id = ui->comboBoxPond->currentData().toInt();
    savefeed.nomenclature_id = ui->comboBoxNomenclature->currentData().toInt();
    savefeed.employee_group_id = ui->comboBoxEmployeeGroup->currentData().toInt();
    ui->dateEditMin->setDate(QDate(2000, 1, 1)); // Устанавливаем начальную дату 2000-01-01
    ui->dateEditMax->setDate(QDate::currentDate());

    int tempIndex = currentFeedingIndex;
    showFeeding();
    if (tempIndex < 0)
        currentFeedingIndex = -1;

    int duplicateIndex = -1;
    for (int i = 0; i < feedings.size(); ++i) {
        const auto& feed = feedings[i];
        if (feed.feeding_date == savefeed.feeding_date &&
            feed.used_quantity == savefeed.used_quantity &&
            feed.pond_id == savefeed.pond_id &&
            feed.nomenclature_id == savefeed.nomenclature_id &&
            feed.employee_group_id == savefeed.employee_group_id &&
            feed.feeding_id != currentFeedingId) {
            duplicateIndex = i;
            break;
        }
        if (feed.feeding_id == currentFeedingId) {
            currentFeedingIndex = i;
        }
    }

    if (duplicateIndex < 0) {
        if (currentFeedingIndex < 0) {
            QSqlQuery query;
            QString insertQuery = QString("insert into feeding(feeding_date,pond_id,nomenclature_id, used_quantity, employee_group_id) "
                                          "VALUES ('%1', '%2', '%3', '%4', '%5') RETURNING feeding_id")
                                      .arg(savefeed.feeding_date).arg(savefeed.pond_id).arg(savefeed.nomenclature_id).arg(savefeed.used_quantity)
                                      .arg(savefeed.employee_group_id);
            if (!query.exec(insertQuery)) {
                QMessageBox::critical(this, "Ошибка в полях", "Ошибка в полях. Запись не добавлена\n" + query.lastError().text());
                add();
                return;
            } else {
                if (query.next()) {
                    savefeed.feeding_id = query.value(0).toInt();
                    feedings.append(savefeed);
                    QMessageBox::information(this, "Успех", "Запись добавлена");
                }
                else {
                    QMessageBox::critical(this, "Ошибка", "Не удалось получить ID новой записи");
                    return;
                }
            }
            currentFeedingIndex = feedings.size() - 1;
        }
        else
        {
            QSqlQuery query;
            QString updateQuery = QString("UPDATE feeding "
                                          "SET feeding_date = '%1', "
                                          "pond_id = %2, "
                                          "nomenclature_id = %3, "
                                          "used_quantity = %4, "
                                          "employee_group_id = %5 "
                                          "WHERE feeding_id = %6;")
                                      .arg(savefeed.feeding_date)
                                      .arg(savefeed.pond_id)
                                      .arg(savefeed.nomenclature_id)
                                      .arg(savefeed.used_quantity)
                                      .arg(savefeed.employee_group_id)
                                      .arg(currentFeedingId);
            if (!query.exec(updateQuery)) {
                QMessageBox::critical(this, "Ошибка в полях", "Ошибка в полях. Запись не изменена\n" + query.lastError().text());
                return;
            } else {
                QMessageBox::information(this, "Успех", "Запись изменена");
            }
            feedings[currentFeedingIndex].feeding_date = savefeed.feeding_date;
            feedings[currentFeedingIndex].pond_id = savefeed.pond_id;
            feedings[currentFeedingIndex].nomenclature_id = savefeed.nomenclature_id;
            feedings[currentFeedingIndex].used_quantity = savefeed.used_quantity;
            feedings[currentFeedingIndex].employee_group_id = savefeed.employee_group_id;
        }
    }
    else
    {
        QMessageBox::information(this, "Отмена", "Такое кормление уже есть");
        currentFeedingIndex = duplicateIndex;
    }
    showFeeding();
}

void FeedingForm::delete_this() {
    currentFeedingId = feedings[currentFeedingIndex].feeding_id;
    QSqlQuery query;
    QString deleteQuery = QString("DELETE FROM feeding "
                                  "WHERE feeding_id = %1;")
                              .arg(currentFeedingId);
    if (!query.exec(deleteQuery)) {
        qDebug() << "Error deleting feeding:" << query.lastError().text();
        QMessageBox::information(this, "Ошибка", "Не получилось запись!");
    } else {
        for (auto it = feedings.begin(); it != feedings.end(); ++it) {
            if ((*it).feeding_id == currentFeedingId) {
                it = feedings.erase(it);
                break;
            }
        }

        ui->dateEdit->clear();
        ui->leUsed->clear();
        ui->comboBoxPond->setCurrentIndex(-1);
        ui->comboBoxNomenclature->setCurrentIndex(-1);
        ui->comboBoxEmployeeGroup->setCurrentIndex(-1);

        if (currentFeedingIndex > 0) {
            --currentFeedingIndex;
        } else if (!feedings.isEmpty()) {
            ++currentFeedingIndex;
        }

        display();

        QMessageBox::information(this, "Успех", "Запись удалена!");
    }
}

void FeedingForm::loadPonds() {
    ponds.clear();
    ui->comboBoxPond->clear();
    QSqlQuery query("SELECT * FROM pond ");
    while (query.next()) {
        int id = query.value("pond_id").toInt();
        QString name = query.value("pond_name").toString();
        ponds.append(qMakePair(id, name));
        ui->comboBoxPond->addItem(name, id);
    }
}

void FeedingForm::loadNomenclature() {
    nomenclatures.clear();
    ui->comboBoxNomenclature->clear();
    QSqlQuery query("SELECT * FROM nomenclature");
    while (query.next()) {
        int id = query.value("nomenclature_id").toInt();
        QString name = query.value("nomenclature_name").toString();
        nomenclatures.append(qMakePair(id, name));
        ui->comboBoxNomenclature->addItem(name, id);
    }
}

void FeedingForm::loadEmployeeGroup() {
    ui->comboBoxEmployeeGroup->clear();
    QSqlQuery query("SELECT * FROM employee_group where authority_limitation = 'Специалист по уходу за рыбой и водоёмом'");
    while (query.next()) {
        int group_id = query.value("employee_group_id").toInt();
        QString group_number = query.value("group_number").toString();
        ui->comboBoxEmployeeGroup->addItem(group_number, group_id);
    }
}

void FeedingForm::on_dateEditMin_dateChanged(const QDate &date) {
    // При изменении минимальной даты обновляем максимальную дату, чтобы она не была меньше
    if (ui->dateEditMax->date() < date) {
        ui->dateEditMax->setDate(date);
    }
    showFeeding();
}

void FeedingForm::on_dateEditMax_dateChanged(const QDate &date) {
    // При изменении максимальной даты обновляем минимальную дату, чтобы она не была больше
    if (ui->dateEditMin->date() > date) {
        ui->dateEditMin->setDate(date);
    }
    showFeeding();
}

void FeedingForm::showFeedingReport()
{
    if (!feedingReport)
    {
        feedingReport = new FeedingReport(); // Создаем экземпляр класса FeedingReport
        connect(feedingReport, &FeedingReport::finished, this, &FeedingForm::feedingReportClosed);
    }
    feedingReport->exec();
}

void FeedingForm::feedingReportClosed()
{
    delete feedingReport;
    feedingReport = nullptr;
}

void FeedingForm::logout()
{
    this->close();
}
