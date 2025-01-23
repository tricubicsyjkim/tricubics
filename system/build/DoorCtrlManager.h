#ifndef DOORCTRLMANAGER_H
#define DOORCTRLMANAGER_H

#include <QThread>
#include <QTimer>
#include <QDebug>
#include "DoorCtrl.h"

#define DOOR_PIN 5

class DoorCtrlManager : public QThread {
    Q_OBJECT
public:
    explicit DoorCtrlManager(QObject *parent = nullptr);
    ~DoorCtrlManager();

    bool getDoorStatus();
    void setTimer(int timeout);

signals:
    void gpio_opened(bool isOpened);
    void opened_door_timeout();

public slots:
    void on_gpio_open_door();
    void on_gpio_close_door();
    void checkDoorStatus();
    void on_opened_door_timeout();

protected:
    void run() override;

private:
    DoorCtrl *door_ctrl;
    QTimer *timer;
    QTimer opened_door_timer_;
    bool door_status;
    bool quit = false;
    int timeout_;
};

#endif // DOORCTRLMANAGER_H
