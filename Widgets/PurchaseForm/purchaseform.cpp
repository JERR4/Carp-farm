#include "purchaseform.h"
#include "ui_purchaseform.h"
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>
#include <QDate>
#include <QIntValidator>


PurchaseForm::PurchaseForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PurchaseForm),
    purchaseReportForm(nullptr)
{
    ui->setupUi(this);
    ui->dateEdit->setCalendarPopup(true); // Устанавливаем календарь для выбора даты
    ui->dateEdit->setDate(QDate::currentDate()); // Устанавливаем текущую дату
    connect(ui->next, &QPushButton::clicked, this, &PurchaseForm::next);
    connect(ui->previous, &QPushButton::clicked, this, &PurchaseForm::previous);
    connect(ui->add, &QPushButton::clicked, this, &PurchaseForm::add);
    connect(ui->save, &QPushButton::clicked, this, &PurchaseForm::save);
    connect(ui->cancel, &QPushButton::clicked, this, &PurchaseForm::display);
    connect(ui->delete_2, &QPushButton::clicked, this, &PurchaseForm::delete_this);
    connect(ui->logout, &QPushButton::clicked, this, &PurchaseForm::logout);
    connect(ui->leUnitPrice, &QLineEdit::textChanged, this, &PurchaseForm::setupValidators);
    connect(ui->purchaseReportButton, &QPushButton::clicked, this, &PurchaseForm::showPurchaseReport);
    ui->dateEditMin->setCalendarPopup(true);
    ui->dateEditMax->setCalendarPopup(true);
    ui->dateEditMin->setDate(QDate(2000, 1, 1));
    ui->dateEditMax->setDate(QDate::currentDate());
    connect(ui->dateEditMin, &QDateEdit::dateChanged, this, &PurchaseForm::on_dateEditMin_dateChanged);
    connect(ui->dateEditMax, &QDateEdit::dateChanged, this, &PurchaseForm::on_dateEditMax_dateChanged);
    loadNomenclatures();
    loadCounterparties();
    loadEmployees();

    showPurchase();
}

PurchaseForm::~PurchaseForm()
{
    delete ui;
}

void PurchaseForm::showPurchase() {
    db = QSqlDatabase::database();
    if (!db.isValid()) {
        qDebug() << "Ошибка: не удалось установить соединение с базой данных.";
        return;
    }

    QSqlQuery query;
    QString queryString = "SELECT * FROM purchase";
    QString condition = "";

    if (ui->dateEditMin->date() != QDate(2000, 1, 1) || ui->dateEditMax->date() != QDate::currentDate()) {
        condition = " WHERE purchase_date BETWEEN :minDate AND :maxDate";
    }

    queryString += condition + " ORDER BY purchase_id";
    query.prepare(queryString);

    if (!condition.isEmpty()) {
        QString minDateFormatted = ui->dateEditMin->date().toString("yyyy-MM-dd");
        QString maxDateFormatted = ui->dateEditMax->date().toString("yyyy-MM-dd");
        query.bindValue(":minDate", minDateFormatted);
        query.bindValue(":maxDate", maxDateFormatted);
        currentPurchaseIndex = 0;
    }

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        return;
    }

    // Очищаем список перед загрузкой новых данных
    purchases.clear();

    while (query.next()) {
        Purchase purchase;
        purchase.purchase_id = query.value("purchase_id").toInt();
        purchase.purchase_date = query.value("purchase_date").toString();
        purchase.nomenclature_id = query.value("nomenclature_id").toInt();
        purchase.counterparty_id = query.value("counterparty_id").toInt();
        purchase.employee_id = query.value("employee_id").toInt();
        purchase.nomenclature_quantity = query.value("nomenclature_quantity").toString();
        purchase.unit_price = query.value("unit_price").toString();
        purchases.append(purchase);
    }
    if (purchases.size() == 0) {
        QMessageBox::critical(this, "Ошибка", "Ничего не найдено");
        ui->dateEditMin->setDate(QDate(2000, 1, 1)); // Устанавливаем начальную дату 2000-01-01
        ui->dateEditMax->setDate(QDate::currentDate());
        showPurchase();
    }

    display();
}


void PurchaseForm::next() {
    if (currentPurchaseIndex < purchases.size() - 1) {
        ++currentPurchaseIndex;
        display();
    }
}

void PurchaseForm::previous() {
    if (currentPurchaseIndex > 0) {
        --currentPurchaseIndex;
        display();
    }
}

void PurchaseForm::add() {
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
    // Устанавливаем валидатор для поля Количество, разрешая только цифры
    QIntValidator *validator = new QIntValidator(1, 999999, this);
    ui->leQuantity->setValidator(validator);
    ui->leQuantity->clear();
    ui->leUnitPrice->clear();

    ui->comboBoxNomenclature->setCurrentIndex(-1);
    ui->comboBoxCounterparty->setCurrentIndex(-1);
    ui->comboBoxEmployee->setCurrentIndex(-1);
    currentPurchaseIndex = -1;
}


void PurchaseForm::display() {
    if (currentPurchaseIndex == -1)
        currentPurchaseIndex = 0;

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

    const Purchase& currentPurchase = purchases[currentPurchaseIndex];

    QDate date = QDate::fromString(currentPurchase.purchase_date, "yyyy-MM-dd");
    ui->dateEdit->setDate(date);
    ui->leQuantity->setText(currentPurchase.nomenclature_quantity);
    ui->leUnitPrice->setText(currentPurchase.unit_price);

    QString nomenclatureName;
    for (const auto& nom_id : nomenclatures) {
        if (nom_id.first == currentPurchase.nomenclature_id) {
            nomenclatureName = nom_id.second;
            break;
        }
    }
    ui->comboBoxNomenclature->setCurrentText(nomenclatureName);

    QString counterpartyName;
    for (const auto& cp_id : counterparties) {
        if (cp_id.first == currentPurchase.counterparty_id) {
            counterpartyName = cp_id.second;
            break;
        }
    }
    ui->comboBoxCounterparty->setCurrentText(counterpartyName);

    QString employeeName;
    for (const auto& emp_id : employees) {
        if (emp_id.first == currentPurchase.employee_id) {
            employeeName = emp_id.second;
            break;
        }
    }
    ui->comboBoxEmployee->setCurrentText(employeeName);
}

void PurchaseForm::save() {
    QString errorMessage;

    // Проверка на пустые поля
    if (ui->dateEdit->text().isEmpty()) {
        errorMessage += "Поле 'Дата' не может быть пустым.\n";
    }
    if (ui->comboBoxNomenclature->currentText().isEmpty()) {
        errorMessage += "Поле 'Номенклатура' не может быть пустым.\n";
    }
    if (ui->comboBoxCounterparty->currentText().isEmpty()) {
        errorMessage += "Поле 'Контрагент' не может быть пустым.\n";
    }
    if (ui->comboBoxEmployee->currentText().isEmpty()) {
        errorMessage += "Поле 'Сотрудник' не может быть пустым.\n";
    }
    if (ui->leQuantity->text().isEmpty()) {
        errorMessage += "Поле 'Количество' не может быть пустым.\n";
    }
    if (ui->leUnitPrice->text().isEmpty()) {
        errorMessage += "Поле 'Цена за единицу' не может быть пустым.\n";
    }

    // Если есть пустые поля, выводим сообщение об ошибке и прерываем операцию
    if (!errorMessage.isEmpty()) {
        QMessageBox::critical(this, "Ошибка в полях", errorMessage);
        if (currentPurchaseIndex < 0)
            add();
        return;
    }

    if(currentPurchaseIndex >= 0) {
        currentPurchaseId = purchases[currentPurchaseIndex].purchase_id;
    }
    else
        currentPurchaseId  = -1;

    Purchase savePurchase;

    savePurchase.purchase_date = ui->dateEdit->date().toString("yyyy-MM-dd");
    savePurchase.nomenclature_id = ui->comboBoxNomenclature->currentData().toInt();
    savePurchase.counterparty_id = ui->comboBoxCounterparty->currentData().toInt();
    savePurchase.employee_id = ui->comboBoxEmployee->currentData().toInt();
    savePurchase.nomenclature_quantity = ui->leQuantity->text();
    savePurchase.unit_price = ui->leUnitPrice->text();

    ui->dateEditMin->setDate(QDate(2000, 1, 1)); // Устанавливаем начальную дату 2000-01-01
    ui->dateEditMax->setDate(QDate::currentDate());

    int tempIndex = currentPurchaseIndex;
    showPurchase();
    if (tempIndex < 0)
        currentPurchaseIndex = -1;

    int duplicateIndex = -1;
    for (int i = 0; i < purchases.size(); ++i) {
        const auto& purchase = purchases[i];
        if (purchase.purchase_date == savePurchase.purchase_date &&
            purchase.nomenclature_id == savePurchase.nomenclature_id &&
            purchase.counterparty_id == savePurchase.counterparty_id &&
            purchase.employee_id == savePurchase.employee_id &&
            purchase.nomenclature_quantity == savePurchase.nomenclature_quantity &&
            purchase.unit_price == savePurchase.unit_price &&
            purchase.purchase_id != currentPurchaseId) {
            duplicateIndex = i;
            break;
        }
        if (purchase.purchase_id == currentPurchaseId) {
            currentPurchaseIndex = i;
        }
    }

    if (duplicateIndex < 0) {
        if (currentPurchaseIndex < 0) {
            QSqlQuery query;
            QString insertQuery = QString("INSERT INTO purchase(purchase_date, nomenclature_id, counterparty_id, employee_id, nomenclature_quantity, unit_price) "
                                          "VALUES ('%1', %2, %3, %4, '%5', '%6') RETURNING purchase_id")
                                      .arg(savePurchase.purchase_date)
                                      .arg(savePurchase.nomenclature_id)
                                      .arg(savePurchase.counterparty_id)
                                      .arg(savePurchase.employee_id)
                                      .arg(savePurchase.nomenclature_quantity)
                                      .arg(savePurchase.unit_price);
            if (!query.exec(insertQuery)) {
                QMessageBox::critical(this, "Ошибка в полях", "Ошибка в полях. Запись не добавлена");
                add();
                return;
            } else {
                if (query.next()) {
                    savePurchase.purchase_id = query.value(0).toInt();
                    purchases.append(savePurchase);
                    QMessageBox::information(this, "Успех", "Запись добавлена");
                }
                else {
                    QMessageBox::critical(this, "Ошибка", "Не удалось получить ID новой записи");
                    return;
                }
            }
            currentPurchaseIndex = purchases.size() - 1;
        }
        else
        {
            QSqlQuery query;
            QString updateQuery = QString("UPDATE purchase "
                                          "SET purchase_date = '%1', "
                                          "nomenclature_id = %2, "
                                          "counterparty_id = %3, "
                                          "employee_id = %4, "
                                          "nomenclature_quantity = '%5', "
                                          "unit_price = '%6' "
                                          "WHERE purchase_id = %7;")
                                      .arg(savePurchase.purchase_date)
                                      .arg(savePurchase.nomenclature_id)
                                      .arg(savePurchase.counterparty_id)
                                      .arg(savePurchase.employee_id)
                                      .arg(savePurchase.nomenclature_quantity)
                                      .arg(savePurchase.unit_price)
                                      .arg(currentPurchaseId);
            if (!query.exec(updateQuery)) {
                QMessageBox::critical(this, "Ошибка в полях", "Ошибка в полях. Запись не изменена\n" + query.lastError().text());
                return;
            } else {
                QMessageBox::information(this, "Успех", "Запись изменена");
            }
            purchases[currentPurchaseIndex].purchase_date = savePurchase.purchase_date;
            purchases[currentPurchaseIndex].nomenclature_id = savePurchase.nomenclature_id;
            purchases[currentPurchaseIndex].counterparty_id = savePurchase.counterparty_id;
            purchases[currentPurchaseIndex].employee_id = savePurchase.employee_id;
            purchases[currentPurchaseIndex].nomenclature_quantity = savePurchase.nomenclature_quantity;
            purchases[currentPurchaseIndex].unit_price = savePurchase.unit_price;
        }
    }
    else
    {
        QMessageBox::information(this, "Отмена", "Такая запись уже существует");
        currentPurchaseIndex = duplicateIndex;
    }
    showPurchase();
}

void PurchaseForm::delete_this() {
    if (currentPurchaseIndex < 0 || currentPurchaseIndex >= purchases.size()) {
        QMessageBox::critical(this, "Ошибка", "Невозможно удалить запись: индекс записи некорректен.");
        return;
    }

    currentPurchaseId = purchases[currentPurchaseIndex].purchase_id;
    QSqlQuery query;
    QString deleteQuery = QString("DELETE FROM purchase WHERE purchase_id = %1;").arg(currentPurchaseId);

    if (!query.exec(deleteQuery)) {
        qDebug() << "Ошибка при удалении записи:" << query.lastError().text();
        QMessageBox::critical(this, "Ошибка", "Не удалось удалить запись!");
    } else {
        for (auto it = purchases.begin(); it != purchases.end(); ++it) {
            if ((*it).purchase_id == currentPurchaseId) {
                it = purchases.erase(it);
                break;
            }
        }

        if (currentPurchaseIndex > 0) {
            --currentPurchaseIndex;
        } else if (!purchases.isEmpty()) {
            ++currentPurchaseIndex;
        }

        display();

        QMessageBox::information(this, "Успех", "Запись удалена!");
    }
}


void PurchaseForm::loadNomenclatures() {
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


void PurchaseForm::loadCounterparties() {
    counterparties.clear();
    ui->comboBoxCounterparty->clear();

    QSqlQuery query("SELECT * FROM counterparty WHERE counterparty_type = 'Поставщик'");
    while (query.next()) {
        int id = query.value("counterparty_id").toInt();
        QString name = query.value("counterparty_name").toString();
        counterparties.append(qMakePair(id, name));
        ui->comboBoxCounterparty->addItem(name, id);
    }
}


void PurchaseForm::loadEmployees() {
    employees.clear();
    ui->comboBoxEmployee->clear();

    QSqlQuery query("SELECT * FROM employee WHERE employee_group_id IN (SELECT employee_group_id FROM employee_group WHERE authority_limitation = 'Менеджер по закупкам и продажам')");
    while (query.next()) {
        int id = query.value("employee_id").toInt();
        QString fullName = query.value("full_name").toString();
        employees.append(qMakePair(id, fullName));
        ui->comboBoxEmployee->addItem(fullName, id);
    }
}

void PurchaseForm::setupValidators(const QString &text) {
    // Сохраняем текущую позицию курсора
    previousCursorPosition = ui->leUnitPrice->cursorPosition();
    if (text.isEmpty()) {
        ui->leUnitPrice->setInputMask("");
    } else {
        // Устанавливаем валидатор для поля Цена за единицу, разрешая только числа с точкой
        QRegularExpression Regex("[0-9]+(?:\\.[0-9]{2}){1}");
        QValidator *validator = new QRegularExpressionValidator(Regex, this);
        ui->leUnitPrice->setValidator(validator);
    }
    ui->leUnitPrice->setCursorPosition(previousCursorPosition);
}

void PurchaseForm::on_dateEditMin_dateChanged(const QDate &date) {
    if (ui->dateEditMax->date() < date) {
        ui->dateEditMax->setDate(date);
    }
    showPurchase();
}

void PurchaseForm::on_dateEditMax_dateChanged(const QDate &date) {
    if (ui->dateEditMin->date() > date) {
        ui->dateEditMin->setDate(date);
    }
    showPurchase();
}

void PurchaseForm::showPurchaseReport(){
    if (!purchaseReportForm)
    {
        purchaseReportForm = new PurchaseReport();
        connect(purchaseReportForm, &PurchaseReport::finished, this, &PurchaseForm::purchaseReportClosed);
    }
    purchaseReportForm->exec();
}

void PurchaseForm::purchaseReportClosed()
{
    // Удаляем виджет
    delete purchaseReportForm;
    purchaseReportForm = nullptr;
}


void PurchaseForm::logout()
{
    this->close();
}
