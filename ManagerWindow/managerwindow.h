#ifndef MANAGERWINDOW_H
#define MANAGERWINDOW_H

#include <QDialog>
#include "../Widgets/NomenclatureForm/nomenclatureform.h"
#include "../Widgets/CounterpartyForm/counterpartyform.h"
#include "../Widgets/PurchaseForm/purchaseform.h"
#include "../Widgets/SaleForm/saleform.h"
#include "../Widgets/PondsForUser/pondsforuser.h"
#include "../Widgets/CarpForManager/carpformanager.h"
#include "../Widgets/CarpInPondForManager/carpinpondformanager.h"
#include "../Widgets/CustomerInfo/customerinfo.h"

namespace Ui {
class ManagerWindow;
}

class ManagerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ManagerWindow(QWidget *parent = nullptr);
    ~ManagerWindow();

private slots:
    void showNomenclature();
    void showCounterparty();
    void showPurchase();
    void showSale();
    void showPonds();
    void showCarp();
    void showCarpInPond();
    void showCustomerInfo();
    void NomenclatureClosed();
    void CounterpartyClosed();
    void PurchaseClosed();
    void SaleClosed();
    void pondsWidgetClosed(); // Добавляем декларацию слота для обработки закрытия
    void CarpClosed();
    void CarpInPondClosed();
    void CustomerInfoClosed();
    void logout();
private:
    Ui::ManagerWindow *ui;
    NomenclatureForm *nomenclature;
    CounterpartyForm *counterparty;
    PurchaseForm *purchase;
    SaleForm *sale;
    pondsforuser *pondsWidgetForUser;
    CarpForManager *carp;
    CarpInPondForManager *carpinpond;
    CustomerInfo* custominfo;
};

#endif // MANAGERWINDOW_H
