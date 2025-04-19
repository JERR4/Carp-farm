#include "carpinpondform.h"
#include "ui_carpinpondform.h"
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>
#include <QDate>
#include <QIntValidator>
#include <QDateEdit>

CarpInPondForm::CarpInPondForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CarpInPondForm),
    carpinpond(nullptr)
{
    ui->setupUi(this);
    ui->dateEdit->setCalendarPopup(true); // Устанавливаем календарь для выбора даты
    ui->dateEdit->setDate(QDate::currentDate()); // Устанавливаем текущую дату
    connect(ui->next, &QPushButton::clicked, this, &CarpInPondForm::next);
    connect(ui->previous, &QPushButton::clicked, this, &CarpInPondForm::previous);
    connect(ui->add, &QPushButton::clicked, this, &CarpInPondForm::add);
    connect(ui->save, &QPushButton::clicked, this, &CarpInPondForm::save);
    connect(ui->cancel, &QPushButton::clicked, this, &CarpInPondForm::display);
    connect(ui->delete_2, &QPushButton::clicked, this, &CarpInPondForm::delete_this);
    connect(ui->carpsforuser, &QPushButton::clicked, this, &CarpInPondForm::showCarpInPondForm);
    connect(ui->logout, &QPushButton::clicked, this, &CarpInPondForm::logout);
    loadPonds();
    loadCarps();
    ui->dateEditMin->setCalendarPopup(true);
    ui->dateEditMax->setCalendarPopup(true);
    ui->dateEditMin->setDate(QDate(2000, 1, 1));
    ui->dateEditMax->setDate(QDate::currentDate());
    connect(ui->dateEditMin, &QDateEdit::dateChanged, this, &CarpInPondForm::on_dateEditMin_dateChanged);
    connect(ui->dateEditMax, &QDateEdit::dateChanged, this, &CarpInPondForm::on_dateEditMax_dateChanged);

    showCarpInPond();

    // Устанавливаем валидатор для поля Число карпов, разрешая только целые числа
    QIntValidator *validator = new QIntValidator(1, 999999, this);
    ui->leQuantity->setValidator(validator);

}

CarpInPondForm::~CarpInPondForm()
{
    delete ui;
}

void CarpInPondForm::showCarpInPond() {

    db = QSqlDatabase::database();
    if (!db.isValid()) {
        qDebug() << "Ошибка: не удалось установить соединение с базой данных.";
        return;
    }

    QSqlQuery query;
    QString queryString = "SELECT * FROM carp_in_pond";
    QString condition = "";

    if (ui->dateEditMin->date() != QDate(2000, 1, 1) || ui->dateEditMax->date() != QDate::currentDate()) {
        condition = " WHERE start_date BETWEEN :minDate AND :maxDate";
    }

    queryString += condition + " ORDER BY carp_in_pond_id";
    query.prepare(queryString);

    if (!condition.isEmpty()) {
        QString minDateFormatted = ui->dateEditMin->date().toString("yyyy-MM-dd");
        QString maxDateFormatted = ui->dateEditMax->date().toString("yyyy-MM-dd");
        query.bindValue(":minDate", minDateFormatted);
        query.bindValue(":maxDate", maxDateFormatted);
        currentCarpInPondIndex = 0;
    }

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        return;
    }

    carpInPonds.clear();
    while (query.next()) {
        CarpInPond carpInPond;
        carpInPond.carp_in_pond_id = query.value("carp_in_pond_id").toInt();
        carpInPond.start_date = query.value("start_date").toString();
        carpInPond.pond_id = query.value("pond_id").toInt();
        carpInPond.carp_id = query.value("carp_id").toInt();
        carpInPond.quantity = query.value("quantity").toString();
        carpInPonds.append(carpInPond);
    }
    if (carpInPonds.size() == 0) {
        QMessageBox::critical(this, "Ошибка", "Ничего не найдено");
        ui->dateEditMin->setDate(QDate(2000, 1, 1)); // Устанавливаем начальную дату 2000-01-01
        ui->dateEditMax->setDate(QDate::currentDate());
    }
    display();
}

void CarpInPondForm::next() {
    if (currentCarpInPondIndex < carpInPonds.size() - 1) {
        ++currentCarpInPondIndex;
        display();
    }
}

void CarpInPondForm::previous() {
    if (currentCarpInPondIndex > 0) {
        --currentCarpInPondIndex;
        display();
    }
}

void CarpInPondForm::add() {
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

    ui->dateEdit->setDate(QDate::currentDate());
    ui->leQuantity->clear();

    ui->comboBoxPond->setCurrentIndex(-1);
    ui->comboBoxCarp->setCurrentIndex(-1);
    currentCarpInPondIndex = -1;
}

void CarpInPondForm::display() {
    if (currentCarpInPondIndex == -1)
        currentCarpInPondIndex = 0;
    ui->dateEditMin->show();
    ui->dateEditMax->show();
    ui->cancel->hide();
    ui->label_4->hide();
    ui->next->show();
    ui->logout->show();
    ui->previous->show();
    ui->add->show();
    ui->delete_2->show();
    ui->label_3->show();
    ui->lbMinDate->show();
    ui->lbMaxDate->show();

    const CarpInPond& currentCarpInPond = carpInPonds[currentCarpInPondIndex];

    ui->dateEdit->setDate(QDate::fromString(currentCarpInPond.start_date, "yyyy-MM-dd"));
    ui->leQuantity->setText(currentCarpInPond.quantity);

    QString pondName;
    for (const auto& pond_id : ponds) {
        if (pond_id.first == currentCarpInPond.pond_id) {
            pondName = pond_id.second;
            break;
        }
    }
    ui->comboBoxPond->setCurrentText(pondName);

    QString carpSpecies;
    for (const auto& carp_id : carps) {
        if (carp_id.first == currentCarpInPond.carp_id) {
            carpSpecies = carp_id.second;
            break;
        }
    }
    ui->comboBoxCarp->setCurrentText(carpSpecies);
}

void CarpInPondForm::save() {
    QString errorMessage;

    // Проверка на пустые поля
    if (ui->dateEdit->date().isNull()) {
        errorMessage += "Поле 'Дата' не может быть пустым.\n";
    }
    if (ui->comboBoxPond->currentText().isEmpty()) {
        errorMessage += "Поле 'Пруд' не может быть пустым.\n";
    }
    if (ui->comboBoxCarp->currentText().isEmpty()) {
        errorMessage += "Поле 'Карп' не может быть пустым.\n";
    }
    if (ui->leQuantity->text().isEmpty()) {
        errorMessage += "Поле 'Количество' не может быть пустым.\n";
    }

    // Если есть пустые поля, выводим сообщение об ошибке и прерываем операцию
    if (!errorMessage.isEmpty()) {
        QMessageBox::critical(this, "Ошибка в полях", errorMessage);
        if (currentCarpInPondIndex < 0)
            add();
        return;
    }

    if(currentCarpInPondIndex >= 0) {
        currentCarpInPondId = carpInPonds[currentCarpInPondIndex].carp_in_pond_id;
    }
    else
        currentCarpInPondId = -1;

    CarpInPond saveCarpInPond;

    saveCarpInPond.start_date = ui->dateEdit->date().toString("yyyy-MM-dd");
    saveCarpInPond.quantity = ui->leQuantity->text();
    saveCarpInPond.pond_id = ui->comboBoxPond->currentData().toInt();
    saveCarpInPond.carp_id = ui->comboBoxCarp->currentData().toInt();

    ui->dateEditMin->setDate(QDate(2000, 1, 1)); // Устанавливаем начальную дату 2000-01-01
    ui->dateEditMax->setDate(QDate::currentDate());

    int tempIndex = currentCarpInPondIndex;
    showCarpInPond();
    if (tempIndex < 0)
        currentCarpInPondIndex = -1;

    int duplicateIndex = -1;
    for (int i = 0; i < carpInPonds.size(); ++i) {
        const auto& carpInPond = carpInPonds[i];
        if (carpInPond.start_date == saveCarpInPond.start_date &&
            carpInPond.quantity == saveCarpInPond.quantity &&
            carpInPond.pond_id == saveCarpInPond.pond_id &&
            carpInPond.carp_id == saveCarpInPond.carp_id &&
            carpInPond.carp_id != currentCarpInPondId) {
            duplicateIndex = i;
            break;
        }
        if (carpInPond.carp_in_pond_id == currentCarpInPondId) {
            currentCarpInPondIndex = i;
        }
    }

    if (duplicateIndex < 0) {
        if (currentCarpInPondIndex < 0) {
            QSqlQuery query;
            QString insertQuery = QString("INSERT INTO carp_in_pond(start_date, pond_id, carp_id, quantity) "
                                          "VALUES ('%1', %2, %3, '%4') RETURNING carp_in_pond_id")
                                      .arg(saveCarpInPond.start_date)
                                      .arg(saveCarpInPond.pond_id)
                                      .arg(saveCarpInPond.carp_id)
                                      .arg(saveCarpInPond.quantity);
            if (!query.exec(insertQuery)) {
                QMessageBox::critical(this, "Ошибка в полях", "Ошибка в полях. Запись не добавлена\n" + query.lastError().text());
                add();
                return;
            } else {
                if (query.next()) {
                    saveCarpInPond.carp_in_pond_id = query.value(0).toInt();
                    carpInPonds.append(saveCarpInPond);
                    QMessageBox::information(this, "Успех", "Запись добавлена");
                }
                else {
                    QMessageBox::critical(this, "Ошибка", "Не удалось получить ID новой записи");
                    return;
                }
            }
            currentCarpInPondIndex = carpInPonds.size() - 1;
        }
        else
        {
            QSqlQuery query;
            QString updateQuery = QString("UPDATE carp_in_pond "
                                          "SET start_date = '%1', "
                                          "pond_id = %2, "
                                          "carp_id = %3, "
                                          "quantity = '%4' "
                                          "WHERE carp_in_pond_id = %5;")
                                      .arg(saveCarpInPond.start_date)
                                      .arg(saveCarpInPond.pond_id)
                                      .arg(saveCarpInPond.carp_id)
                                      .arg(saveCarpInPond.quantity)
                                      .arg(currentCarpInPondId);
            if (!query.exec(updateQuery)) {
                QMessageBox::critical(this, "Ошибка в полях", "Ошибка в полях. Запись не изменена\n" + query.lastError().text());
                return;
            } else {
                QMessageBox::information(this, "Успех", "Запись изменена");
            }
            carpInPonds[currentCarpInPondIndex].start_date = saveCarpInPond.start_date;
            carpInPonds[currentCarpInPondIndex].pond_id = saveCarpInPond.pond_id;
            carpInPonds[currentCarpInPondIndex].carp_id = saveCarpInPond.carp_id;
            carpInPonds[currentCarpInPondIndex].quantity = saveCarpInPond.quantity;
        }
    }
    else
    {
        QMessageBox::information(this, "Отмена", "Такая запись уже существует");
        currentCarpInPondIndex = duplicateIndex;
    }
    showCarpInPond();
}

void CarpInPondForm::delete_this() {
    currentCarpInPondId = carpInPonds[currentCarpInPondIndex].carp_in_pond_id;
    QSqlQuery query;
    QString deleteQuery = QString("DELETE FROM carp_in_pond "
                                  "WHERE carp_in_pond_id = %1;")
                              .arg(currentCarpInPondId);
    if (!query.exec(deleteQuery)) {
        qDebug() << "Ошибка при удалении записи:" << query.lastError().text();
        QMessageBox::information(this, "Ошибка", "Не удалось удалить запись!");
    } else {
        for (auto it = carpInPonds.begin(); it != carpInPonds.end(); ++it) {
            if ((*it).carp_in_pond_id == currentCarpInPondId) {
                it = carpInPonds.erase(it);
                break;
            }
        }

        ui->dateEdit->setDate(QDate::currentDate());
        ui->leQuantity->clear();
        ui->comboBoxPond->setCurrentIndex(-1);
        ui->comboBoxCarp->setCurrentIndex(-1);

        if (currentCarpInPondIndex > 0) {
            --currentCarpInPondIndex;
        } else if (!carpInPonds.isEmpty()) {
            ++currentCarpInPondIndex;
        }

        display();

        QMessageBox::information(this, "Успех", "Запись удалена!");
    }
}

void CarpInPondForm::loadPonds() {
    ponds.clear();
    ui->comboBoxPond->clear();

    QSqlQuery query("SELECT * FROM pond");
    while (query.next()) {
        int id = query.value("pond_id").toInt();
        QString name = query.value("pond_name").toString();
        ponds.append(qMakePair(id, name));
        ui->comboBoxPond->addItem(name, id);
    }
}

void CarpInPondForm::loadCarps() {
    carps.clear();
    ui->comboBoxCarp->clear();

    QSqlQuery query("SELECT * FROM carp");
    while (query.next()) {
        int id = query.value("carp_id").toInt();
        QString name = query.value("species").toString() + " — " + query.value("carp_type").toString();
        carps.append(qMakePair(id, name));
        ui->comboBoxCarp->addItem(name, id);
    }
}

void CarpInPondForm::on_dateEditMin_dateChanged(const QDate &date) {
    if (ui->dateEditMax->date() < date) {
        ui->dateEditMax->setDate(date);
    }
    showCarpInPond();
}

void CarpInPondForm::on_dateEditMax_dateChanged(const QDate &date) {
    if (ui->dateEditMin->date() > date) {
        ui->dateEditMin->setDate(date);
    }
    showCarpInPond();
}

void CarpInPondForm::showCarpInPondForm(){
    if (!carpinpond)
    {
        carpinpond = new CarpInPondForManager();
        connect(carpinpond, &CarpInPondForManager::finished, this, &CarpInPondForm::CarpInPondClosed);
    }
    carpinpond->exec();
}

void CarpInPondForm::CarpInPondClosed()
{
    // Удаляем виджет
    delete carpinpond;
    carpinpond = nullptr;
}

void CarpInPondForm::logout()
{
    this->close();
}
