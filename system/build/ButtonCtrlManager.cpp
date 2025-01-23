#include "ButtonCtrlManager.h"

ButtonCtrlManager::ButtonCtrlManager()
{
    btn_ctrl = new ButtonCtrl(BUTTON_PIN);
    btn_status = btn_ctrl->isPressed();
}

ButtonCtrlManager::~ButtonCtrlManager()
{
    quit = true;
    if (timer) {
        timer->stop();
        timer->deleteLater();
    }
    wait();
    delete btn_ctrl;
}

void ButtonCtrlManager::run()
{
    qDebug() << "button thread start";

    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &ButtonCtrlManager::checkButtonStatus);
    timer->start(500);

    exec();
}

void ButtonCtrlManager::checkButtonStatus()
{
    bool isPressed = btn_ctrl->isPressed();
    if (isPressed != btn_status) {
        btn_status = isPressed;
        qDebug() << "manager isPressed: " << isPressed;
        if (isPressed) {
            qDebug()<<"button pressed to main";
            emit button_pressed();
        } else {
            emit button_released();
        }
    }
}

bool ButtonCtrlManager::getBtnStatus()
{
    return btn_ctrl->isPressed();
}
