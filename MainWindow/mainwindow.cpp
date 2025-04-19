#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    modWin(nullptr),
    regWin(nullptr)
{
    ui->setupUi(this);
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
    // Подключение сигнала к кнопке входа
    connect(ui->loginButton, &QPushButton::clicked, this, &MainWindow::loginButtonClicked);
    connect(ui->dev, &QPushButton::clicked, this, &MainWindow::showModalWindow);
    connect(ui->registration, &QPushButton::clicked, this, &MainWindow::showRegWindow);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getLogin() const {
    QString login = ui->loginEdit->text(); // Сохранение значения поля ввода логина
    ui->loginEdit->clear(); // Очистка поля ввода логина
    return login; // Возвращение сохраненного значения
}

QString MainWindow::getPassword() const {
    QString password = ui->passwordEdit->text(); // Сохранение значения поля ввода пароля
    ui->passwordEdit->clear(); // Очистка поля ввода пароля
    return password; // Возвращение сохраненного значения
}

void MainWindow::showModalWindow(){
    if (!modWin)
    {
        modWin = new ModalWindow();
        connect(modWin, &ModalWindow::finished, this, &MainWindow::closeModalWindow);
    }
    modWin->exec();
}

void MainWindow::closeModalWindow()
{
    // Удаляем виджет
    delete modWin;
    modWin = nullptr;
}

void MainWindow::showRegWindow(){
    if (!regWin)
    {
        regWin = new RegWindow();
        connect(regWin, &ModalWindow::finished, this, &MainWindow::closeRegWindow);
    }
    regWin->exec();
}

void MainWindow::closeRegWindow()
{
    // Удаляем виджет
    delete regWin;
    regWin = nullptr;
}

