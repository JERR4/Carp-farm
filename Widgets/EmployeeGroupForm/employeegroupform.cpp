#include "employeegroupform.h"
#include "ui_employeegroupform.h"
#include <QSqlError>
#include <QIntValidator>

EmployeeGroupForm::EmployeeGroupForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EmployeeGroupForm),
    employeegr(nullptr)
{
    ui->setupUi(this);
    connect(ui->next, &QPushButton::clicked, this, &EmployeeGroupForm::next);
    connect(ui->previous, &QPushButton::clicked, this, &EmployeeGroupForm::previous);
    connect(ui->add, &QPushButton::clicked, this, &EmployeeGroupForm::add);
    connect(ui->save, &QPushButton::clicked, this, &EmployeeGroupForm::save);
    connect(ui->cancel, &QPushButton::clicked, this, &EmployeeGroupForm::display);
    connect(ui->delete_2, &QPushButton::clicked, this, &EmployeeGroupForm::deleteThis);
    connect(ui->lineEditSearch, &QLineEdit::textChanged, this, &EmployeeGroupForm::searchEmployeeGroup);
    connect(ui->employeegroup, &QPushButton::clicked, this, &EmployeeGroupForm::showEmployeeGroupForUser);
    connect(ui->logout, &QPushButton::clicked, this, &EmployeeGroupForm::logout);
    showEmployeeGroup();
    loadAuthorityLimitations();
}

EmployeeGroupForm::~EmployeeGroupForm()
{
    delete ui;
}

void EmployeeGroupForm::showEmployeeGroup() {
    db = QSqlDatabase::database();
    if (!db.isValid()) {
        qDebug() << "Ошибка: не удалось установить соединение с базой данных.";
        return;
    }

    QSqlQuery query;
    QString queryString = "SELECT * FROM employee_group";
    if (!searchFilter.isEmpty()) {
        queryString += " WHERE authority_limitation LIKE '" + searchFilter + "%'";
        currentEmployeeGroupIndex = 0;
    }
    queryString += " ORDER BY employee_group_id";
    if (!query.exec(queryString)) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        return;
    }

    employeeGroups.clear();
    while (query.next()) {
        EmployeeGroup employeeGroup;
        employeeGroup.employee_group_id = query.value("employee_group_id").toInt();
        employeeGroup.authority_limitation = query.value("authority_limitation").toString();
        employeeGroup.group_number = query.value("group_number").toString();
        employeeGroups.append(employeeGroup);
    }
    if (employeeGroups.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Ничего не найдено");
        ui->lineEditSearch->clear();
    }
    display();
}

void EmployeeGroupForm::next() {
    if (currentEmployeeGroupIndex < employeeGroups.size() - 1) {
        ++currentEmployeeGroupIndex;
        display();
    }
}

void EmployeeGroupForm::previous() {
    if (currentEmployeeGroupIndex > 0) {
        --currentEmployeeGroupIndex;
        display();
    }
}

void EmployeeGroupForm::add() {
    ui->next->hide();
    ui->previous->hide();
    ui->add->hide();
    ui->logout->hide();
    ui->delete_2->hide();
    ui->label_3->hide();
    ui->cancel->show();
    ui->label_4->show();
    ui->lineEditSearch->hide();
    ui->lineEditGroupNumber->clear();
    ui->comboBoxAuthorityLimitation->setCurrentIndex(-1);
    currentEmployeeGroupIndex = -1;
}

void EmployeeGroupForm::display() {
    if (currentEmployeeGroupIndex == -1)
        currentEmployeeGroupIndex = 0;
    ui->lineEditSearch->show();
    ui->cancel->hide();
    ui->label_4->hide();
    ui->next->show();
    ui->logout->show();
    ui->previous->show();
    ui->add->show();
    ui->delete_2->show();
    ui->label_3->show();

    const EmployeeGroup& currentEmployeeGroup = employeeGroups[currentEmployeeGroupIndex];

    ui->lineEditGroupNumber->setText(currentEmployeeGroup.group_number);
    ui->comboBoxAuthorityLimitation->setCurrentText(currentEmployeeGroup.authority_limitation);
}

void EmployeeGroupForm::save() {
    QString errorMessage;

    // Проверка на пустые поля
    if (ui->lineEditGroupNumber->text().isEmpty()) {
        errorMessage += "Поле 'Номер группы' не может быть пустым.\n";
    }
    if (ui->comboBoxAuthorityLimitation->currentText().isEmpty()) {
        errorMessage += "Поле 'Ограничение полномочий' не может быть пустым.\n";
    }

    // Если есть пустые поля, выводим сообщение об ошибке и прерываем операцию
    if (!errorMessage.isEmpty()) {
        QMessageBox::critical(this, "Ошибка в полях", errorMessage);
        if (currentEmployeeGroupIndex < 0)
            add();
        return;
    }

    if(currentEmployeeGroupIndex >= 0) {
        currentEmployeeGroupId = employeeGroups[currentEmployeeGroupIndex].employee_group_id;
    }
    else
        currentEmployeeGroupId= -1;

    EmployeeGroup saveEmployeeGroup;
    saveEmployeeGroup.group_number = ui->lineEditGroupNumber->text();
    saveEmployeeGroup.authority_limitation = ui->comboBoxAuthorityLimitation->currentText();
    ui->lineEditSearch->clear();

    int tempIndex = currentEmployeeGroupIndex;
    showEmployeeGroup();
    if (tempIndex < 0)
        currentEmployeeGroupIndex = -1;

    int duplicateIndex = -1;
    for (int i = 0; i < employeeGroups.size(); ++i) {
        const auto& employeeGroup = employeeGroups[i];
        if (employeeGroup.group_number == saveEmployeeGroup.group_number && employeeGroup.employee_group_id != currentEmployeeGroupId) {
            duplicateIndex = i;
            break;
        }
        if (employeeGroup.employee_group_id == currentEmployeeGroupId) {
            currentEmployeeGroupIndex = i;
        }
    }

    if (duplicateIndex < 0 && currentEmployeeGroupIndex < 0) {
        QSqlQuery query;
        QString insertQuery = QString("INSERT INTO employee_group(authority_limitation, group_number) "
                                      "VALUES ('%1', '%2') RETURNING employee_group_id")
                                  .arg(saveEmployeeGroup.authority_limitation)
                                  .arg(saveEmployeeGroup.group_number);
        if (!query.exec(insertQuery)) {
            QMessageBox::critical(this, "Ошибка в полях", "Ошибка в полях. Запись не добавлена");
            add();
            return;
        } else {
            if (query.next()) {
                saveEmployeeGroup.employee_group_id = query.value(0).toInt();
                employeeGroups.append(saveEmployeeGroup);
                QMessageBox::information(this, "Успех", "Запись добавлена");
            }
            else {
                QMessageBox::critical(this, "Ошибка", "Не удалось получить ID новой записи");
                return;
            }
        }
        currentEmployeeGroupIndex = employeeGroups.size() - 1;
    }
    else if (duplicateIndex < 0 && currentEmployeeGroupIndex >= 0)
    {
        QSqlQuery query;
        QString updateQuery = QString("UPDATE employee_group "
                                      "SET authority_limitation = '%1', "
                                      "group_number = '%2' "
                                      "WHERE employee_group_id = %3;")
                                  .arg(saveEmployeeGroup.authority_limitation)
                                  .arg(saveEmployeeGroup.group_number)
                                  .arg(currentEmployeeGroupId);
        if (!query.exec(updateQuery)) {
            QMessageBox::critical(this, "Ошибка в полях", "Ошибка в полях. Запись не изменена\n" + query.lastError().text());
            return;
        } else {
            QMessageBox::information(this, "Успех", "Запись изменена");
        }
        employeeGroups[currentEmployeeGroupIndex].group_number = saveEmployeeGroup.group_number;
        employeeGroups[currentEmployeeGroupIndex].authority_limitation = saveEmployeeGroup.authority_limitation;
    }
    else
    {
        QMessageBox::information(this, "Отмена", "Такая запись уже существует");
        currentEmployeeGroupIndex = duplicateIndex;
    }
    showEmployeeGroup();
}

void EmployeeGroupForm::deleteThis() {
    currentEmployeeGroupId = employeeGroups[currentEmployeeGroupIndex].employee_group_id;
    QSqlQuery query;
    QString deleteQuery = QString("DELETE FROM employee_group "
                                  "WHERE employee_group_id = %1;")
                              .arg(currentEmployeeGroupId);
    if (!query.exec(deleteQuery)) {
        qDebug() << "Ошибка при удалении записи:" << query.lastError().text();
        QMessageBox::information(this, "Ошибка", "Не удалось удалить запись!");
    } else {
        for (auto it = employeeGroups.begin(); it != employeeGroups.end(); ++it) {
            if ((*it).employee_group_id == currentEmployeeGroupId) {
                it = employeeGroups.erase(it);
                break;
            }
        }

        ui->lineEditGroupNumber->clear();
        ui->comboBoxAuthorityLimitation->setCurrentIndex(-1);

        if (currentEmployeeGroupIndex > 0) {
            --currentEmployeeGroupIndex;
        } else if (!employeeGroups.isEmpty()) {
            ++currentEmployeeGroupIndex;
        }

        display();

        QMessageBox::information(this, "Успех", "Запись удалена!");
    }
}

void EmployeeGroupForm::loadAuthorityLimitations() {
    authorityLimitations.clear();
    ui->comboBoxAuthorityLimitation->clear();

    // Add the specified authority limitations
    authorityLimitations << "Администратор" << "Менеджер по закупкам и продажам" << "Специалист по уходу за рыбой и водоёмом";

    ui->comboBoxAuthorityLimitation->addItems(authorityLimitations);
}

void EmployeeGroupForm::searchEmployeeGroup(const QString &text) {
    searchFilter = text;
    showEmployeeGroup();
}

void EmployeeGroupForm::showEmployeeGroupForUser(){
    if (!employeegr)
    {
        employeegr= new employeeGroupsForUser();
        connect(employeegr, &employeeGroupsForUser::finished, this, &EmployeeGroupForm::EmployeeGroupForUserClosed);
    }
    employeegr->exec();
}

void EmployeeGroupForm::EmployeeGroupForUserClosed()
{
    // Удаляем виджет
    delete employeegr;
    employeegr = nullptr;
}

void EmployeeGroupForm::logout()
{
    this->close();
}
