#include "modalwindow.h"
#include "ui_modalwindow.h"

ModalWindow::ModalWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ModalWindow)
{
    ui->setupUi(this);
    connect(ui->logout, &QPushButton::clicked, this, &ModalWindow::logout);
}

void ModalWindow::logout()
{
    this->close();
}

ModalWindow::~ModalWindow()
{
    delete ui;
}
