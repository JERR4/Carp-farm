#include "carpform.h"
#include "ui_carpform.h"
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>

CarpForm::CarpForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CarpForm),
    carpforuser(nullptr)
{
    ui->setupUi(this);
    connect(ui->next, &QPushButton::clicked, this, &CarpForm::next);
    connect(ui->previous, &QPushButton::clicked, this, &CarpForm::previous);
    connect(ui->add, &QPushButton::clicked, this, &CarpForm::add);
    connect(ui->save, &QPushButton::clicked, this, &CarpForm::save);
    connect(ui->cancel, &QPushButton::clicked, this, &CarpForm::display);
    connect(ui->delete_2, &QPushButton::clicked, this, &CarpForm::delete_this);
    connect(ui->lineEditSearch, &QLineEdit::textChanged, this, &CarpForm::searchSpecies);
    connect(ui->carpsforuser, &QPushButton::clicked, this, &CarpForm::showCarpForUser);
    connect(ui->logout, &QPushButton::clicked, this, &CarpForm::logout);
    showCarp();
    loadCarpTypes();
}

CarpForm::~CarpForm()
{
    delete ui;
}

void CarpForm::showCarp() {
    db = QSqlDatabase::database();
    if (!db.isValid()) {
        qDebug() << "Ошибка: не удалось установить соединение с базой данных.";
        return;
    }

    QSqlQuery query;
    QString queryString = "SELECT * FROM carp";
    if (!searchFilter.isEmpty()) {
        currentCarpIndex = 0;
        queryString += " WHERE species LIKE '" + searchFilter + "%'";
    }
    queryString += " ORDER BY carp_id";
    if (!query.exec(queryString)) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        return;
    }

    carps.clear();
    while (query.next()) {
        Carp carp;
        carp.carp_id = query.value("carp_id").toInt();
        carp.species = query.value("species").toString();
        carp.carp_type = query.value("carp_type").toString();
        carps.append(carp);
    }
    if (carps.size() == 0) {
        QMessageBox::critical(this, "Ошибка", "Ничего не найдено");
        ui->lineEditSearch->clear();
    }
    display();
}

void CarpForm::next() {
    if (currentCarpIndex < carps.size() - 1) {
        ++currentCarpIndex;
        display();
    }
}

void CarpForm::previous() {
    if (currentCarpIndex > 0) {
        --currentCarpIndex;
        display();
    }
}

void CarpForm::add() {
    ui->lineEditSearch->hide();
    ui->next->hide();
    ui->previous->hide();
    ui->add->hide();
    ui->logout->hide();
    ui->delete_2->hide();
    ui->label_3->hide();
    ui->cancel->show();
    ui->label_4->show();

    ui->leSpecies->clear();
    ui->comboBoxType->setCurrentIndex(-1);
    currentCarpIndex = -1;
}

void CarpForm::display() {
    if (currentCarpIndex == -1)
        currentCarpIndex = 0;
    ui->lineEditSearch->show();
    ui->cancel->hide();
    ui->label_4->hide();
    ui->next->show();
    ui->logout->show();
    ui->previous->show();
    ui->add->show();
    ui->delete_2->show();
    ui->label_3->show();

    const Carp& currentCarp = carps[currentCarpIndex];

    ui->leSpecies->setText(currentCarp.species);
    ui->comboBoxType->setCurrentText(currentCarp.carp_type);
}

void CarpForm::save() {
    QString errorMessage;

    // Проверка на пустые поля
    if (ui->leSpecies->text().isEmpty()) {
        errorMessage += "Поле 'Вид карпа' не может быть пустым.\n";
    }
    if (ui->comboBoxType->currentText().isEmpty()) {
        errorMessage += "Поле 'Тип карпа' не может быть пустым.\n";
    }

    // Если есть пустые поля, выводим сообщение об ошибке и прерываем операцию
    if (!errorMessage.isEmpty()) {
        QMessageBox::critical(this, "Ошибка в полях", errorMessage);
        if (currentCarpIndex < 0)
            add();
        return;
    }
    if(currentCarpIndex >= 0) {
        currentCarpId = carps[currentCarpIndex].carp_id;
    }
    else
        currentCarpId = -1;
    Carp saveCarp;

    saveCarp.species = ui->leSpecies->text();
    saveCarp.carp_type = ui->comboBoxType->currentText();

    int tempIndex = currentCarpIndex;
    ui->lineEditSearch->clear();
    showCarp();
    if (tempIndex < 0)
        currentCarpIndex = -1;

    int duplicateIndex = -1;
    for (int i = 0; i < carps.size(); ++i) {
        const auto& carp = carps[i];
        if (carp.species == saveCarp.species &&
            carp.carp_type == saveCarp.carp_type && carp.carp_id != currentCarpId) {
            duplicateIndex = i;
            break;
        }
        if (carp.carp_id == currentCarpId) {
            currentCarpIndex = i;
        }
    }
    if (duplicateIndex < 0) {
        if (currentCarpIndex < 0) {
            QSqlQuery query;
            QString insertQuery = QString("INSERT INTO carp(species, carp_type) "
                                          "VALUES ('%1', '%2') RETURNING carp_id")
                                      .arg(saveCarp.species)
                                      .arg(saveCarp.carp_type);
            if (!query.exec(insertQuery)) {
                QMessageBox::critical(this, "Ошибка в полях", "Ошибка в полях. Запись не добавлена");
                add();
                return;
            } else {
                if (query.next()) {
                    saveCarp.carp_id = query.value(0).toInt();
                    carps.append(saveCarp);
                    QMessageBox::information(this, "Успех", "Запись добавлена");
                }
                else {
                    QMessageBox::critical(this, "Ошибка", "Не удалось получить ID новой записи");
                    return;
                }
            }
            currentCarpIndex = carps.size() - 1;
        }
        else
        {
            QSqlQuery query;
            QString updateQuery = QString("UPDATE carp "
                                          "SET species = '%1', "
                                          "carp_type = '%2' "
                                          "WHERE carp_id = %3;")
                                      .arg(saveCarp.species)
                                      .arg(saveCarp.carp_type)
                                      .arg(currentCarpId);
            if (!query.exec(updateQuery)) {
                QMessageBox::critical(this, "Ошибка в полях", "Ошибка в полях. Запись не изменена\n" + query.lastError().text());
                return;
            } else {
                QMessageBox::information(this, "Успех", "Запись изменена");
            }
            qDebug() << "Ошибка выполнения запроса:" << currentCarpIndex;
            carps[currentCarpIndex].species = saveCarp.species;
            carps[currentCarpIndex].carp_type = saveCarp.carp_type;
        }
    }
    else
    {
        QMessageBox::information(this, "Отмена", "Такая запись уже существует");
        currentCarpIndex = duplicateIndex;
    }
    showCarp();
}

void CarpForm::delete_this() {
    currentCarpId = carps[currentCarpIndex].carp_id;
    QSqlQuery query;
    QString deleteQuery = QString("DELETE FROM carp "
                                  "WHERE carp_id = %1;")
                              .arg(currentCarpId);
    if (!query.exec(deleteQuery)) {
        qDebug() << "Ошибка при удалении записи:" << query.lastError().text();
        QMessageBox::information(this, "Ошибка", "Не удалось удалить запись!");
    } else {
        for (auto it = carps.begin(); it != carps.end(); ++it) {
            if ((*it).carp_id == currentCarpId) {
                it = carps.erase(it);
                break;
            }
        }

        ui->leSpecies->clear();
        ui->comboBoxType->setCurrentIndex(-1);

        if (currentCarpIndex > 0) {
            --currentCarpIndex;
        } else if (!carps.isEmpty()) {
            ++currentCarpIndex;
        }

        display();

        QMessageBox::information(this, "Успех", "Запись удалена!");
    }
}

void CarpForm::loadCarpTypes() {
    carpTypes.clear();
    ui->comboBoxType->clear();

    // Загружаем только виды, соответствующие поисковому запросу, если он задан
    if (!searchFilter.isEmpty()) {
        QSqlQuery query;
        QString queryString = "SELECT DISTINCT carp_type FROM carp WHERE species LIKE '" + searchFilter + "%'";
        if (!query.exec(queryString)) {
            qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
            return;
        }

        while (query.next()) {
            carpTypes.append(query.value(0).toString());
        }
    } else {
        // В противном случае загружаем все виды
        carpTypes << "Взрослый" << "Сеголеток" << "Малёк";
    }

    ui->comboBoxType->addItems(carpTypes);
}


void CarpForm::searchSpecies(const QString &text) {
    searchFilter = text;
    showCarp();
}

void CarpForm::showCarpForUser(){
    if (!carpforuser)
    {
        carpforuser = new CarpForManager();
        connect(carpforuser, &CarpForManager::finished, this, &CarpForm::CarpForUserClosed);
    }
    carpforuser->exec();
}

void CarpForm::CarpForUserClosed()
{
    // Удаляем виджет
    delete carpforuser;
    carpforuser = nullptr;
}


void CarpForm::logout()
{
    this->close();
}
