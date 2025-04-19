#include "employeeform.h"
#include "ui_employeeform.h"
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>
#include <QWebEngineView>
#include <QDesktopServices>
#include <QUrl>
#include <QRegularExpressionValidator>

EmployeeForm::EmployeeForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EmployeeForm),
    employeeReport(nullptr)
{
    ui->setupUi(this);
    connect(ui->next, &QPushButton::clicked, this, &EmployeeForm::next);
    connect(ui->previous, &QPushButton::clicked, this, &EmployeeForm::previous);
    connect(ui->add, &QPushButton::clicked, this, &EmployeeForm::add);
    connect(ui->save, &QPushButton::clicked, this, &EmployeeForm::save);
    connect(ui->cancel, &QPushButton::clicked, this, &EmployeeForm::display);
    connect(ui->delete_2, &QPushButton::clicked, this, &EmployeeForm::delete_this);
    connect(ui->lineEditSearch, &QLineEdit::textChanged, this, &EmployeeForm::searchEmployee);
    connect(ui->logout, &QPushButton::clicked, this, &EmployeeForm::logout);
    connect(ui->openMail, &QPushButton::clicked, this, &EmployeeForm::openGoogleMail);
    connect(ui->lePhoneNumber, &QLineEdit::textChanged, this, &EmployeeForm::updatePhoneNumberMask);
    connect(ui->leEmail, &QLineEdit::textChanged, this, &EmployeeForm::updateEmailMask);
    connect(ui->employeeReportButton, &QPushButton::clicked, this, &EmployeeForm::showEmployeeReport);
    showEmployee();
    loadEmployeeGroups();
}

EmployeeForm::~EmployeeForm()
{
    delete ui;
}

void EmployeeForm::showEmployee() {
    db = QSqlDatabase::database();
    if (!db.isValid()) {
        qDebug() << "Ошибка: не удалось установить соединение с базой данных.";
        return;
    }

    QSqlQuery query;
    QString queryString = "SELECT * FROM employee";
    if (!searchFilter.isEmpty()) {
        currentEmployeeIndex = 0;
        queryString += " WHERE full_name LIKE '" + searchFilter + "%'";
    }
    queryString += " ORDER BY employee_id";
    if (!query.exec(queryString)) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        return;
    }

    employees.clear();
    while (query.next()) {
        Employee employee;
        employee.employee_id = query.value("employee_id").toInt();
        employee.full_name = query.value("full_name").toString();
        employee.phone_number = query.value("phone_number").toString();
        employee.email = query.value("email").toString();
        employee.employee_group_id = query.value("employee_group_id").toInt();
        employees.append(employee);
    }
    if (employees.size() == 0) {
        QMessageBox::critical(this, "Ошибка", "Ничего не найдено");
        ui->lineEditSearch->clear();
    }
    display();
}

void EmployeeForm::next() {
    if (currentEmployeeIndex < employees.size() - 1) {
        ++currentEmployeeIndex;
        display();
    }
}

void EmployeeForm::previous() {
    if (currentEmployeeIndex > 0) {
        --currentEmployeeIndex;
        display();
    }
}

void EmployeeForm::add() {
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

    ui->leFullName->clear();
    ui->lePhoneNumber->clear();
    ui->lePhoneNumber->setInputMask("(000) 000-00-00");
    ui->leEmail->clear();
    ui->comboBoxGroup->setCurrentIndex(-1);
    currentEmployeeIndex = -1;
}

void EmployeeForm::display() {
    if (currentEmployeeIndex == -1)
        currentEmployeeIndex = 0;
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

    const Employee& currentEmployee = employees[currentEmployeeIndex];

    ui->leFullName->setText(currentEmployee.full_name);
    ui->lePhoneNumber->setText(currentEmployee.phone_number);
    ui->leEmail->setText(currentEmployee.email);
    QString employeeGr;
    for (const auto& employee_id : employeeGroups) {
        if (employee_id.first == currentEmployee.employee_group_id) {
            employeeGr = employee_id.second;
            break;
        }
    }
    ui->comboBoxGroup->setCurrentText(employeeGr);
}

void EmployeeForm::save() {
    QString errorMessage;

    // Проверка на пустые поля
    if (ui->leFullName->text().isEmpty()) {
        errorMessage += "Поле 'ФИО сотрудника' не может быть пустым.\n";
    }
    if (ui->lePhoneNumber->text().isEmpty()) {
        errorMessage += "Поле 'Номер телефона' не может быть пустым.\n";
    }
    if (ui->leEmail->text().isEmpty()) {
        errorMessage += "Поле 'Email' не может быть пустым.\n";
    }
    if (ui->comboBoxGroup->currentIndex() < 0) {
        errorMessage += "Не выбрана группа сотрудника.\n";
    }

    // Если есть пустые поля, выводим сообщение об ошибке и прерываем операцию
    if (!errorMessage.isEmpty()) {
        QMessageBox::critical(this, "Ошибка в полях", errorMessage);
        if (currentEmployeeIndex < 0)
            add();
        return;
    }

    if(currentEmployeeIndex >= 0) {
        currentEmployeeId = employees[currentEmployeeIndex].employee_id;
    }
    else
        currentEmployeeId = -1;

    Employee saveEmployee;
    saveEmployee.full_name = ui->leFullName->text();
    saveEmployee.phone_number = ui->lePhoneNumber->text();
    saveEmployee.email = ui->leEmail->text();
    saveEmployee.employee_group_id = ui->comboBoxGroup->currentData().toInt();

    int tempIndex = currentEmployeeIndex;
    ui->lineEditSearch->clear();
    showEmployee();

    if (tempIndex < 0)
        currentEmployeeIndex = -1;

    int duplicateIndex = -1;
    for (int i = 0; i < employees.size(); ++i) {
        const auto& employee = employees[i];
        if (employee.full_name == saveEmployee.full_name && employee.employee_id != currentEmployeeId) {
            duplicateIndex = i;
            break;
        }
        if (employee.employee_id == currentEmployeeId) {
            currentEmployeeIndex = i;
        }
    }

    if (duplicateIndex < 0 && currentEmployeeIndex < 0) {
        QSqlQuery query;
        QString insertQuery = QString("INSERT INTO employee(full_name, phone_number, email, employee_group_id) "
                                      "VALUES ('%1', '%2', '%3', %4) RETURNING employee_id")
                                  .arg(saveEmployee.full_name)
                                  .arg(saveEmployee.phone_number)
                                  .arg(saveEmployee.email)
                                  .arg(saveEmployee.employee_group_id);
        if (!query.exec(insertQuery)) {
            QMessageBox::critical(this, "Ошибка в полях", "Поле ФИО или Телефон не уникально.\n Запись не добавлена");
            add();
            return;
        } else {
            if (query.next()) {
                saveEmployee.employee_id = query.value(0).toInt();
                employees.append(saveEmployee);
                QMessageBox::information(this, "Успех", "Запись добавлена");
            } else {
                QMessageBox::critical(this, "Ошибка", "Не удалось получить ID новой записи");
                return;
            }
        }
        currentEmployeeIndex = employees.size() - 1;
    }
    else if (duplicateIndex < 0 && currentEmployeeIndex >= 0)
    {
        QSqlQuery query;
        QString updateQuery = QString("UPDATE employee "
                                      "SET full_name = '%1', "
                                      "phone_number = '%2', "
                                      "email = '%3', "
                                      "employee_group_id = %4 "
                                      "WHERE employee_id = %5;")
                                  .arg(saveEmployee.full_name)
                                  .arg(saveEmployee.phone_number)
                                  .arg(saveEmployee.email)
                                  .arg(saveEmployee.employee_group_id)
                                  .arg(currentEmployeeId);
        if (!query.exec(updateQuery)) {
            QMessageBox::critical(this, "Ошибка в полях", "Поле ФИО или Телефон не уникально.\n Запись не изменена\n" + query.lastError().text());
            display();
            return;
        } else {
            QMessageBox::information(this, "Успех", "Запись изменена");
        }
        employees[currentEmployeeIndex] = saveEmployee;
    }
    else
    {
        QMessageBox::information(this, "Отмена", "Сотрудник с таким ФИО уже существует");
        currentEmployeeIndex = duplicateIndex;
    }
    showEmployee();
}

void EmployeeForm::delete_this() {
    currentEmployeeId = employees[currentEmployeeIndex].employee_id;
    QSqlQuery query;
    QString deleteQuery = QString("DELETE FROM employee "
                                  "WHERE employee_id = %1;")
                              .arg(currentEmployeeId);
    if (!query.exec(deleteQuery)) {
        qDebug() << "Ошибка при удалении записи:" << query.lastError().text();
        QMessageBox::information(this, "Ошибка", "Не удалось удалить запись!");
    } else {
        for (auto it = employees.begin(); it != employees.end(); ++it) {
            if ((*it).employee_id == currentEmployeeId) {
                it = employees.erase(it);
                break;
            }
        }

        ui->leFullName->clear();
        ui->lePhoneNumber->clear();
        ui->leEmail->clear();
        ui->comboBoxGroup->setCurrentIndex(-1);

        if (currentEmployeeIndex > 0) {
            --currentEmployeeIndex;
        } else if (!employees.isEmpty()) {
            ++currentEmployeeIndex;
        }

        display();

        QMessageBox::information(this, "Успех", "Запись удалена!");
    }
}

void EmployeeForm::loadEmployeeGroups() {
    employeeGroups.clear();
    ui->comboBoxGroup->clear();
    QSqlQuery query("SELECT * FROM employee_group");
    while (query.next()) {
        int id = query.value("employee_group_id").toInt();
        QString name = query.value("authority_limitation").toString() + " — " + query.value("group_number").toString();
        employeeGroups.append(qMakePair(id, name));
        ui->comboBoxGroup->addItem(name, id);
    }
}

void EmployeeForm::searchEmployee(const QString &text) {
    searchFilter = text;
    showEmployee();
}

void EmployeeForm::openGoogleMail() {
    const QString& email = ui->leEmail->text();
    if (!email.isEmpty()) {
        QUrl url("https://mail.google.com/mail/?view=cm&fs=1&to=" + email);
        QDesktopServices::openUrl(url);
    } else {
        QMessageBox::warning(this, "Пустой адрес", "Адрес электронной почты сотрудника не указан.");
    }
}

void EmployeeForm::updatePhoneNumberMask(const QString &text) {
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

void EmployeeForm::updateEmailMask(const QString &text) {
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

void EmployeeForm::showEmployeeReport(){
    if (!employeeReport)
    {
        employeeReport = new EmployeeReport();
        connect(employeeReport, &EmployeeReport::finished, this, &EmployeeForm::EmployeeReportClosed);
    }
    employeeReport->exec();
}

void EmployeeForm::EmployeeReportClosed()
{
    // Удаляем виджет
    delete employeeReport;
    employeeReport = nullptr;
}

void EmployeeForm::logout()
{
    this->close();
}
