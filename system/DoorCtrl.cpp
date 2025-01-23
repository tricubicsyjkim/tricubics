#include "DoorCtrl.h"

DoorCtrl::DoorCtrl(uint32_t gpioPin): m_gpio(GpioCtrl(gpioPin))
{
    m_gpio_v_for_close = GPIO_HIGH;
    m_gpio.set(m_gpio_v_for_close);
}

DoorCtrl::~DoorCtrl() {
}

void DoorCtrl::close_if_gpio_is_low() {
    m_gpio_v_for_close = GPIO_LOW;
}

void DoorCtrl::close_if_gpio_is_high() {
    m_gpio_v_for_close = GPIO_HIGH;
}

int DoorCtrl::open() {
    GpioValue v = (m_gpio_v_for_close == GPIO_HIGH) ? GPIO_LOW : GPIO_HIGH;
    int result = m_gpio.set(v);
    if (result == 0) {
//        emit doorOpened();
        qDebug()<<"gpio door opened";
    }
    return result;
}

int DoorCtrl::close() {
    int result = m_gpio.set(m_gpio_v_for_close);
    if (result == 0) {
//        emit doorClosed();
        qDebug()<<"gpio door closed";
    }
    return result;
}

bool DoorCtrl::isOpened() {
    GpioValue value = m_gpio.get();
    return (value != m_gpio_v_for_close);
}

bool DoorCtrl::isClosed() {
    GpioValue value = m_gpio.get();
    return (value == m_gpio_v_for_close);
}

void DoorCtrl::initDoorClosed()
{
    if(isOpened()){
        close();
    }
    qDebug()<<"init door closed";
}

