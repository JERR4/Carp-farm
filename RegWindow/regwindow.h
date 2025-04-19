#ifndef REGWINDOW_H
#define REGWINDOW_H

#include <QDialog>

namespace Ui {
class RegWindow;
}

class RegWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RegWindow(QWidget *parent = nullptr);
    ~RegWindow();

private slots:
    void registerUser(); // Обработчик нажатия кнопки регистрации

private:
    Ui::RegWindow *ui;
    void logout();
};

#endif // REGWINDOW_H
