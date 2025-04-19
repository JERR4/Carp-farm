#include "specialistwindow.h"
#include "ui_specialistwindow.h"

SpecialistWindow::SpecialistWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SpecialistWindow),
    pondsWidgetForUser(nullptr),
    feedingFormForSpecialist(nullptr),
    carpInPondForSpecialist(nullptr),
    carpForSpecialist(nullptr),
    nomenclatureInStockForm(nullptr),
    lastFeedngForm(nullptr)
{
    ui->setupUi(this);
    connect(ui->ponds, &QPushButton::clicked, this, &SpecialistWindow::showPonds);
    connect(ui->carp, &QPushButton::clicked, this, &SpecialistWindow::showCarp);
    connect(ui->carp_in_pond, &QPushButton::clicked, this, &SpecialistWindow::showCarpInPond);
    connect(ui->feeding, &QPushButton::clicked, this, &SpecialistWindow::showFeeding);
    connect(ui->logout, &QPushButton::clicked, this, &SpecialistWindow::logout);
    connect(ui->nomenclature_in_stock, &QPushButton::clicked, this, &SpecialistWindow::showNomenclatureInStock);
    connect(ui->last_feeding, &QPushButton::clicked, this, &SpecialistWindow::showLastFeeding);
}

SpecialistWindow::~SpecialistWindow()
{
    delete ui;
}

void SpecialistWindow::showPonds(){
    if (!pondsWidgetForUser)
    {
        pondsWidgetForUser = new pondsforuser();
        connect(pondsWidgetForUser, &pondsforuser::finished, this, &SpecialistWindow::pondsWidgetClosed);
    }
    pondsWidgetForUser->exec();
}

void SpecialistWindow::pondsWidgetClosed()
{
    // Удаляем виджет
    delete pondsWidgetForUser;
    pondsWidgetForUser = nullptr;
}

void SpecialistWindow::showFeeding(){
    if (!feedingFormForSpecialist)
    {
        feedingFormForSpecialist = new FeedingForm();
        connect(feedingFormForSpecialist, &FeedingForm::finished, this, &SpecialistWindow::FeedingFormClosed);
    }
    feedingFormForSpecialist->exec();
}

void SpecialistWindow::FeedingFormClosed()
{
    // Удаляем виджет
    delete feedingFormForSpecialist;
    feedingFormForSpecialist = nullptr;
}

void SpecialistWindow::showCarpInPond(){
    if (!carpInPondForSpecialist)
    {
        carpInPondForSpecialist = new CarpInPondForm();
        connect(carpInPondForSpecialist, &CarpInPondForm::finished, this, &SpecialistWindow::CarpInPondClosed);
    }
    carpInPondForSpecialist->exec();
}

void SpecialistWindow::CarpInPondClosed()
{
    // Удаляем виджет
    delete carpInPondForSpecialist;
    carpInPondForSpecialist = nullptr;
}

void SpecialistWindow::showCarp(){
    if (!carpForSpecialist)
    {
        carpForSpecialist = new CarpForm();
        connect(carpForSpecialist, &CarpForm::finished, this, &SpecialistWindow::CarpClosed);
    }
    carpForSpecialist->exec();

}

void SpecialistWindow::CarpClosed()
{
    // Удаляем виджет
    delete carpForSpecialist;
    carpForSpecialist = nullptr;
}

void SpecialistWindow::showNomenclatureInStock(){
    if (!nomenclatureInStockForm)
    {
        nomenclatureInStockForm = new NomenclatureInStock();
        connect( nomenclatureInStockForm, &NomenclatureInStock::finished, this, &SpecialistWindow::NomenclatureInStockClosed);
    }
    nomenclatureInStockForm->exec();

}

void SpecialistWindow::NomenclatureInStockClosed()
{
    // Удаляем виджет
    delete nomenclatureInStockForm;
    nomenclatureInStockForm = nullptr;
}

void SpecialistWindow::showLastFeeding(){
    if (!lastFeedngForm)
    {
        lastFeedngForm = new LastFeeding();
        connect( lastFeedngForm, &LastFeeding::finished, this, &SpecialistWindow::LastFeedingClosed);
    }
    lastFeedngForm->exec();

}

void SpecialistWindow::LastFeedingClosed()
{
    // Удаляем виджет
    delete lastFeedngForm;
    lastFeedngForm = nullptr;
}

void SpecialistWindow::logout()
{
    this->close();
}
