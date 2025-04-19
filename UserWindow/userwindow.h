#ifndef USERWINDOW_H
#define USERWINDOW_H

#include <QDialog>
#include "../Widgets/availablecarpsforuser/availablecarpsforuser.h"
#include "../Widgets/UsersOrdersWidgetForUser/usersorderswidgetforuser.h"
#include "../Widgets/PondsForUser/pondsforuser.h"

namespace Ui {
class UserWindow;
}

class UserWindow : public QDialog
{
    Q_OBJECT

public:
    explicit UserWindow(QString login_param, QWidget *parent = nullptr);
    ~UserWindow();

private slots:
    void showCarpsInStock();
    void showOrders();
    void showPonds();
    void carpsWidgetClosed(); // Добавляем декларацию слота для обработки закрытия
    void ordersWidgetClosed(); // Добавляем декларацию слота для обработки закрытия
    void pondsWidgetClosed(); // Добавляем декларацию слота для обработки закрытия
    void logout();

private:
    Ui::UserWindow *ui;
    availablecarpsforuser *carpsWidgetForUser; // Добавляем указатель на виджет карпов
    UsersOrdersWidgetForUser *ordersWidgetForUser;
    pondsforuser *pondsWidgetForUser;
    QString login;
};

#endif // USERWINDOW_H
