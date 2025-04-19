#include "managerwindow.h"
#include "ui_managerwindow.h"
#include <QWebEngineView>

ManagerWindow::ManagerWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManagerWindow),
    nomenclature(nullptr),
    counterparty(nullptr),
    purchase(nullptr),
    pondsWidgetForUser(nullptr),
    sale(nullptr),
    carp(nullptr),
    carpinpond(nullptr),
    custominfo(nullptr)
{
    ui->setupUi(this);
    connect(ui->nomenclature, &QPushButton::clicked, this, &ManagerWindow::showNomenclature);
    connect(ui->counterparty, &QPushButton::clicked, this, &ManagerWindow::showCounterparty);
    connect(ui->purchase, &QPushButton::clicked, this, &ManagerWindow::showPurchase);
    connect(ui->sale,  &QPushButton::clicked, this, &ManagerWindow::showSale);
    connect(ui->ponds, &QPushButton::clicked, this, &ManagerWindow::showPonds);
    connect(ui->carps, &QPushButton::clicked, this, &ManagerWindow::showCarp);
    connect(ui->carpinpond, &QPushButton::clicked, this, &ManagerWindow::showCarpInPond);
    connect(ui->custominfo, &QPushButton::clicked, this, &ManagerWindow::showCustomerInfo);
    connect(ui->logout, &QPushButton::clicked, this, &ManagerWindow::logout);
}

ManagerWindow::~ManagerWindow()
{
    delete ui;
}

void ManagerWindow::showNomenclature(){
    if (!nomenclature)
    {
        nomenclature = new NomenclatureForm();
        connect(nomenclature, &NomenclatureForm::finished, this, &ManagerWindow::NomenclatureClosed);
    }
    nomenclature->exec();
}

void ManagerWindow::NomenclatureClosed()
{
    // Удаляем виджет
    delete nomenclature;
    nomenclature = nullptr;
}

void ManagerWindow::showCounterparty(){
    if (!counterparty)
    {
        counterparty = new CounterpartyForm();
        connect(counterparty, &CounterpartyForm::finished, this, &ManagerWindow::CounterpartyClosed);
    }
    counterparty->exec();
}

void ManagerWindow::CounterpartyClosed()
{
    // Удаляем виджет
    delete counterparty;
    counterparty = nullptr;
}

void ManagerWindow::showPurchase(){
    if (!purchase)
    {
        purchase = new PurchaseForm();
        connect(purchase, &PurchaseForm::finished, this, &ManagerWindow::PurchaseClosed);
    }
    purchase->exec();
}

void ManagerWindow::PurchaseClosed()
{
    // Удаляем виджет
    delete purchase;
    purchase = nullptr;
}

void ManagerWindow::showSale(){
    if (!sale)
    {
        sale = new SaleForm();
        connect(sale, &SaleForm::finished, this, &ManagerWindow::SaleClosed);
    }
    sale->exec();
}

void ManagerWindow::SaleClosed()
{
    // Удаляем виджет
    delete sale;
    sale = nullptr;
}

void ManagerWindow::showPonds(){
    if (!pondsWidgetForUser)
    {
        pondsWidgetForUser = new pondsforuser();
        connect(pondsWidgetForUser, &pondsforuser::finished, this, &ManagerWindow::pondsWidgetClosed);
    }

    pondsWidgetForUser->exec();
}

void ManagerWindow::pondsWidgetClosed()
{
    // Удаляем виджет книг
    delete pondsWidgetForUser;
    pondsWidgetForUser = nullptr;
}

void ManagerWindow::showCarp(){
    if (!carp)
    {
        carp = new CarpForManager();
        connect(carp, &CarpForManager::finished, this, &ManagerWindow::CarpClosed);
    }
    carp->exec();
}

void ManagerWindow::CarpClosed()
{
    // Удаляем виджет
    delete carp;
    carp = nullptr;
}

void ManagerWindow::showCarpInPond(){
    if (!carpinpond)
    {
        carpinpond = new CarpInPondForManager();
        connect(carpinpond, &CarpInPondForManager::finished, this, &ManagerWindow::CarpInPondClosed);
    }
    carpinpond->exec();
}

void ManagerWindow::CarpInPondClosed()
{
    // Удаляем виджет
    delete carpinpond;
    carpinpond = nullptr;
}

void ManagerWindow::showCustomerInfo(){
    if (!custominfo)
    {
        custominfo = new CustomerInfo();
        connect(custominfo, &CustomerInfo::finished, this, &ManagerWindow::CustomerInfoClosed);
    }
    custominfo->exec();
}

void ManagerWindow::CustomerInfoClosed()
{
    // Удаляем виджет
    delete custominfo;
    custominfo = nullptr;
}

void ManagerWindow::logout()
{
    this->close();
}
