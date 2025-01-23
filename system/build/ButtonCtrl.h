#ifndef BUTTONCTRL_H
#define BUTTONCTRL_H


#include <QObject>
#include "GpioCtrl.h"

#define DEFAULT_BUTTON_GPIO 2


class ButtonCtrl : public QObject
{
    Q_OBJECT
public:
    explicit ButtonCtrl(uint32_t gpioPin = DEFAULT_BUTTON_GPIO);
    ~ButtonCtrl();

    void pressed_if_gpio_is_low();
    void pressed_if_gpio_is_high();
    bool isPressed();
    bool isReleased();

signals:
    void buttonPressed();
    void buttonReleased();

private:
    uint32_t gpioPin_;
    GpioCtrl m_gpio;
    GpioValue m_gpio_v_for_pressed;
};

#endif // BUTTONCTRL_H
