#ifndef SPECIALISTWINDOW_H
#define SPECIALISTWINDOW_H

#include <QDialog>
#include "../Widgets/PondsForUser/pondsforuser.h"
#include "../Widgets/FeedingForm/feedingform.h"
#include "../Widgets/CarpInPondForm/carpinpondform.h"
#include "../Widgets/CarpForm/carpform.h"
#include "../Widgets/NomenclatureInStock/nomenclatureinstock.h"
#include "../Widgets/LastFeeding/lastfeeding.h"

namespace Ui {
class SpecialistWindow;
}

class SpecialistWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SpecialistWindow(QWidget *parent = nullptr);
    ~SpecialistWindow();

private slots:
    void showPonds();
    void showFeeding();
    void showCarpInPond();
    void showCarp();
    void showNomenclatureInStock();
    void showLastFeeding();
    void pondsWidgetClosed();
    void FeedingFormClosed();
    void CarpInPondClosed();
    void CarpClosed();
    void NomenclatureInStockClosed();
    void LastFeedingClosed();
    void logout();

private:
    Ui::SpecialistWindow *ui;
    pondsforuser *pondsWidgetForUser;
    FeedingForm *feedingFormForSpecialist;
    CarpInPondForm *carpInPondForSpecialist;
    CarpForm *carpForSpecialist;
    NomenclatureInStock *nomenclatureInStockForm;
    LastFeeding *lastFeedngForm;
};

#endif // SPECIALISTWINDOW_H
