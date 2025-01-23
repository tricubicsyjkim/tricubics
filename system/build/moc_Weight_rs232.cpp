/****************************************************************************
** Meta object code from reading C++ file 'Weight_rs232.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Weight_rs232.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Weight_rs232.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Weight_rs232_t {
    QByteArrayData data[25];
    char stringdata0[308];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Weight_rs232_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Weight_rs232_t qt_meta_stringdata_Weight_rs232 = {
    {
QT_MOC_LITERAL(0, 0, 12), // "Weight_rs232"
QT_MOC_LITERAL(1, 13, 8), // "sendData"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 12), // "weight_value"
QT_MOC_LITERAL(4, 36, 9), // "mcu_alive"
QT_MOC_LITERAL(5, 46, 6), // "usb_on"
QT_MOC_LITERAL(6, 53, 7), // "usb_off"
QT_MOC_LITERAL(7, 61, 9), // "usb_reset"
QT_MOC_LITERAL(8, 71, 10), // "usb_status"
QT_MOC_LITERAL(9, 82, 11), // "door_opened"
QT_MOC_LITERAL(10, 94, 11), // "door_closed"
QT_MOC_LITERAL(11, 106, 16), // "usb_disconnected"
QT_MOC_LITERAL(12, 123, 15), // "lock_all_opened"
QT_MOC_LITERAL(13, 139, 14), // "deadbolt_error"
QT_MOC_LITERAL(14, 154, 14), // "reset_deadbolt"
QT_MOC_LITERAL(15, 169, 9), // "writeData"
QT_MOC_LITERAL(16, 179, 4), // "data"
QT_MOC_LITERAL(17, 184, 8), // "readData"
QT_MOC_LITERAL(18, 193, 11), // "handleError"
QT_MOC_LITERAL(19, 205, 28), // "QSerialPort::SerialPortError"
QT_MOC_LITERAL(20, 234, 5), // "error"
QT_MOC_LITERAL(21, 240, 10), // "on_polling"
QT_MOC_LITERAL(22, 251, 17), // "open_door_timeout"
QT_MOC_LITERAL(23, 269, 18), // "close_door_timeout"
QT_MOC_LITERAL(24, 288, 19) // "on_reset_error_prop"

    },
    "Weight_rs232\0sendData\0\0weight_value\0"
    "mcu_alive\0usb_on\0usb_off\0usb_reset\0"
    "usb_status\0door_opened\0door_closed\0"
    "usb_disconnected\0lock_all_opened\0"
    "deadbolt_error\0reset_deadbolt\0writeData\0"
    "data\0readData\0handleError\0"
    "QSerialPort::SerialPortError\0error\0"
    "on_polling\0open_door_timeout\0"
    "close_door_timeout\0on_reset_error_prop"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Weight_rs232[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      13,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  114,    2, 0x06 /* Public */,
       3,    2,  117,    2, 0x06 /* Public */,
       4,    0,  122,    2, 0x06 /* Public */,
       5,    0,  123,    2, 0x06 /* Public */,
       6,    0,  124,    2, 0x06 /* Public */,
       7,    0,  125,    2, 0x06 /* Public */,
       8,    1,  126,    2, 0x06 /* Public */,
       9,    0,  129,    2, 0x06 /* Public */,
      10,    0,  130,    2, 0x06 /* Public */,
      11,    0,  131,    2, 0x06 /* Public */,
      12,    0,  132,    2, 0x06 /* Public */,
      13,    0,  133,    2, 0x06 /* Public */,
      14,    0,  134,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      15,    1,  135,    2, 0x0a /* Public */,
      17,    0,  138,    2, 0x0a /* Public */,
      18,    1,  139,    2, 0x0a /* Public */,
      21,    0,  142,    2, 0x0a /* Public */,
      22,    0,  143,    2, 0x0a /* Public */,
      23,    0,  144,    2, 0x0a /* Public */,
      24,    0,  145,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QByteArray,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QByteArray,   16,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 19,   20,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Weight_rs232::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Weight_rs232 *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendData((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 1: _t->weight_value((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->mcu_alive(); break;
        case 3: _t->usb_on(); break;
        case 4: _t->usb_off(); break;
        case 5: _t->usb_reset(); break;
        case 6: _t->usb_status((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->door_opened(); break;
        case 8: _t->door_closed(); break;
        case 9: _t->usb_disconnected(); break;
        case 10: _t->lock_all_opened(); break;
        case 11: _t->deadbolt_error(); break;
        case 12: _t->reset_deadbolt(); break;
        case 13: _t->writeData((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 14: _t->readData(); break;
        case 15: _t->handleError((*reinterpret_cast< QSerialPort::SerialPortError(*)>(_a[1]))); break;
        case 16: _t->on_polling(); break;
        case 17: _t->open_door_timeout(); break;
        case 18: _t->close_door_timeout(); break;
        case 19: _t->on_reset_error_prop(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Weight_rs232::*)(const QByteArray & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Weight_rs232::sendData)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Weight_rs232::*)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Weight_rs232::weight_value)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Weight_rs232::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Weight_rs232::mcu_alive)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Weight_rs232::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Weight_rs232::usb_on)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Weight_rs232::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Weight_rs232::usb_off)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (Weight_rs232::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Weight_rs232::usb_reset)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (Weight_rs232::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Weight_rs232::usb_status)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (Weight_rs232::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Weight_rs232::door_opened)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (Weight_rs232::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Weight_rs232::door_closed)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (Weight_rs232::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Weight_rs232::usb_disconnected)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (Weight_rs232::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Weight_rs232::lock_all_opened)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (Weight_rs232::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Weight_rs232::deadbolt_error)) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (Weight_rs232::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Weight_rs232::reset_deadbolt)) {
                *result = 12;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Weight_rs232::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_Weight_rs232.data,
    qt_meta_data_Weight_rs232,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Weight_rs232::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Weight_rs232::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Weight_rs232.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Weight_rs232::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void Weight_rs232::sendData(const QByteArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Weight_rs232::weight_value(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Weight_rs232::mcu_alive()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Weight_rs232::usb_on()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void Weight_rs232::usb_off()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void Weight_rs232::usb_reset()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void Weight_rs232::usb_status(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Weight_rs232::door_opened()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void Weight_rs232::door_closed()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void Weight_rs232::usb_disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void Weight_rs232::lock_all_opened()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void Weight_rs232::deadbolt_error()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void Weight_rs232::reset_deadbolt()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
