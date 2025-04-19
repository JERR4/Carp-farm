QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += sql
QT += charts
QT += webenginewidgets


CONFIG += c++17
LIBS += -L"/opt/homebrew/opt/libpq/lib"

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AdminWindow/adminwindow.cpp \
    ManagerWindow/managerwindow.cpp \
    ModalWindow/modalwindow.cpp \
    QrCodeGenerator/qrcodegen.cpp \
    RegWindow/regwindow.cpp \
    SpecialistWindow/specialistwindow.cpp \
    UserWindow/userwindow.cpp \
    Widgets/AvailableCarpsForUser/availablecarpsforuser.cpp \
    Widgets/CarpForManager/carpformanager.cpp \
    Widgets/CarpForm/carpform.cpp \
    Widgets/CarpInPondForManager/carpinpondformanager.cpp \
    Widgets/CarpInPondForm/carpinpondform.cpp \
    Widgets/CounterpartyForm/counterpartyform.cpp \
    Widgets/CounterpartyReport/counterpartyreport.cpp \
    Widgets/CustomerInfo/customerinfo.cpp \
    Widgets/DiagramPonds/diagrammponds.cpp \
    Widgets/EmployeeForm/employeeform.cpp \
    Widgets/EmployeeGroupForm/employeegroupform.cpp \
    Widgets/EmployeeReport/employeereport.cpp \
    Widgets/FeedingForm/feedingform.cpp \
    Widgets/FeedingReport/feedingreport.cpp \
    Widgets/GistogrammTotalPurchase/gistogrammtotalpurchase.cpp \
    Widgets/LastFeeding/lastfeeding.cpp \
    Widgets/NomenclatureForm/nomenclatureform.cpp \
    Widgets/NomenclatureInStock/nomenclatureinstock.cpp \
    Widgets/PondForm/pondform.cpp \
    Widgets/PondsForUser/pondsforuser.cpp \
    Widgets/PurchaseForm/purchaseform.cpp \
    Widgets/PurchaseReport/purchasereport.cpp \
    Widgets/SaleForm/saleform.cpp \
    Widgets/SaleReport/salereport.cpp \
    Widgets/UserForm/userform.cpp \
    Widgets/UsersOrdersWidgetForUser/usersorderswidgetforuser.cpp \
    Widgets/UsersReport/usersreport.cpp \
    Widgets/employeeGroupsForUser/employeegroupsforuser.cpp \
    main.cpp \
    MainWindow/mainwindow.cpp

HEADERS += \
    AdminWindow/adminwindow.h \
    ManagerWindow/managerwindow.h \
    ModalWindow/modalwindow.h \
    QrCodeGenerator/qrcodegen.h \
    RegWindow/regwindow.h \
    SpecialistWindow/specialistwindow.h \
    UserWindow/userwindow.h \
    Widgets/AvailableCarpsForUser/availablecarpsforuser.h \
    Widgets/CarpForManager/carpformanager.h \
    Widgets/CarpForm/carpform.h \
    Widgets/CarpInPondForManager/carpinpondformanager.h \
    Widgets/CarpInPondForm/carpinpondform.h \
    Widgets/CounterpartyForm/counterpartyform.h \
    Widgets/CounterpartyReport/counterpartyreport.h \
    Widgets/CustomerInfo/customerinfo.h \
    Widgets/DiagramPonds/diagrammponds.h \
    Widgets/EmployeeForm/employeeform.h \
    Widgets/EmployeeGroupForm/employeegroupform.h \
    Widgets/EmployeeReport/employeereport.h \
    Widgets/FeedingForm/feedingform.h \
    Widgets/FeedingReport/feedingreport.h \
    Widgets/GistogrammTotalPurchase/gistogrammtotalpurchase.h \
    Widgets/LastFeeding/lastfeeding.h \
    Widgets/NomenclatureForm/nomenclatureform.h \
    Widgets/NomenclatureInStock/nomenclatureinstock.h \
    Widgets/PondForm/pondform.h \
    Widgets/PondsForUser/pondsforuser.h \
    Widgets/PurchaseForm/purchaseform.h \
    Widgets/PurchaseReport/purchasereport.h \
    Widgets/SaleForm/saleform.h \
    Widgets/SaleReport/salereport.h \
    Widgets/UserForm/userform.h \
    Widgets/UsersOrdersWidgetForUser/usersorderswidgetforuser.h \
    MainWindow/mainwindow.h \
    Widgets/UsersReport/usersreport.h \
    Widgets/employeeGroupsForUser/employeegroupsforuser.h

FORMS += \
    AdminWindow/adminwindow.ui \
    ManagerWindow/managerwindow.ui \
    ModalWindow/modalwindow.ui \
    RegWindow/regwindow.ui \
    SpecialistWindow/specialistwindow.ui \
    UserWindow/userwindow.ui \
    Widgets/AvailableCarpsForUser/availablecarpsforuser.ui \
    Widgets/CarpForManager/carpformanager.ui \
    Widgets/CarpForm/carpform.ui \
    Widgets/CarpInPondForManager/carpinpondformanager.ui \
    Widgets/CarpInPondForm/carpinpondform.ui \
    Widgets/CounterpartyForm/counterpartyform.ui \
    Widgets/CounterpartyReport/counterpartyreport.ui \
    Widgets/CustomerInfo/customerinfo.ui \
    Widgets/DiagramPonds/diagrammponds.ui \
    Widgets/EmployeeForm/employeeform.ui \
    Widgets/EmployeeGroupForm/employeegroupform.ui \
    Widgets/EmployeeReport/employeereport.ui \
    Widgets/FeedingForm/feedingform.ui \
    Widgets/FeedingReport/feedingreport.ui \
    Widgets/GistogrammTotalPurchase/gistogrammtotalpurchase.ui \
    Widgets/LastFeeding/lastfeeding.ui \
    Widgets/NomenclatureForm/nomenclatureform.ui \
    Widgets/NomenclatureInStock/nomenclatureinstock.ui \
    Widgets/PondForm/pondform.ui \
    Widgets/PondsForUser/pondsforuser.ui \
    Widgets/PurchaseForm/purchaseform.ui \
    Widgets/PurchaseReport/purchasereport.ui \
    Widgets/SaleForm/saleform.ui \
    Widgets/SaleReport/salereport.ui \
    Widgets/UserForm/userform.ui \
    Widgets/UsersOrdersWidgetForUser/usersorderswidgetforuser.ui \
    MainWindow/mainwindow.ui \
    Widgets/UsersReport/usersreport.ui \
    Widgets/employeeGroupsForUser/employeegroupsforuser.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
