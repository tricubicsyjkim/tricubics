#ifndef GPIOCTRL_H
#define GPIOCTRL_H


#include <QObject>
#include <QDebug>
#include <QElapsedTimer>
#include <QMap>
#include <QVector>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QTimer>
#include <memory>
#include <stdint.h>

#define DEV_ASUS_N100   0
#define DEV_ORDROID_M1  1

#ifndef TARGET_DEV
#define TARGET_DEV DEV_ASUS_N100
// #define TARGET_DEV DEV_ORDROID_M1
#endif

#if TARGET_DEV == DEV_ASUS_N100
#include "omniedgeapi.h"
#elif TARGET_DEV == DEV_ORDROID_M1
#include <wiringPi.h>
#endif

typedef enum {
    GPIO_MODE_INPUT = 0,
    GPIO_MODE_OUTPUT = 1
} GpioMode;

typedef enum {
    GPIO_ERROR = -1,
    GPIO_LOW = 0,
    GPIO_HIGH = 1
} GpioValue;

class GpioCtrl : public QObject
{
    Q_OBJECT
public:
    explicit GpioCtrl(uint32_t gpioPin);
    ~GpioCtrl();

    int setDirection(GpioMode mode);
    int set(GpioValue value);
    GpioValue get();

signals:
    void gpioValueChanged(uint32_t gpio, GpioValue value);

private:
    static bool m_initialized;
    uint32_t m_gpio;

#if TARGET_DEV == DEV_ASUS_N100
    int convertGpioNum(uint32_t num);
#endif
};

#endif // GPIOCTRL_H
