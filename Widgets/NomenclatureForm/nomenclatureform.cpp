#include "nomenclatureform.h"
#include "ui_nomenclatureform.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QRegularExpressionValidator>

NomenclatureForm::NomenclatureForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NomenclatureForm),
    nomenclatureInStockForm(nullptr)
{
    ui->setupUi(this);
    connect(ui->next, &QPushButton::clicked, this, &NomenclatureForm::next);
    connect(ui->previous, &QPushButton::clicked, this, &NomenclatureForm::previous);
    connect(ui->add, &QPushButton::clicked, this, &NomenclatureForm::add);
    connect(ui->save, &QPushButton::clicked, this, &NomenclatureForm::save);
    connect(ui->cancel, &QPushButton::clicked, this, &NomenclatureForm::display);
    connect(ui->delete_2, &QPushButton::clicked, this, &NomenclatureForm::deleteThis);
    connect(ui->lineEditSearch, &QLineEdit::textChanged, this, &NomenclatureForm::searchNomenclature);
    connect(ui->nomenclature_in_stock, &QPushButton::clicked, this, &NomenclatureForm::showNomenclatureInStock);
    connect(ui->logout, &QPushButton::clicked, this, &NomenclatureForm::logout);
    showNomenclature();
    loadNomenclatureTypes();
}

NomenclatureForm::~NomenclatureForm()
{
    delete ui;
}

void NomenclatureForm::showNomenclature() {
    db = QSqlDatabase::database();
    if (!db.isValid()) {
        qDebug() << "Ошибка: не удалось установить соединение с базой данных.";
        return;
    }

    QSqlQuery query;
    QString queryString = "SELECT * FROM nomenclature";
    if (!searchFilter.isEmpty()) {
        queryString += " WHERE nomenclature_name LIKE '" + searchFilter + "%'";
        currentNomenclatureIndex = 0;
    }
    queryString += " ORDER BY nomenclature_id";
    if (!query.exec(queryString)) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        return;
    }

    nomenclatures.clear();
    while (query.next()) {
        Nomenclature nomenclature;
        nomenclature.nomenclature_id = query.value("nomenclature_id").toInt();
        nomenclature.nomenclature_name = query.value("nomenclature_name").toString();
        nomenclature.nomenclature_type = query.value("nomenclature_type").toString();
        nomenclatures.append(nomenclature);
    }
    if (nomenclatures.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Ничего не найдено");
        ui->lineEditSearch->clear();
    }
    display();
}

void NomenclatureForm::next() {
    if (currentNomenclatureIndex < nomenclatures.size() - 1) {
        ++currentNomenclatureIndex;
        display();
    }
}

void NomenclatureForm::previous() {
    if (currentNomenclatureIndex > 0) {
        --currentNomenclatureIndex;
        display();
    }
}

void NomenclatureForm::add() {
    ui->lineEditSearch->hide();
    ui->next->hide();
    ui->previous->hide();
    ui->add->hide();
    ui->logout->hide();
    ui->delete_2->hide();
    ui->label_3->hide();
    ui->cancel->show();
    ui->label_4->show();

    ui->leName->clear();
    ui->comboBoxType->setCurrentIndex(-1);
    currentNomenclatureIndex = -1;
}

void NomenclatureForm::display() {
    if (currentNomenclatureIndex == -1)
        currentNomenclatureIndex = 0;
    ui->lineEditSearch->show();
    ui->cancel->hide();
    ui->label_4->hide();
    ui->next->show();
    ui->logout->show();
    ui->previous->show();
    ui->add->show();
    ui->delete_2->show();
    ui->label_3->show();

    const Nomenclature& currentNomenclature = nomenclatures[currentNomenclatureIndex];

    ui->leName->setText(currentNomenclature.nomenclature_name);
    ui->comboBoxType->setCurrentText(currentNomenclature.nomenclature_type);
}

void NomenclatureForm::save() {
    QString errorMessage;
    // Проверка на пустые поля
    if (ui->leName->text().isEmpty()) {
        errorMessage += "Поле 'Имя контрагента' не может быть пустым.\n";
    }
    if (ui->comboBoxType->currentText().isEmpty()) {
        errorMessage += "Поле 'Тип контрагента' не может быть пустым.\n";
    }

    // Если есть пустые поля, выводим сообщение об ошибке и прерываем операцию
    if (!errorMessage.isEmpty()) {
        QMessageBox::critical(this, "Ошибка в полях", errorMessage);
        if (currentNomenclatureIndex < 0)
            add();
        return;
    }

    if(currentNomenclatureIndex >= 0) {
        currentNomenclatureId = nomenclatures[currentNomenclatureIndex].nomenclature_id;
    }
    else
        currentNomenclatureId = -1;

    Nomenclature saveNomenclature;

    saveNomenclature.nomenclature_name = ui->leName->text();
    saveNomenclature.nomenclature_type = ui->comboBoxType->currentText();

    int tempIndex = currentNomenclatureIndex;
    ui->lineEditSearch->clear();
    showNomenclature();
    if (tempIndex < 0)
        currentNomenclatureIndex = -1;

    int duplicateIndex = -1;
    for (int i = 0; i < nomenclatures.size(); ++i) {
        const auto& nomenclature = nomenclatures[i];
        if (nomenclature.nomenclature_name == saveNomenclature.nomenclature_name && nomenclature.nomenclature_id != currentNomenclatureId) {
            duplicateIndex = i;
            break;
        }
        if (nomenclature.nomenclature_id == currentNomenclatureId) {
            currentNomenclatureIndex = i;
        }
    }

    if (duplicateIndex < 0 && currentNomenclatureIndex < 0) {
        QSqlQuery query;
        QString insertQuery = QString("INSERT INTO nomenclature(nomenclature_name, nomenclature_type) "
                                      "VALUES ('%1', '%2') RETURNING nomenclature_id")
                                  .arg(saveNomenclature.nomenclature_name)
                                  .arg(saveNomenclature.nomenclature_type);
        if (!query.exec(insertQuery)) {
            QMessageBox::critical(this, "Ошибка в полях", "Ошибка в полях. Запись не добавлена");
            add();
            return;
        } else {
            if (query.next()) {
                saveNomenclature.nomenclature_id = query.value(0).toInt();
                nomenclatures.append(saveNomenclature);
                QMessageBox::information(this, "Успех", "Запись добавлена");
            }
            else {
                QMessageBox::critical(this, "Ошибка", "Не удалось получить ID новой записи");
                return;
            }
        }
        currentNomenclatureIndex = nomenclatures.size() - 1;
    }
    else if (duplicateIndex < 0 && currentNomenclatureIndex >= 0)
    {
        QSqlQuery query;
        QString updateQuery = QString("UPDATE nomenclature "
                                      "SET nomenclature_name = '%1', "
                                      "nomenclature_type = '%2' "
                                      "WHERE nomenclature_id = %3;")
                                  .arg(saveNomenclature.nomenclature_name)
                                  .arg(saveNomenclature.nomenclature_type)
                                  .arg(currentNomenclatureId);
        if (!query.exec(updateQuery)) {
            QMessageBox::critical(this, "Ошибка в полях", "Ошибка в полях. Запись не изменена\n" + query.lastError().text());
            return;
        } else {
            QMessageBox::information(this, "Успех", "Запись изменена");
        }
        //qDebug() << "Ошибка выполнения запроса:" << currentNomenclatureIndex;
        nomenclatures[currentNomenclatureIndex].nomenclature_name = saveNomenclature.nomenclature_name;
        nomenclatures[currentNomenclatureIndex].nomenclature_type = saveNomenclature.nomenclature_type;
    }
    else
    {
        QMessageBox::information(this, "Отмена", "Такая запись с таким названием уже существует");
        currentNomenclatureIndex = duplicateIndex;
    }
    showNomenclature();
}

void NomenclatureForm::deleteThis() {
    currentNomenclatureId = nomenclatures[currentNomenclatureIndex].nomenclature_id;
    QSqlQuery query;
    QString deleteQuery = QString("DELETE FROM nomenclature "
                                  "WHERE nomenclature_id = %1;")
                              .arg(currentNomenclatureId);
    if (!query.exec(deleteQuery)) {
        qDebug() << "Ошибка при удалении записи:" << query.lastError().text();
        QMessageBox::information(this, "Ошибка", "Не удалось удалить запись!");
    } else {
        for (auto it = nomenclatures.begin(); it != nomenclatures.end(); ++it) {
            if ((*it).nomenclature_id == currentNomenclatureId) {
                it = nomenclatures.erase(it);
                break;
            }
        }

        ui->leName->clear();
        ui->comboBoxType->setCurrentIndex(-1);

        if (currentNomenclatureIndex > 0) {
            --currentNomenclatureIndex;
        } else if (!nomenclatures.isEmpty()) {
            ++currentNomenclatureIndex;
        }

        display();

        QMessageBox::information(this, "Успех", "Запись удалена!");
    }
}

void NomenclatureForm::loadNomenclatureTypes() {
    nomenclatureTypes.clear();
    ui->comboBoxType->clear();

    nomenclatureTypes << "Корм" << "Медикаменты";
    ui->comboBoxType->addItems(nomenclatureTypes);
}

void NomenclatureForm::searchNomenclature(const QString &text) {
    searchFilter = text;
    showNomenclature();
}

void NomenclatureForm::showNomenclatureInStock(){
    if (!nomenclatureInStockForm)
    {
        nomenclatureInStockForm = new NomenclatureInStock();
        connect( nomenclatureInStockForm, &NomenclatureInStock::finished, this, &NomenclatureForm::NomenclatureInStockClosed);
    }
    nomenclatureInStockForm->exec();

}

void NomenclatureForm::NomenclatureInStockClosed()
{
    // Удаляем виджет
    delete nomenclatureInStockForm;
    nomenclatureInStockForm = nullptr;
}

void NomenclatureForm::logout()
{
    this->close();
}
