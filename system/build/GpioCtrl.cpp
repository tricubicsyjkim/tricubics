#include "GpioCtrl.h"

bool GpioCtrl::m_initialized = false;

#if TARGET_DEV == DEV_ASUS_N100
int GpioCtrl::convertGpioNum(uint32_t num) {
    int result = -1;

    int gpio_table[] = {
        EAPI_ID_GPIO_GPIO00,
        EAPI_ID_GPIO_GPIO01,
        EAPI_ID_GPIO_GPIO02,
        EAPI_ID_GPIO_GPIO03,
        EAPI_ID_GPIO_GPIO04,
        EAPI_ID_GPIO_GPIO05,
        EAPI_ID_GPIO_GPIO06,
        EAPI_ID_GPIO_GPIO07
    };

    if (num >= 1 && num <= 8) {
        result = gpio_table[num - 1];
    }
    return result;
}
#endif

GpioCtrl::GpioCtrl(uint32_t gpioPin) : m_gpio(gpioPin) {
    if (!m_initialized) {
#if TARGET_DEV == DEV_ASUS_N100
        if (EApiLibInitialize() == EAPI_STATUS_SUCCESS) m_initialized = true;
#elif TARGET_DEV == DEV_ORDROID_M1
#endif
    }
}

GpioCtrl::~GpioCtrl() {
    if(m_initialized) {
#if TARGET_DEV == DEV_ASUS_N100
        EApiLibUnInitialize();
#elif TARGET_DEV == DEV_ORDROID_M1

#endif
        m_initialized = false;
    }
}

int GpioCtrl::setDirection(GpioMode mode) {
    int result = 0;

#if TARGET_DEV == DEV_ASUS_N100
    int id = convertGpioNum(m_gpio);
    if (id == -1) {
        result = -1;
    } else {
        uint32_t direction = (mode == GPIO_MODE_INPUT) ? EAPI_GPIO_INPUT : EAPI_GPIO_OUTPUT;
        result = static_cast<int>(EApiGPIOSetDirection(id, EAPI_GPIO_BITMASK_SELECT, direction));
    }
#elif TARGET_DEV == DEV_ORDROID_M1
#endif

    return result;
}

int GpioCtrl::set(GpioValue value) {
    int result=setDirection(GPIO_MODE_OUTPUT);
    if(result == EAPI_STATUS_SUCCESS) {
#if TARGET_DEV == DEV_ASUS_N100
        int id = convertGpioNum(m_gpio);
        if(id == -1) {
            // ERROR!!
            result = -1;
        } else {
            int level;
            if (value == GPIO_LOW) {
                level = EAPI_GPIO_LOW;
            } else if (value == GPIO_HIGH) {
                level = EAPI_GPIO_HIGH;
            }
            result = (int)EApiGPIOSetLevel(id, EAPI_GPIO_BITMASK_SELECT, level);
        }
#elif TARGET_DEV == DEV_ORDROID_M1

#endif /* TARGET_DEV */
    }
    return result;
}

GpioValue GpioCtrl::get() {
    GpioValue value = GPIO_ERROR;
#if TARGET_DEV == DEV_ASUS_N100
    int id = convertGpioNum(m_gpio);
    if(id == -1) {
        qDebug()<< "Error: GPIO pin number is invalid";
        value = GPIO_ERROR;
    } else {
        uint32_t level;
        int result = EApiGPIOGetLevel(id, EAPI_GPIO_BITMASK_SELECT, &level);
        if(result != EAPI_STATUS_SUCCESS) {
            qDebug() << "Error: Fail to get GPIO level";
            value = GPIO_ERROR;
        } else {
            if (level == EAPI_GPIO_LOW) {
                value = GPIO_LOW;
            } else if (level == EAPI_GPIO_HIGH) {
                value = GPIO_HIGH;
            }
        }
    }
#elif TARGET_DEV == DEV_ORDROID_M1

#endif /* TARGET_DEV */
    return value;
}

