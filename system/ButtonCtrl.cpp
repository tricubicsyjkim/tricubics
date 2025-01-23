#include "ButtonCtrl.h"

ButtonCtrl::ButtonCtrl(uint32_t gpioPin): m_gpio(GpioCtrl(gpioPin))
{
//    m_gpio.setDirection(GPIO_MODE_INPUT);
    m_gpio_v_for_pressed = GPIO_HIGH;
}

ButtonCtrl::~ButtonCtrl() {
}

void ButtonCtrl::pressed_if_gpio_is_low() {
    m_gpio_v_for_pressed = GPIO_LOW;
}

void ButtonCtrl::pressed_if_gpio_is_high() {
    m_gpio_v_for_pressed = GPIO_HIGH;
}

bool ButtonCtrl::isPressed() {
    GpioValue value = m_gpio.get();
    if (value == m_gpio_v_for_pressed) {
        emit buttonPressed();
        return true;
    }
    return false;
}

bool ButtonCtrl::isReleased() {
    GpioValue value = m_gpio.get();
    if (value != m_gpio_v_for_pressed) {
        emit buttonReleased();
        return true;
    }
    return false;
}
