#include "counterpartyform.h"
#include "ui_counterpartyform.h"
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>
#include <QWebEngineView>
#include <QDesktopServices>
#include <QUrl>
#include <QRegularExpressionValidator>

CounterpartyForm::CounterpartyForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CounterpartyForm),
    counterpartyReportForm(nullptr)
{
    ui->setupUi(this);
    connect(ui->next, &QPushButton::clicked, this, &CounterpartyForm::next);
    connect(ui->previous, &QPushButton::clicked, this, &CounterpartyForm::previous);
    connect(ui->add, &QPushButton::clicked, this, &CounterpartyForm::add);
    connect(ui->save, &QPushButton::clicked, this, &CounterpartyForm::save);
    connect(ui->cancel, &QPushButton::clicked, this, &CounterpartyForm::display);
    connect(ui->delete_2, &QPushButton::clicked, this, &CounterpartyForm::delete_this);
    connect(ui->lineEditSearch, &QLineEdit::textChanged, this, &CounterpartyForm::searchCounterparty);
    connect(ui->logout, &QPushButton::clicked, this, &CounterpartyForm::logout);
    connect(ui->report, &QPushButton::clicked, this, &CounterpartyForm::showCounterpartyReport);
    connect(ui->openMail, &QPushButton::clicked, this, &CounterpartyForm::openGoogleMail);
    connect(ui->lePhoneNumber, &QLineEdit::textChanged, this, &CounterpartyForm::updatePhoneNumberMask);
    connect(ui->leEmail, &QLineEdit::textChanged, this, &CounterpartyForm::updateEmailMask);
    showCounterparty();
    loadCounterpartyTypes();
}

CounterpartyForm::~CounterpartyForm()
{
    delete ui;
}

void CounterpartyForm::showCounterparty() {
    db = QSqlDatabase::database();
    if (!db.isValid()) {
        qDebug() << "Ошибка: не удалось установить соединение с базой данных.";
        return;
    }

    QSqlQuery query;
    QString queryString = "SELECT * FROM counterparty";
    if (!searchFilter.isEmpty()) {
        currentCounterpartyIndex = 0;
        queryString += " WHERE counterparty_name LIKE '" + searchFilter + "%'";
    }
    queryString += " ORDER BY counterparty_id";
    if (!query.exec(queryString)) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        return;
    }

    counterparties.clear();
    while (query.next()) {
        Counterparty counterparty;
        counterparty.counterparty_id = query.value("counterparty_id").toInt();
        counterparty.counterparty_name = query.value("counterparty_name").toString();
        counterparty.counterparty_type = query.value("counterparty_type").toString();
        counterparty.phone_number = query.value("phone_number").toString();
        counterparty.email = query.value("email").toString();
        counterparty.address = query.value("address").toString();
        counterparties.append(counterparty);
    }
    if (counterparties.size() == 0) {
        QMessageBox::critical(this, "Ошибка", "Ничего не найдено");
        ui->lineEditSearch->clear();
    }
    display();
}

void CounterpartyForm::next() {
    if (currentCounterpartyIndex < counterparties.size() - 1) {
        ++currentCounterpartyIndex;
        display();
    }
}

void CounterpartyForm::previous() {
    if (currentCounterpartyIndex > 0) {
        --currentCounterpartyIndex;
        display();
    }
}

void CounterpartyForm::add() {
    ui->lineEditSearch->hide();
    ui->next->hide();
    ui->previous->hide();
    ui->add->hide();
    ui->logout->hide();
    ui->delete_2->hide();
    ui->label_3->hide();
    ui->cancel->show();
    ui->label_4->show();
    ui->openMail->hide();

    ui->leName->clear();
    ui->comboBoxType->setCurrentIndex(-1);
    ui->lePhoneNumber->clear();
    ui->lePhoneNumber->setInputMask("(000) 000-00-00");
    ui->leEmail->clear();
    ui->teAddress->clear();
    QString addressRegex = "[А-Яа-я\\s]+,\\sул\\.\\s[А-Яа-я\\s]+,\\s\\d+";
    QRegularExpression addressRegExp(addressRegex);
    QValidator *addressValidator = new QRegularExpressionValidator(addressRegExp, this);
    ui->teAddress->setValidator(addressValidator);

    QRegularExpression emailRegex("[A-Za-z0-9]+(?:[._%+-][A-Za-z0-9]+)*@[A-Za-z0-9-]+(?:\\.[A-Za-z]{2,3}){1}");
    QValidator *emailValidator = new QRegularExpressionValidator(emailRegex, this);
    ui->leEmail->setValidator(emailValidator);
    currentCounterpartyIndex = -1;
}

void CounterpartyForm::display() {
    if (currentCounterpartyIndex == -1)
        currentCounterpartyIndex = 0;
    ui->lineEditSearch->show();
    ui->cancel->hide();
    ui->label_4->hide();
    ui->next->show();
    ui->logout->show();
    ui->previous->show();
    ui->add->show();
    ui->delete_2->show();
    ui->label_3->show();
    ui->openMail->show();

    const Counterparty& currentCounterparty = counterparties[currentCounterpartyIndex];

    ui->leName->setText(currentCounterparty.counterparty_name);
    ui->comboBoxType->setCurrentText(currentCounterparty.counterparty_type);
    ui->lePhoneNumber->setText(currentCounterparty.phone_number);
    ui->leEmail->setText(currentCounterparty.email);
    ui->teAddress->setText(currentCounterparty.address);


    // Создаем виджет WebView для отображения карты Google Maps внутри iframe
    QWebEngineView *mapView = new QWebEngineView;
    QString mapUrl = "<iframe width=\"400\" height=\"300\" frameborder=\"0\" style=\"border:0\" src=\"https://www.google.com/maps/embed/v1/place?key=AIzaSyBGerk23H7blFhKh3AuRLsfk4OoOaH2Vow&q=" + currentCounterparty.address + "\" allowfullscreen></iframe>";
    mapView->setHtml(mapUrl);

    // Очищаем существующий макет, если он есть
    QLayout* existingLayout = ui->widgetMap->layout();
    if (existingLayout)
        delete existingLayout;

    // Создаем вертикальный контейнер для размещения текстового поля и карты
    QVBoxLayout *layout = new QVBoxLayout;
    //layout->addWidget(addressBrowser);
    layout->addWidget(mapView);
    ui->widgetMap->setLayout(layout); // ui->widgetMap - это ваш виджет, в который вы хотите поместить текстовое поле и карту
    connect(ui->openMapButton, &QPushButton::clicked, this, [currentCounterparty]() {
        QDesktopServices::openUrl(QUrl("https://www.google.com/maps/search/?api=1&query=" + currentCounterparty.address));
    });
}



void CounterpartyForm::save() {
    QString errorMessage;
    // Проверка на пустые поля
    if (ui->leName->text().isEmpty()) {
        errorMessage += "Поле 'Имя контрагента' не может быть пустым.\n";
    }
    if (ui->comboBoxType->currentText().isEmpty()) {
        errorMessage += "Поле 'Тип контрагента' не может быть пустым.\n";
    }
    if (ui->lePhoneNumber->text().isEmpty()) {
        errorMessage += "Поле 'Номер телефона' не может быть пустым.\n";
    }
    if (ui->leEmail->text().isEmpty()) {
        errorMessage += "Поле 'Email' не может быть пустым.\n";
    }
    if (ui->teAddress->text().isEmpty()) {
        errorMessage += "Поле 'Адресс' не может быть пустым.\n";
    }

    // Если есть пустые поля, выводим сообщение об ошибке и прерываем операцию
    if (!errorMessage.isEmpty()) {
        QMessageBox::critical(this, "Ошибка в полях", errorMessage);
        if (currentCounterpartyIndex < 0)
            add();
        return;
    }
    if (currentCounterpartyIndex >= 0)
        currentCounterpartyId = counterparties[currentCounterpartyIndex].counterparty_id;
    else
        currentCounterpartyId = -1;

    Counterparty saveCounterparty;
    saveCounterparty.counterparty_name = ui->leName->text();
    saveCounterparty.counterparty_type = ui->comboBoxType->currentText();
    saveCounterparty.phone_number = ui->lePhoneNumber->text();
    saveCounterparty.email = ui->leEmail->text();
    saveCounterparty.address = ui->teAddress->text();

    int tempIndex = currentCounterpartyIndex;
    ui->lineEditSearch->clear();
    showCounterparty();
    if (tempIndex < 0)
        currentCounterpartyIndex = -1;

    int duplicateIndex = -1;
    for (int i = 0; i < counterparties.size(); ++i) {
        const auto& counterparty = counterparties[i];
        if (counterparty.counterparty_name == saveCounterparty.counterparty_name && counterparty.counterparty_id != currentCounterpartyId) {
            duplicateIndex = i;
            break;
        }
        if (counterparty.counterparty_id == currentCounterpartyId) {
            currentCounterpartyIndex = i;
        }
    }

    if (duplicateIndex < 0 && currentCounterpartyIndex < 0) {
        QSqlQuery query;
        QString insertQuery = QString("INSERT INTO counterparty(counterparty_name, counterparty_type, phone_number, email, address) "
                                      "VALUES ('%1', '%2', '%3', '%4', '%5') RETURNING counterparty_id")
                                  .arg(saveCounterparty.counterparty_name)
                                  .arg(saveCounterparty.counterparty_type)
                                  .arg(saveCounterparty.phone_number)
                                  .arg(saveCounterparty.email)
                                  .arg(saveCounterparty.address);
        if (!query.exec(insertQuery)) {
            QMessageBox::critical(this, "Ошибка в полях", "Поле Название, Телефон или Почта не уникально.\n Запись не добавлена");
            add();
            return;
        } else {
            if (query.next()) {
                saveCounterparty.counterparty_id = query.value(0).toInt();
                counterparties.append(saveCounterparty);
                QMessageBox::information(this, "Успех", "Запись добавлена");
            } else {
                QMessageBox::critical(this, "Ошибка", "Не удалось получить ID новой записи");
                return;
            }
        }
        currentCounterpartyIndex = counterparties.size() - 1;
    }
    else if (duplicateIndex < 0 && currentCounterpartyIndex >= 0)
    {
        QSqlQuery query;
        QString updateQuery = QString("UPDATE counterparty "
                                      "SET counterparty_name = '%1', "
                                      "counterparty_type = '%2', "
                                      "phone_number = '%3', "
                                      "email = '%4', "
                                      "address = '%5' "
                                      "WHERE counterparty_id = %6;")
                                  .arg(saveCounterparty.counterparty_name)
                                  .arg(saveCounterparty.counterparty_type)
                                  .arg(saveCounterparty.phone_number)
                                  .arg(saveCounterparty.email)
                                  .arg(saveCounterparty.address)
                                  .arg(currentCounterpartyId);
        if (!query.exec(updateQuery)) {
            QMessageBox::critical(this, "Ошибка в полях", "Поле Название, Телефон или Почта не уникально.\n Запись не изменена\n" + query.lastError().text());      
            display();
            return;
        } else {
            QMessageBox::information(this, "Успех", "Запись изменена");
        }
        counterparties[currentCounterpartyIndex] = saveCounterparty;
    }
    else
    {
        QMessageBox::information(this, "Отмена", "Контрагент с таким названием уже существует");
        currentCounterpartyIndex = duplicateIndex;
    }
    showCounterparty();
}

void CounterpartyForm::delete_this() {
    currentCounterpartyId = counterparties[currentCounterpartyIndex].counterparty_id;
    QSqlQuery query;
    QString deleteQuery = QString("DELETE FROM counterparty "
                                  "WHERE counterparty_id = %1;")
                              .arg(currentCounterpartyId);
    if (!query.exec(deleteQuery)) {
        qDebug() << "Ошибка при удалении записи:" << query.lastError().text();
        QMessageBox::information(this, "Ошибка", "Не удалось удалить запись!");
    } else {
        for (auto it = counterparties.begin(); it != counterparties.end(); ++it) {
            if ((*it).counterparty_id == currentCounterpartyId) {
                it = counterparties.erase(it);
                break;
            }
        }

        ui->leName->clear();
        ui->comboBoxType->setCurrentIndex(-1);
        ui->lePhoneNumber->clear();
        ui->leEmail->clear();
        ui->teAddress->clear();

        if (currentCounterpartyIndex > 0) {
            --currentCounterpartyIndex;
        } else if (!counterparties.isEmpty()) {
            ++currentCounterpartyIndex;
        }

        display();

        QMessageBox::information(this, "Успех", "Запись удалена!");
    }
}

void CounterpartyForm::loadCounterpartyTypes() {
    counterpartyTypes.clear();
    ui->comboBoxType->clear();

    counterpartyTypes << "Поставщик" << "Покупатель";

    ui->comboBoxType->addItems(counterpartyTypes);
}

void CounterpartyForm::searchCounterparty(const QString &text) {
    searchFilter = text;
    showCounterparty();
}

void CounterpartyForm::openGoogleMail() {
    const QString& email = ui->leEmail->text();
    if (!email.isEmpty()) {
        QUrl url("https://mail.google.com/mail/?view=cm&fs=1&to=" + email);
        QDesktopServices::openUrl(url);
    } else {
        QMessageBox::warning(this, "Пустой адрес", "Адрес электронной почты контрагента не указан.");
    }
}

void CounterpartyForm::updatePhoneNumberMask(const QString &text) {
    // Сохраняем текущую позицию курсора
    previousCursorPosition = ui->lePhoneNumber->cursorPosition();

    // Если текст пустой, устанавливаем пустую маску
    if (text.isEmpty()) {
        ui->lePhoneNumber->setInputMask("");
    } else {
        // В противном случае устанавливаем маску для телефонного номера
        ui->lePhoneNumber->setInputMask("(000) 000-00-00");
    }

    // Восстанавливаем позицию курсора
    ui->lePhoneNumber->setCursorPosition(previousCursorPosition);
}

void CounterpartyForm::updateEmailMask(const QString &text) {
    // Сохраняем текущую позицию курсора
    previousCursorPosition = ui->leEmail->cursorPosition();

    // Если текст пустой, устанавливаем пустую маску
    if (text.isEmpty()) {
        ui->leEmail->setInputMask("");
    } else {
        // В противном случае устанавливаем маску для email
        QRegularExpression emailRegex("[A-Za-z0-9]+(?:[._%+-][A-Za-z0-9]+)*@[A-Za-z0-9-]+(?:\\.[A-Za-z]{2,3}){1}");
        QValidator *emailValidator = new QRegularExpressionValidator(emailRegex, this);
        ui->leEmail->setValidator(emailValidator);
    }

    // Восстанавливаем позицию курсора
    ui->leEmail->setCursorPosition(previousCursorPosition);
}

void CounterpartyForm::showCounterpartyReport()
{
    if (!counterpartyReportForm)
    {
        counterpartyReportForm = new CounterpartyReport();
        connect(counterpartyReportForm, &CounterpartyReport::finished, this, &CounterpartyForm::CounterpartyReportClosed);
    }
    counterpartyReportForm->exec();
}

// В методе SpecialistWindow::CounterpartyReportClosed()
void CounterpartyForm::CounterpartyReportClosed()
{
    // Удаляем виджет
    delete counterpartyReportForm;
    counterpartyReportForm = nullptr;
}


void CounterpartyForm::logout()
{
    this->close();
}
