#include "saleform.h"
#include "ui_saleform.h"
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>
#include <QDate>
#include <QDoubleValidator>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>
#include <QPrintPreviewDialog>
#include "../../QrCodeGenerator/qrcodegen.hpp"

using std::uint8_t;
using qrcodegen::QrCode;
using qrcodegen::QrSegment;

SaleForm::SaleForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaleForm),
    saleReportForm(nullptr)
{
    ui->setupUi(this);
    ui->dateEdit->setCalendarPopup(true); // Устанавливаем календарь для выбора даты
    ui->dateEdit->setDate(QDate::currentDate()); // Устанавливаем текущую дату
    connect(ui->next, &QPushButton::clicked, this, &SaleForm::next);
    connect(ui->previous, &QPushButton::clicked, this, &SaleForm::previous);
    connect(ui->add, &QPushButton::clicked, this, &SaleForm::add);
    connect(ui->save, &QPushButton::clicked, this, &SaleForm::save);
    connect(ui->cancel, &QPushButton::clicked, this, &SaleForm::display);
    connect(ui->delete_2, &QPushButton::clicked, this, &SaleForm::delete_this);
    connect(ui->logout, &QPushButton::clicked, this, &SaleForm::logout);
    connect(ui->lePricePerKilo, &QLineEdit::textChanged, this, &SaleForm::setupValidators);
    connect(ui->leWeight, &QLineEdit::textChanged, this, &SaleForm::setupValidators);
    connect(ui->leCarpQuantity, &QLineEdit::textChanged, this, &SaleForm::setupValidators);
    ui->dateEditMin->setCalendarPopup(true);
    ui->dateEditMax->setCalendarPopup(true);
    ui->dateEditMin->setDate(QDate(2000, 1, 1));
    ui->dateEditMax->setDate(QDate::currentDate());
    connect(ui->dateEditMin, &QDateEdit::dateChanged, this, &SaleForm::on_dateEditMin_dateChanged);
    connect(ui->dateEditMax, &QDateEdit::dateChanged, this, &SaleForm::on_dateEditMax_dateChanged);
    loadCounterparties();
    loadEmployees();
    loadCarpSpecies();

    showSale();
}


SaleForm::~SaleForm()
{
    delete ui;
}

void SaleForm::showSale() {
    db = QSqlDatabase::database();
    if (!db.isValid()) {
        qDebug() << "Ошибка: не удалось установить соединение с базой данных.";
        return;
    }

    QSqlQuery query;
    QString queryString = "SELECT * FROM sale";
    QString condition = "";

    if (ui->dateEditMin->date() != QDate(2000, 1, 1) || ui->dateEditMax->date() != QDate::currentDate()) {
        condition = " WHERE sale_date BETWEEN :minDate AND :maxDate";
    }

    queryString += condition + " ORDER BY sale_id";
    query.prepare(queryString);

    if (!condition.isEmpty()) {
        QString minDateFormatted = ui->dateEditMin->date().toString("yyyy-MM-dd");
        QString maxDateFormatted = ui->dateEditMax->date().toString("yyyy-MM-dd");
        query.bindValue(":minDate", minDateFormatted);
        query.bindValue(":maxDate", maxDateFormatted);
        currentSaleIndex = 0;
    }

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        return;
    }

    // Очищаем список перед загрузкой новых данных
    sales.clear();

    while (query.next()) {
        Sale sale;
        sale.sale_id = query.value("sale_id").toInt();
        sale.sale_date = query.value("sale_date").toString();
        sale.carp_in_pond_id = query.value("carp_in_pond_id").toInt();
        sale.counterparty_id = query.value("counterparty_id").toInt();
        sale.employee_id = query.value("employee_id").toInt();
        sale.weight = query.value("weight").toString();
        sale.price_per_kilo = query.value("price_per_kilo").toString();
        sale.carp_quantity = query.value("carp_quantity").toString();
        sales.append(sale);
    }
    if (sales.size() == 0) {
        QMessageBox::critical(this, "Ошибка", "Ничего не найдено");
        ui->dateEditMin->setDate(QDate(2000, 1, 1)); // Устанавливаем начальную дату 2000-01-01
        ui->dateEditMax->setDate(QDate::currentDate());
        showSale();
    }
    display();
}


void SaleForm::next() {
    if (currentSaleIndex < sales.size() - 1) {
        ++currentSaleIndex;
        display();
    }
}

void SaleForm::previous() {
    if (currentSaleIndex > 0) {
        --currentSaleIndex;
        display();
    }
}

void SaleForm::add() {
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

    // Устанавливаем валидатор для поля Вес и Цена за кило, разрешая только числа
    QDoubleValidator *validator = new QDoubleValidator(0.0, 999999.99, 2, this);
    ui->leWeight->setValidator(validator);
    ui->leWeight->clear();
    ui->lePricePerKilo->clear();
    ui->leCarpQuantity->clear();

    ui->comboBoxCarpSpecies->setCurrentIndex(-1);
    ui->comboBoxCounterparty->setCurrentIndex(-1);
    ui->comboBoxEmployee->setCurrentIndex(-1);
    currentSaleIndex = -1;
}

void SaleForm::display() {
    if (currentSaleIndex == -1)
        currentSaleIndex = 0;

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

    const Sale& currentSale = sales[currentSaleIndex];
    QDate date = QDate::fromString(currentSale.sale_date, "yyyy-MM-dd");
    ui->dateEdit->setDate(date);
    ui->leWeight->setText(currentSale.weight);
    ui->lePricePerKilo->setText(currentSale.price_per_kilo);
    ui->leCarpQuantity->setText(currentSale.carp_quantity);

    QString carpSpeciesInfo;
    for (const auto& species : carpSpecies) {
        if (species.first == currentSale.carp_in_pond_id) {
            carpSpeciesInfo = species.second;
            break;
        }
    }
    ui->comboBoxCarpSpecies->setCurrentText(carpSpeciesInfo);

    QString counterpartyName;
    for (const auto& cp_id : counterparties) {
        if (cp_id.first == currentSale.counterparty_id) {
            counterpartyName = cp_id.second;
            break;
        }
    }
    ui->comboBoxCounterparty->setCurrentText(counterpartyName);

    QString employeeName;
    for (const auto& emp_id : employees) {
        if (emp_id.first == currentSale.employee_id) {
            employeeName = emp_id.second;
            break;
        }
    }
    ui->comboBoxEmployee->setCurrentText(employeeName);
}

void SaleForm::save() {
    QString errorMessage;

    // Проверка на пустые поля
    if (ui->dateEdit->text().isEmpty()) {
        errorMessage += "Поле 'Дата' не может быть пустым.\n";
    }
    if (ui->comboBoxCarpSpecies->currentText().isEmpty()) {
        errorMessage += "Поле 'Вид карпа' не может быть пустым.\n";
    }
    if (ui->comboBoxCounterparty->currentText().isEmpty()) {
        errorMessage += "Поле 'Контрагент' не может быть пустым.\n";
    }
    if (ui->comboBoxEmployee->currentText().isEmpty()) {
        errorMessage += "Поле 'Сотрудник' не может быть пустым.\n";
    }
    if (ui->leWeight->text().isEmpty()) {
        errorMessage += "Поле 'Вес' не может быть пустым.\n";
    }
    if (ui->lePricePerKilo->text().isEmpty()) {
        errorMessage += "Поле 'Цена за кило' не может быть пустым.\n";
    }
    if (ui->leCarpQuantity->text().isEmpty()) {
        errorMessage += "Поле 'Количество карпа' не может быть пустым.\n";
    }

    // Если есть пустые поля, выводим сообщение об ошибке и прерываем операцию
    if (!errorMessage.isEmpty()) {
        QMessageBox::critical(this, "Ошибка в полях", errorMessage);
        if (currentSaleIndex < 0)
            add();
        return;
    }

    if(currentSaleIndex >= 0) {
        currentSaleId = sales[currentSaleIndex].sale_id;
    }
    else
        currentSaleId  = -1;

    Sale saveSale;

    saveSale.sale_date = ui->dateEdit->date().toString("yyyy-MM-dd");
    saveSale.carp_in_pond_id = ui->comboBoxCarpSpecies->currentData().toInt();
    saveSale.counterparty_id = ui->comboBoxCounterparty->currentData().toInt();
    saveSale.employee_id = ui->comboBoxEmployee->currentData().toInt();
    saveSale.weight = ui->leWeight->text();
    saveSale.price_per_kilo = ui->lePricePerKilo->text();
    saveSale.carp_quantity = ui->leCarpQuantity->text();

    ui->dateEditMin->setDate(QDate(2000, 1, 1)); // Устанавливаем начальную дату 2000-01-01
    ui->dateEditMax->setDate(QDate::currentDate());

    int tempIndex = currentSaleIndex;
    showSale();
    if (tempIndex < 0)
        currentSaleIndex = -1;

    int duplicateIndex = -1;
    for (int i = 0; i < sales.size(); ++i) {
        const auto& sale = sales[i];
        if (sale.sale_date == saveSale.sale_date &&
            sale.carp_in_pond_id == saveSale.carp_in_pond_id &&
            sale.counterparty_id == saveSale.counterparty_id &&
            sale.employee_id == saveSale.employee_id &&
            sale.weight == saveSale.weight &&
            sale.price_per_kilo == saveSale.price_per_kilo &&
            sale.carp_quantity == saveSale.carp_quantity &&
            sale.sale_id != currentSaleId) {
            duplicateIndex = i;
            break;
        }
        if (sale.sale_id == currentSaleId) {
            currentSaleIndex = i;
        }
    }

    if (duplicateIndex < 0) {
        if (currentSaleIndex < 0) {
            QSqlQuery query;
            QString insertQuery = QString("INSERT INTO sale(sale_date, carp_in_pond_id, counterparty_id, employee_id, weight, price_per_kilo, carp_quantity) "
                                          "VALUES ('%1', %2, %3, %4, '%5', '%6', '%7') RETURNING sale_id")
                                      .arg(saveSale.sale_date)
                                      .arg(saveSale.carp_in_pond_id)
                                      .arg(saveSale.counterparty_id)
                                      .arg(saveSale.employee_id)
                                      .arg(saveSale.weight)
                                      .arg(saveSale.price_per_kilo)
                                      .arg(saveSale.carp_quantity);
            if (!query.exec(insertQuery)) {
                QMessageBox::critical(this, "Ошибка в полях", "Ошибка в полях. Запись не добавлена\n" + query.lastError().text());
                add();
                return;
            } else {
                if (query.next()) {
                    saveSale.sale_id = query.value(0).toInt();
                    sales.append(saveSale);
                    QMessageBox::information(this, "Успех", "Запись добавлена");
                }
                else {
                    QMessageBox::critical(this, "Ошибка", "Не удалось получить ID новой записи");
                    return;
                }
            }
            currentSaleIndex = sales.size() - 1;
        }
        else
        {
            QSqlQuery query;
            QString updateQuery = QString("UPDATE sale "
                                          "SET sale_date = '%1', "
                                          "carp_in_pond_id = %2, "
                                          "counterparty_id = %3, "
                                          "employee_id = %4, "
                                          "weight = '%5', "
                                          "price_per_kilo = '%6', "
                                          "carp_quantity = '%7' "
                                          "WHERE sale_id = %8;")
                                      .arg(saveSale.sale_date)
                                      .arg(saveSale.carp_in_pond_id)
                                      .arg(saveSale.counterparty_id)
                                      .arg(saveSale.employee_id)
                                      .arg(saveSale.weight)
                                      .arg(saveSale.price_per_kilo)
                                      .arg(saveSale.carp_quantity)
                                      .arg(currentSaleId);
            if (!query.exec(updateQuery)) {
                QMessageBox::critical(this, "Ошибка в полях", "Ошибка в полях. Запись не изменена\n" + query.lastError().text());
                return;
            } else {
                QMessageBox::information(this, "Успех", "Запись изменена");
            }
            saveSale.sale_id = sales[currentSaleIndex].sale_id;
            sales[currentSaleIndex].sale_date = saveSale.sale_date;
            sales[currentSaleIndex].carp_in_pond_id = saveSale.carp_in_pond_id;
            sales[currentSaleIndex].counterparty_id = saveSale.counterparty_id;
            sales[currentSaleIndex].employee_id = saveSale.employee_id;
            sales[currentSaleIndex].weight = saveSale.weight;
            sales[currentSaleIndex].price_per_kilo = saveSale.price_per_kilo;
            sales[currentSaleIndex].carp_quantity = saveSale.carp_quantity;
        }
        getQueue(saveSale.sale_id, saveSale.sale_date, ui->comboBoxCarpSpecies->currentText(),
                 ui->comboBoxCounterparty->currentText(),  ui->comboBoxEmployee->currentText(),
                 saveSale.weight, saveSale.price_per_kilo, saveSale.carp_quantity);
    }
    else
    {
        QMessageBox::information(this, "Отмена", "Такая запись уже существует");
        currentSaleIndex = duplicateIndex;
    }
    showSale();
}

void SaleForm::delete_this() {
    if (currentSaleIndex < 0 || currentSaleIndex >= sales.size()) {
        QMessageBox::critical(this, "Ошибка", "Невозможно удалить запись: индекс записи некорректен.");
        return;
    }

    currentSaleId = sales[currentSaleIndex].sale_id;
    QSqlQuery query;
    QString deleteQuery = QString("DELETE FROM sale WHERE sale_id = %1;").arg(currentSaleId);

    if (!query.exec(deleteQuery)) {
        qDebug() << "Ошибка при удалении записи:" << query.lastError().text();
        QMessageBox::critical(this, "Ошибка", "Не удалось удалить запись!");
    } else {
        for (auto it = sales.begin(); it != sales.end(); ++it) {
            if ((*it).sale_id == currentSaleId) {
                it = sales.erase(it);
                break;
            }
        }

        if (currentSaleIndex > 0) {
            --currentSaleIndex;
        } else if (!sales.isEmpty()) {
            ++currentSaleIndex;
        }

        display();

        QMessageBox::information(this, "Успех", "Запись удалена!");
    }
}

void SaleForm::loadCarpSpecies() {
    carpSpecies.clear();
    ui->comboBoxCarpSpecies->clear();

    QSqlQuery query("SELECT sale.carp_in_pond_id, carp.species, carp.carp_type, pond.pond_name "
                    "FROM sale "
                    "JOIN carp_in_pond ON sale.carp_in_pond_id = carp_in_pond.carp_in_pond_id "
                    "JOIN carp ON carp_in_pond.carp_id = carp.carp_id "
                    "JOIN pond ON carp_in_pond.pond_id = pond.pond_id "
                    "ORDER BY carp_in_pond.carp_in_pond_id;");
    while (query.next()) {
        int id = query.value("carp_in_pond_id").toInt();
        QString name = query.value("species").toString() + " — " + query.value("carp_type").toString() + " — " + query.value("pond_name").toString();
        carpSpecies.append(qMakePair(id, name));
        ui->comboBoxCarpSpecies->addItem(name, id);
    }
}

void SaleForm::loadCounterparties() {
    counterparties.clear();
    ui->comboBoxCounterparty->clear();

    QSqlQuery query("SELECT * FROM counterparty WHERE counterparty_type = 'Покупатель'");
    while (query.next()) {
        int id = query.value("counterparty_id").toInt();
        QString name = query.value("counterparty_name").toString();
        counterparties.append(qMakePair(id, name));
        ui->comboBoxCounterparty->addItem(name, id);
    }
}

void SaleForm::loadEmployees() {
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

void SaleForm::setupValidators(const QString &text) {
    // Сохраняем текущую позицию курсора
    previousCursorPosition = ui->lePricePerKilo->cursorPosition();

    if (text.isEmpty()) {
        ui->lePricePerKilo->setInputMask("");
    } else {
        // Устанавливаем валидатор для поля Цена за кило, разрешая только числа с точкой
        QRegularExpression Regex("[0-9]+(?:\\.[0-9]{2}){1}");
        QValidator *validator = new QRegularExpressionValidator(Regex, this);
        ui->lePricePerKilo->setValidator(validator);
    }

    // Восстанавливаем позицию курсора
    ui->lePricePerKilo->setCursorPosition(previousCursorPosition);

    // Сохраняем текущую позицию курсора
    previousCursorPosition = ui->leWeight->cursorPosition();

    if (text.isEmpty()) {
        ui->leWeight->setInputMask("");
    } else {
        // Устанавливаем валидатор для поля Вес, разрешая только числа с точкой
        QRegularExpression Regex("[0-9]+(?:\\.[0-9]{2}){1}");
        QValidator *validator = new QRegularExpressionValidator(Regex, this);
        ui->leWeight->setValidator(validator);
    }

    // Восстанавливаем позицию курсора
    ui->leWeight->setCursorPosition(previousCursorPosition);

    // Сохраняем текущую позицию курсора
    previousCursorPosition = ui->leCarpQuantity->cursorPosition();

    if (text.isEmpty()) {
        ui->leCarpQuantity->setInputMask("");
    } else {
        // Устанавливаем валидатор для поля Число карпов, разрешая только целые числа
        QIntValidator *validator = new QIntValidator(1, 999999, this);
        ui->leCarpQuantity->setValidator(validator);
    }

    // Восстанавливаем позицию курсора
    ui->leCarpQuantity->setCursorPosition(previousCursorPosition);
}
void SaleForm::on_dateEditMin_dateChanged(const QDate &date) {
    if (ui->dateEditMax->date() < date) {
        ui->dateEditMax->setDate(date);
    }
    showSale();
}

void SaleForm::on_dateEditMax_dateChanged(const QDate &date) {
    if (ui->dateEditMin->date() > date) {
        ui->dateEditMin->setDate(date);
    }
    showSale();
}

void SaleForm::showSaleReport(){
    if (!saleReportForm)
    {
        saleReportForm = new SaleReport();
        connect(saleReportForm, &SaleReport::finished, this, &SaleForm::SaleReportClosed);
    }
    saleReportForm->exec();
}

void SaleForm::SaleReportClosed()
{
    // Удаляем виджет
    delete saleReportForm;
    saleReportForm = nullptr;
}

void SaleForm::getQueue(int sale_id, QString sale_date, QString species, QString counterparty, QString employee, QString weight, QString price_per_kilo, QString carp_quantity)
{
    QString fileName = QString("/Users/jerry/BMSTU/Databases/course_work/qt_app/carp_farm1/%1.txt").arg(sale_id);
    QString data; // <-- Создаем переменную для хранения данных в виде строки

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        out << "Номер продажи: " << sale_id << "\n"; // <-- Здесь я вывел данные в файл
        QString currID = QString::number(sale_id);
        data += QString("Номер продажи: ") + currID + "\n";// <-- Здесь я закинул данные в строку

        out << "Дата продажи: " << sale_date << "\n";
        data += QString("Дата продажи: ") + sale_date + "\n"; // <-- Здесь я закинул данные в строку

        out << QString("Клиент: ") << counterparty << "\n\n";
        data += "Клиент: " + counterparty + "\n\n"; // <-- Здесь я закинул данные в строку

        out << "Карп: " << species << "\n";
        data += QString("Карп: ") + species + "\n"; // <-- Здесь я закинул данные в строку

        out << "Сотрудник: " << employee << "\n";
        data += QString("Сотрудник: ") + employee + "\n"; // <-- Здесь я закинул данные в строку

        out << "Вес: " << weight << " кг\n";
        data += QString("Вес: ") + weight + " кг\n\n"; // <-- Здесь я закинул данные в строку

        out << "Цена за кг: " << price_per_kilo << " руб\n";
        data += QString("Цена за кг: ") + price_per_kilo + " руб\n\n"; // <-- Здесь я закинул данные в строку

        out << "Количество карпов: " << carp_quantity << " шт.\n";
        data += QString("Количество карпов: ") + carp_quantity + " шт\n\n"; // <-- Здесь я закинул данные в строку


        //Генерация QR-кода по данным в data
        const QrCode::Ecc errCorLvl = QrCode::Ecc::LOW; //
        std::string dataStr = data.toStdString(); // Преобразование в const char*
        const QrCode qr = QrCode::encodeText(dataStr.c_str(), errCorLvl); //Создали QR-код

        //Вывод QR-кода
        QString result = QString("");
        int border = 0;
        for (int y = -border; y < qr.getSize() + border; y++) {
            for (int x = -border; x < qr.getSize() + border; x++) {
                result += (qr.getModule(x, y) ? "██" : "  ");
            }
            result += "\n";
        }
        result += "\n";
        out << result;

        // Закрытие файла после записи
        file.close();
    } else {
        // Если не удалось открыть файл для записи, выводим сообщение об ошибке
        qDebug() << "Error creating queue file.";
    }
}


void SaleForm::logout()
{
    this->close();
}
