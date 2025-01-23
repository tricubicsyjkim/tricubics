#include "DoorCtrlManager.h"

#define DOOR_PIN 5

DoorCtrlManager::DoorCtrlManager(QObject *parent) : QThread(parent), timer(nullptr) {
    door_ctrl = new DoorCtrl(DOOR_PIN);
    door_ctrl->initDoorClosed();
    door_status = door_ctrl->isOpened();
    qDebug() << "door init status: " << door_status;
    connect(&opened_door_timer_, SIGNAL(timeout()), this, SLOT(on_opened_door_timeout()));
}

DoorCtrlManager::~DoorCtrlManager() {
    quit = true;
    if (timer) {
        timer->stop();
        timer->deleteLater();
    }
    wait();
    delete door_ctrl;
}

bool DoorCtrlManager::getDoorStatus() {
    return door_ctrl->isOpened();
}

void DoorCtrlManager::setTimer(int timeout) {
    if (door_status) {
        opened_door_timer_.stop();
        opened_door_timer_.start(timeout);
        timeout_ = timeout;
        qDebug() << "set timer: " << timeout;
    }
}

void DoorCtrlManager::on_gpio_open_door() {
    door_ctrl->open();
}

void DoorCtrlManager::on_gpio_close_door() {
    qDebug() << "close door from gpio";
    door_ctrl->close();
}

void DoorCtrlManager::on_opened_door_timeout() {
    emit opened_door_timeout();
}

void DoorCtrlManager::checkDoorStatus() {
    bool isOpened = door_ctrl->isOpened();
    if (isOpened != door_status) {
        door_status = isOpened;
        qDebug() << "manager isOpened: " << isOpened;
        if (isOpened) {
            emit gpio_opened(true);
        } else {
            emit gpio_opened(false);
        }
    }
}

void DoorCtrlManager::run() {
    qDebug() << "door thread start";

    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &DoorCtrlManager::checkDoorStatus);
    timer->start(500);

    exec();
}
