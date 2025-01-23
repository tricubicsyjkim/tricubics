#ifndef BUTTONCTRLMANAGER_H
#define BUTTONCTRLMANAGER_H

#include <QThread>
#include <QTimer>
#include "ButtonCtrl.h"

#define BUTTON_PIN 1

class ButtonCtrlManager : public QThread
{
    Q_OBJECT

public:
    ButtonCtrlManager();
    ~ButtonCtrlManager();

    bool getBtnStatus();

signals:
    void button_pressed();
    void button_released();

protected:
    void run() override;

private slots:
    void checkButtonStatus();

private:
    ButtonCtrl *btn_ctrl;
    bool btn_status;
    QTimer *timer;
    bool quit = false;
};

#endif // BUTTONCTRLMANAGER_H
