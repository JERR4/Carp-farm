#include "pondform.h"
#include "ui_pondform.h"
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>
#include <QIntValidator>

PondForm::PondForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PondForm),
    pondforuser(nullptr)
{
    ui->setupUi(this);
    connect(ui->next, &QPushButton::clicked, this, &PondForm::next);
    connect(ui->previous, &QPushButton::clicked, this, &PondForm::previous);
    connect(ui->add, &QPushButton::clicked, this, &PondForm::add);
    connect(ui->save, &QPushButton::clicked, this, &PondForm::save);
    connect(ui->cancel, &QPushButton::clicked, this, &PondForm::display);
    connect(ui->delete_2, &QPushButton::clicked, this, &PondForm::delete_this);
    connect(ui->lineEditSearch, &QLineEdit::textChanged, this, &PondForm::searchPond);
    connect(ui->logout, &QPushButton::clicked, this, &PondForm::logout);
    connect(ui->pondsform, &QPushButton::clicked, this, &PondForm::ShowPondReport);
    showPond();
    loadPondPurposes();
    QIntValidator *validator = new QIntValidator(1, 999999, this);
    ui->leVolume->setValidator(validator);
}

PondForm::~PondForm()
{
    delete ui;
}

void PondForm::showPond() {
    db = QSqlDatabase::database();
    if (!db.isValid()) {
        qDebug() << "Ошибка: не удалось установить соединение с базой данных.";
        return;
    }

    QSqlQuery query;
    QString queryString = "SELECT * FROM pond";
    if (!searchFilter.isEmpty()) {
        queryString += " WHERE pond_name LIKE '" + searchFilter + "%'";
        currentPondIndex = 0;
    }
    queryString += " ORDER BY pond_id";
    if (!query.exec(queryString)) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        return;
    }

    ponds.clear();
    while (query.next()) {
        Pond pond;
        pond.pond_id = query.value("pond_id").toInt();
        pond.pond_name = query.value("pond_name").toString();
        pond.purpose = query.value("purpose").toString();
        pond.volume = query.value("volume").toString();
        ponds.append(pond);
    }
    if (ponds.size() == 0) {
        QMessageBox::critical(this, "Ошибка", "Ничего не найдено");
        ui->lineEditSearch->clear();
    }
    display();
}

void PondForm::next() {
    if (currentPondIndex < ponds.size() - 1) {
        ++currentPondIndex;
        display();
    }
}

void PondForm::previous() {
    if (currentPondIndex > 0) {
        --currentPondIndex;
        display();
    }
}

void PondForm::add() {
    ui->next->hide();
    ui->previous->hide();
    ui->add->hide();
    ui->logout->hide();
    ui->delete_2->hide();
    ui->label_3->hide();
    ui->cancel->show();
    ui->label_4->show();
    ui->lineEditSearch->hide();
    ui->lePondName->clear();
    ui->comboBoxPurpose->setCurrentIndex(-1);
    ui->leVolume->clear();
    currentPondIndex = -1;
}

void PondForm::display() {
    if (currentPondIndex == -1)
        currentPondIndex = 0;
    ui->lineEditSearch->show();
    ui->cancel->hide();
    ui->label_4->hide();
    ui->next->show();
    ui->logout->show();
    ui->previous->show();
    ui->add->show();
    ui->delete_2->show();
    ui->label_3->show();

    const Pond& currentPond = ponds[currentPondIndex];

    ui->lePondName->setText(currentPond.pond_name);
    ui->comboBoxPurpose->setCurrentText(currentPond.purpose);
    ui->leVolume->setText(currentPond.volume);
}

void PondForm::save() {
    QString errorMessage;

    // Проверка на пустые поля
    if (ui->lePondName->text().isEmpty()) {
        errorMessage += "Поле 'Название пруда' не может быть пустым.\n";
    }
    if (ui->comboBoxPurpose->currentText().isEmpty()) {
        errorMessage += "Поле 'Цель пруда' не может быть пустым.\n";
    }

    // Если есть пустые поля, выводим сообщение об ошибке и прерываем операцию
    if (!errorMessage.isEmpty()) {
        QMessageBox::critical(this, "Ошибка в полях", errorMessage);
        if (currentPondIndex < 0)
            add();
        return;
    }

    if(currentPondIndex >= 0) {
        currentPondId = ponds[currentPondIndex].pond_id;
    }
    else
        currentPondId = -1;

    Pond savePond;

    savePond.pond_name = ui->lePondName->text();
    savePond.purpose = ui->comboBoxPurpose->currentText();
    savePond.volume = ui->leVolume->text();

    int tempIndex = currentPondIndex;
    ui->lineEditSearch->clear();
    showPond();
    if (tempIndex < 0)
        currentPondIndex = -1;

    int duplicateIndex = -1;
    for (int i = 0; i < ponds.size(); ++i) {
        const auto& pond = ponds[i];
        if (pond.pond_name == savePond.pond_name && pond.pond_id != currentPondId) {
            duplicateIndex = i;
            break;
        }
        if (pond.pond_id == currentPondId) {
            currentPondIndex = i;
        }
    }

    if (duplicateIndex < 0 && currentPondIndex < 0) {
        QSqlQuery query;
        QString insertQuery = QString("INSERT INTO pond(pond_name, purpose, volume) "
                                      "VALUES ('%1', '%2', %3) RETURNING pond_id")
                                  .arg(savePond.pond_name)
                                  .arg(savePond.purpose)
                                  .arg(savePond.volume);
        if (!query.exec(insertQuery)) {
            QMessageBox::critical(this, "Ошибка в полях", "Ошибка в полях. Запись не добавлена");
            add();
            return;
        } else {
            if (query.next()) {
                savePond.pond_id = query.value(0).toInt();
                ponds.append(savePond);
                QMessageBox::information(this, "Успех", "Запись добавлена");
            }
            else {
                QMessageBox::critical(this, "Ошибка", "Не удалось получить ID новой записи");
                return;
            }
        }
            currentPondIndex = ponds.size() - 1;
    }
    else if (duplicateIndex < 0 && currentPondIndex >= 0)
    {
        QSqlQuery query;
        QString updateQuery = QString("UPDATE pond "
                                      "SET pond_name = '%1', "
                                      "purpose = '%2', "
                                      "volume = %3 "
                                      "WHERE pond_id = %4;")
                                  .arg(savePond.pond_name)
                                  .arg(savePond.purpose)
                                  .arg(savePond.volume)
                                  .arg(currentPondId);
        if (!query.exec(updateQuery)) {
            QMessageBox::critical(this, "Ошибка в полях", "Ошибка в полях. Запись не изменена\n" + query.lastError().text());
            return;
        } else {
            QMessageBox::information(this, "Успех", "Запись изменена");
        }
        ponds[currentPondIndex].pond_name = savePond.pond_name;
        ponds[currentPondIndex].purpose = savePond.purpose;
        ponds[currentPondIndex].volume = savePond.volume;
    }
    else
    {
        QMessageBox::information(this, "Отмена", "Такая запись уже существует");
        currentPondIndex = duplicateIndex;
    }
    showPond();
}

void PondForm::delete_this() {
    currentPondId = ponds[currentPondIndex].pond_id;
    QSqlQuery query;
    QString deleteQuery = QString("DELETE FROM pond "
                                  "WHERE pond_id = %1;")
                              .arg(currentPondId);
    if (!query.exec(deleteQuery)) {
        qDebug() << "Ошибка при удалении записи:" << query.lastError().text();
        QMessageBox::information(this, "Ошибка", "Не удалось удалить запись!");
    } else {
        for (auto it = ponds.begin(); it != ponds.end(); ++it) {
            if ((*it).pond_id == currentPondId) {
                it = ponds.erase(it);
                break;
            }
        }

        ui->lePondName->clear();
        ui->comboBoxPurpose->setCurrentIndex(-1);
        ui->leVolume->clear();

        if (currentPondIndex > 0) {
            --currentPondIndex;
        } else if (!ponds.isEmpty()) {
            ++currentPondIndex;
        }

        display();

        QMessageBox::information(this, "Успех", "Запись удалена!");
    }
}

void PondForm::loadPondPurposes() {
    pondPurposes.clear();
    ui->comboBoxPurpose->clear();

    // Add the specified pond purposes
    pondPurposes << "Мальковый" << "Нагульный" << "Нерестовый" << "Садок";

    ui->comboBoxPurpose->addItems(pondPurposes);
}

void PondForm::searchPond(const QString &text) {
    searchFilter = text;
    showPond();
}

void PondForm::ShowPondReport() {
    if (!pondforuser)
    {
        pondforuser = new pondsforuser();
        // Устанавливаем соединение для обработки сигнала о закрытии виджета
        connect(pondforuser, &pondsforuser::finished, this, &PondForm::closePondReport);
    }
    pondforuser->exec();
}

void PondForm::closePondReport()
{
    // Удаляем виджет карпов
    delete pondforuser;
    pondforuser = nullptr;
}

void PondForm::logout()
{
    this->close();
}
