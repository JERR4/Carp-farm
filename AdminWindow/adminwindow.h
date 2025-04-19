#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QDialog>
#include "../Widgets/NomenclatureForm/nomenclatureform.h"
#include "../Widgets/CounterpartyForm/counterpartyform.h"
#include "../Widgets/PurchaseForm/purchaseform.h"
#include "../Widgets/SaleForm/saleform.h"
#include "../Widgets/FeedingForm/feedingform.h"
#include "../Widgets/CarpInPondForm/carpinpondform.h"
#include "../Widgets/CarpForm/carpform.h"
#include "../Widgets/PondForm/pondform.h"
#include "../Widgets/EmployeeGroupForm/employeegroupform.h"
#include "../Widgets/EmployeeForm/employeeform.h"
#include "../Widgets/UserForm/userform.h"

namespace Ui {
class AdminWindow;
}

class AdminWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AdminWindow(QWidget *parent = nullptr);
    ~AdminWindow();
private slots:
    void showNomenclature();
    void showCounterparty();
    void showPurchase();
    void showSale();
    void showFeeding();
    void showCarpInPond();
    void showCarp();
    void showPond();
    void showEmployeeGroup();
    void showEmployee();
    void showUser();
    void NomenclatureClosed();
    void CounterpartyClosed();
    void PurchaseClosed();
    void SaleClosed();
    void FeedingFormClosed();
    void CarpInPondClosed();
    void CarpClosed();
    void PondClosed();
    void EmployeeGroupFormClosed();
    void EmployeeFormClosed();
    void UserFormClosed();
    void logout();
private:
    Ui::AdminWindow *ui;
    NomenclatureForm *nomenclature;
    CounterpartyForm *counterparty;
    PurchaseForm *purchase;
    SaleForm *sale;
    FeedingForm *feedingFormForSpecialist;
    CarpInPondForm *carpInPondForSpecialist;
    CarpForm *carpForSpecialist;
    PondForm* pond;
    EmployeeGroupForm* employeeGroup;
    EmployeeForm* employee;
    UserForm* user;
};

#endif // ADMINWINDOW_H
