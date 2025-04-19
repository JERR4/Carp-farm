#include "userform.h"
#include "ui_userform.h"
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>

UserForm::UserForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserForm),
    usersReportForm(nullptr)
{
    ui->setupUi(this);
    connect(ui->next, &QPushButton::clicked, this, &UserForm::next);
    connect(ui->previous, &QPushButton::clicked, this, &UserForm::previous);
    connect(ui->add, &QPushButton::clicked, this, &UserForm::add);
    connect(ui->save, &QPushButton::clicked, this, &UserForm::save);
    connect(ui->cancel, &QPushButton::clicked, this, &UserForm::display);
    connect(ui->delete_2, &QPushButton::clicked, this, &UserForm::delete_this);
    connect(ui->lineEditSearch, &QLineEdit::textChanged, this, &UserForm::searchUser);
    connect(ui->logout, &QPushButton::clicked, this, &UserForm::logout);
    connect(ui->users_report, &QPushButton::clicked, this, &UserForm::showUsersReport); // Соединение кнопки с вызовом отчета о пользователях
    showUser();
    loadUserRoles();
}

UserForm::~UserForm()
{
    delete ui;
}

void UserForm::showUser() {
    db = QSqlDatabase::database();
    if (!db.isValid()) {
        qDebug() << "Ошибка: не удалось установить соединение с базой данных.";
        return;
    }

    QSqlQuery query;
    QString queryString = "SELECT * FROM users";
    if (!searchFilter.isEmpty()) {
        queryString += " WHERE user_login LIKE '" + searchFilter + "%'";
        currentUserIndex = 0;
    }
    queryString += " ORDER BY user_id";
    if (!query.exec(queryString)) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        return;
    }

    users.clear();
    while (query.next()) {
        User user;
        user.user_id = query.value("user_id").toInt();
        user.user_role = query.value("user_role").toString();
        user.user_login = query.value("user_login").toString();
        user.user_pass = query.value("user_pass").toString();
        users.append(user);
    }
    if (users.size() == 0) {
        QMessageBox::critical(this, "Ошибка", "Ничего не найдено");
        ui->lineEditSearch->clear();
    }
    display();
}

void UserForm::next() {
    if (currentUserIndex < users.size() - 1) {
        ++currentUserIndex;
        display();
    }
}

void UserForm::previous() {
    if (currentUserIndex > 0) {
        --currentUserIndex;
        display();
    }
}

void UserForm::add() {
    ui->next->hide();
    ui->previous->hide();
    ui->add->hide();
    ui->logout->hide();
    ui->delete_2->hide();
    ui->label_3->hide();
    ui->cancel->show();
    ui->label_4->show();
    ui->lineEditSearch->hide();
    ui->leUserRole->clear();
    ui->leUserLogin->clear();
    ui->leUserPass->clear();
    currentUserIndex = -1;
}

void UserForm::display() {
    if (currentUserIndex == -1)
        currentUserIndex = 0;
    ui->lineEditSearch->show();
    ui->cancel->hide();
    ui->label_4->hide();
    ui->next->show();
    ui->logout->show();
    ui->previous->show();
    ui->add->show();
    ui->delete_2->show();
    ui->label_3->show();

    const User& currentUser = users[currentUserIndex];

    ui->leUserRole->setCurrentText(currentUser.user_role);
    ui->leUserLogin->setText(currentUser.user_login);
    ui->leUserPass->setText(currentUser.user_pass);
}

void UserForm::save() {
    QString errorMessage;

    // Проверка на пустые поля
    if (ui->leUserRole->currentText().isEmpty()) {
        errorMessage += "Поле 'Роль пользователя' не может быть пустым.\n";
    }
    if (ui->leUserLogin->text().isEmpty()) {
        errorMessage += "Поле 'Логин пользователя' не может быть пустым.\n";
    }
    if (ui->leUserPass->text().isEmpty()) {
        errorMessage += "Поле 'Пароль пользователя' не может быть пустым.\n";
    }

    // Если есть пустые поля, выводим сообщение об ошибке и прерываем операцию
    if (!errorMessage.isEmpty()) {
        QMessageBox::critical(this, "Ошибка в полях", errorMessage);
        if (currentUserIndex < 0)
            add();
        return;
    }

    if(currentUserIndex >= 0) {
        currentUserID = users[currentUserIndex].user_id;
    }
    else
        currentUserID  = -1;

    User saveUser;

    saveUser.user_role = ui->leUserRole->currentText();
    saveUser.user_login = ui->leUserLogin->text();
    saveUser.user_pass = ui->leUserPass->text();

    int tempIndex = currentUserIndex;
    ui->lineEditSearch->clear();
    showUser();
    if (tempIndex < 0)
        currentUserIndex = -1;

    int duplicateIndex = -1;
    for (int i = 0; i < users.size(); ++i) {
        const auto& user = users[i];
        if (user.user_login == saveUser.user_login && user.user_id != currentUserID) {
            duplicateIndex = i;
            break;
        }
        if (user.user_id == currentUserID) {
            currentUserIndex = i;
        }
    }

    if (duplicateIndex < 0 && currentUserIndex < 0) {
        QSqlQuery query;
        QString insertQuery = QString("INSERT INTO users(user_role, user_login, user_pass) "
                                      "VALUES ('%1', '%2', '%3')")
                                  .arg(saveUser.user_role)
                                  .arg(saveUser.user_login)
                                  .arg(saveUser.user_pass);
        if (!query.exec(insertQuery)) {
            QMessageBox::critical(this, "Ошибка в полях", "Ошибка в полях. Запись не добавлена");
            add();
            return;
        } else {
            QMessageBox::information(this, "Успех", "Запись добавлена");
            showUser();
        }
    }
    else if (duplicateIndex < 0 && currentUserIndex >= 0)
    {
        QSqlQuery query;
        QString updateQuery = QString("UPDATE users "
                                      "SET user_role = '%1', "
                                      "user_login = '%2', "
                                      "user_pass = '%3' "
                                      "WHERE user_id = %4;")
                                  .arg(saveUser.user_role)
                                  .arg(saveUser.user_login)
                                  .arg(saveUser.user_pass)
                                  .arg(currentUserID);
        if (!query.exec(updateQuery)) {
            QMessageBox::critical(this, "Ошибка в полях", "Ошибка в полях. Запись не изменена\n" + query.lastError().text());
            return;
        } else {
            QMessageBox::information(this, "Успех", "Запись изменена");
        }
        users[currentUserIndex].user_role = saveUser.user_role;
        users[currentUserIndex].user_login = saveUser.user_login;
        users[currentUserIndex].user_pass = saveUser.user_pass;
    }
    else
    {
        QMessageBox::information(this, "Отмена", "Пользователь с таким логином уже существует");
        currentUserIndex = duplicateIndex;
    }
    showUser();
}

void UserForm::delete_this() {
    currentUserID = users[currentUserIndex].user_id;
    QSqlQuery query;
    QString deleteQuery = QString("DELETE FROM users "
                                  "WHERE user_id = %1;")
                              .arg(currentUserID);
    if (!query.exec(deleteQuery)) {
        qDebug() << "Ошибка при удалении записи:" << query.lastError().text();
        QMessageBox::information(this, "Ошибка", "Не удалось удалить запись!");
    } else {
        for (auto it = users.begin(); it != users.end(); ++it) {
            if ((*it).user_id == currentUserID) {
                it = users.erase(it);
                break;
            }
        }

        ui->leUserRole->clear();
        ui->leUserLogin->clear();
        ui->leUserPass->clear();

        if (currentUserIndex > 0) {
            --currentUserIndex;
        } else if (!users.isEmpty()) {
            ++currentUserIndex;
        }

        display();

        QMessageBox::information(this, "Успех", "Запись удалена!");
    }
}

void UserForm::loadUserRoles() {
    userRoles.clear();
    ui->leUserRole->clear();

    // Add the specified user roles
    userRoles << "Покупатель" << "Менеджер по закупкам и продажам" << "Администратор" << "Специалист по уходу за рыбой и водоёмом";

    ui->leUserRole->addItems(userRoles);
}


void UserForm::searchUser(const QString &text) {
    searchFilter = text;
    showUser();
}

// Определения слота для отображения отчета о пользователях
void UserForm::showUsersReport()
{
    if (!usersReportForm)
    {
        usersReportForm = new UsersReport(); // Создание объекта класса UsersReport
        connect(usersReportForm, &UsersReport::finished, this, &UserForm::UsersReportClosed); // Соединение сигнала finished от UsersReport с слотом закрытия окна
    }
    usersReportForm->exec(); // Отображение окна
}

// Определение слота для закрытия отчета о пользователях
void UserForm::UsersReportClosed()
{
    // Удаление объекта класса UsersReport
    delete usersReportForm;
    usersReportForm = nullptr;
}

void UserForm::logout() {
    this->close();
}
