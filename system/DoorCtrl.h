#ifndef DOORCTRL_H
#define DOORCTRL_H

#include <QObject>
#include <QTimer>
#include "GpioCtrl.h"
#include "Instance.h"

#define DEFAULT_DOOR_GPIO 5

class DoorCtrl : public QObject
{
    Q_OBJECT
public:
    explicit DoorCtrl(uint32_t gpioPin = DEFAULT_DOOR_GPIO);
    ~DoorCtrl();

    void close_if_gpio_is_low();
    void close_if_gpio_is_high();

    int open();
    int close();
    bool isOpened();
    bool isClosed();
    void initDoorClosed();
signals:
    void doorOpened();
    void doorClosed();

private:
    GpioCtrl m_gpio;
    GpioValue m_gpio_v_for_close;
};

#endif // DOORCTRL_H
