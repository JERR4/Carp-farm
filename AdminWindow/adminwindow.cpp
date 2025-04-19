#include "adminwindow.h"
#include "ui_adminwindow.h"

AdminWindow::AdminWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminWindow),
    nomenclature(nullptr),
    counterparty(nullptr),
    sale(nullptr),
    purchase(nullptr),
    feedingFormForSpecialist(nullptr),
    carpInPondForSpecialist(nullptr),
    carpForSpecialist(nullptr),
    employeeGroup(nullptr),
    employee(nullptr),
    pond(nullptr),
    user(nullptr)
{
    ui->setupUi(this);
    connect(ui->nomenclature, &QPushButton::clicked, this, &AdminWindow::showNomenclature);
    connect(ui->counterparty, &QPushButton::clicked, this, &AdminWindow::showCounterparty);
    connect(ui->purchase, &QPushButton::clicked, this, &AdminWindow::showPurchase);
    connect(ui->sale,  &QPushButton::clicked, this, &AdminWindow::showSale);
    connect(ui->carp, &QPushButton::clicked, this, &AdminWindow::showCarp);
    connect(ui->carp_in_pond, &QPushButton::clicked, this, &AdminWindow::showCarpInPond);
    connect(ui->feeding, &QPushButton::clicked, this, &AdminWindow::showFeeding);
    connect(ui->pond, &QPushButton::clicked, this, &AdminWindow::showPond);
    connect(ui->employeeGroup, &QPushButton::clicked, this, &AdminWindow::showEmployeeGroup);
    connect(ui->employee, &QPushButton::clicked, this, &AdminWindow::showEmployee);
    connect(ui->user, &QPushButton::clicked, this, &AdminWindow::showUser);
    connect(ui->logout, &QPushButton::clicked, this, &AdminWindow::logout);
}

AdminWindow::~AdminWindow()
{
    delete ui;
}

void AdminWindow::showNomenclature(){
    if (!nomenclature)
    {
        nomenclature = new NomenclatureForm();
        connect(nomenclature, &NomenclatureForm::finished, this, &AdminWindow::NomenclatureClosed);
    }
    nomenclature->exec();
}

void AdminWindow::NomenclatureClosed()
{
    // Удаляем виджет
    delete nomenclature;
    nomenclature = nullptr;
}

void AdminWindow::showCounterparty(){
    if (!counterparty)
    {
        counterparty = new CounterpartyForm();
        connect(counterparty, &CounterpartyForm::finished, this, &AdminWindow::CounterpartyClosed);
    }
    counterparty->exec();
}

void AdminWindow::CounterpartyClosed()
{
    // Удаляем виджет
    delete counterparty;
    counterparty = nullptr;
}

void AdminWindow::showPurchase(){
    if (!purchase)
    {
        purchase = new PurchaseForm();
        connect(purchase, &PurchaseForm::finished, this, &AdminWindow::PurchaseClosed);
    }
    purchase->exec();
}

void AdminWindow::PurchaseClosed()
{
    // Удаляем виджет
    delete purchase;
    purchase = nullptr;
}

void AdminWindow::showSale(){
    if (!sale)
    {
        sale = new SaleForm();
        connect(sale, &SaleForm::finished, this, &AdminWindow::SaleClosed);
    }
    sale->exec();
}

void AdminWindow::SaleClosed()
{
    // Удаляем виджет
    delete sale;
    sale = nullptr;
}

void AdminWindow::showFeeding(){
    if (!feedingFormForSpecialist)
    {
        feedingFormForSpecialist = new FeedingForm();
        connect(feedingFormForSpecialist, &FeedingForm::finished, this, &AdminWindow::FeedingFormClosed);
    }
    feedingFormForSpecialist->exec();
}

void AdminWindow::FeedingFormClosed()
{
    // Удаляем виджет
    delete feedingFormForSpecialist;
    feedingFormForSpecialist = nullptr;
}

void AdminWindow::showCarpInPond(){
    if (!carpInPondForSpecialist)
    {
        carpInPondForSpecialist = new CarpInPondForm();
        connect(carpInPondForSpecialist, &CarpInPondForm::finished, this, &AdminWindow::CarpInPondClosed);
    }
    carpInPondForSpecialist->exec();
}

void AdminWindow::CarpInPondClosed()
{
    // Удаляем виджет
    delete carpInPondForSpecialist;
    carpInPondForSpecialist = nullptr;
}

void AdminWindow::showCarp(){
    if (!carpForSpecialist)
    {
        carpForSpecialist = new CarpForm();
        connect(carpForSpecialist, &CarpForm::finished, this, &AdminWindow::CarpClosed);
    }
    carpForSpecialist->exec();

}

void AdminWindow::CarpClosed()
{
    // Удаляем виджет
    delete carpForSpecialist;
    carpForSpecialist = nullptr;
}

void AdminWindow::showPond(){
    if (!pond)
    {
        pond = new PondForm();
        connect(pond, &PondForm::finished, this, &AdminWindow::PondClosed);
    }
    pond->exec();
}

void AdminWindow::PondClosed()
{
    // Удаляем виджет
    delete pond;
    pond = nullptr;
}

void AdminWindow::showEmployeeGroup(){
    if (!employeeGroup)
    {
        employeeGroup = new EmployeeGroupForm();
        connect(employeeGroup, &EmployeeGroupForm::finished, this, &AdminWindow::EmployeeGroupFormClosed);
    }
    employeeGroup->exec();
}

void AdminWindow::EmployeeGroupFormClosed()
{
    // Удаляем виджет
    delete employeeGroup;
    employeeGroup = nullptr;
}

void AdminWindow::showEmployee(){
    if (!employee)
    {
        employee = new EmployeeForm();
        connect(employee, &EmployeeForm::finished, this, &AdminWindow::EmployeeFormClosed);
    }
    employee->exec();
}

void AdminWindow::EmployeeFormClosed()
{
    // Удаляем виджет
    delete employee;
    employee = nullptr;
}

void AdminWindow::showUser(){
    if (!user)
    {
        user = new UserForm();
        connect(user, &EmployeeForm::finished, this, &AdminWindow::UserFormClosed);
    }
    user->exec();
}

void AdminWindow::UserFormClosed()
{
    // Удаляем виджет
    delete user;
    user = nullptr;
}

void AdminWindow::logout()
{
    this->close();
}
