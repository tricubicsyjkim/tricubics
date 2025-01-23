/****************************************************************************
** Meta object code from reading C++ file 'WeightManager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../system/WeightManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WeightManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_WeightManager_t {
    QByteArrayData data[32];
    char stringdata0[463];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WeightManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WeightManager_t qt_meta_stringdata_WeightManager = {
    {
QT_MOC_LITERAL(0, 0, 13), // "WeightManager"
QT_MOC_LITERAL(1, 14, 19), // "weight_trigger_proc"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 15), // "usb_port_opened"
QT_MOC_LITERAL(4, 51, 19), // "weight_sensor_error"
QT_MOC_LITERAL(5, 71, 11), // "door_closed"
QT_MOC_LITERAL(6, 83, 11), // "door_opened"
QT_MOC_LITERAL(7, 95, 16), // "loadcell_trigger"
QT_MOC_LITERAL(8, 112, 17), // "WeightChangedInfo"
QT_MOC_LITERAL(9, 130, 24), // "loadcell_trigger_4vision"
QT_MOC_LITERAL(10, 155, 16), // "reset_error_prop"
QT_MOC_LITERAL(11, 172, 15), // "lock_all_opened"
QT_MOC_LITERAL(12, 188, 14), // "deadbolt_error"
QT_MOC_LITERAL(13, 203, 14), // "reset_deadbolt"
QT_MOC_LITERAL(14, 218, 9), // "on_weight"
QT_MOC_LITERAL(15, 228, 8), // "scale_id"
QT_MOC_LITERAL(16, 237, 5), // "value"
QT_MOC_LITERAL(17, 243, 18), // "on_delta_triggered"
QT_MOC_LITERAL(18, 262, 12), // "on_mcu_alive"
QT_MOC_LITERAL(19, 275, 9), // "on_usb_on"
QT_MOC_LITERAL(20, 285, 10), // "on_usb_off"
QT_MOC_LITERAL(21, 296, 12), // "on_usb_reset"
QT_MOC_LITERAL(22, 309, 13), // "on_usb_status"
QT_MOC_LITERAL(23, 323, 7), // "port_id"
QT_MOC_LITERAL(24, 331, 12), // "on_door_open"
QT_MOC_LITERAL(25, 344, 13), // "on_door_close"
QT_MOC_LITERAL(26, 358, 19), // "on_usb_disconnected"
QT_MOC_LITERAL(27, 378, 18), // "on_ioboard_polling"
QT_MOC_LITERAL(28, 397, 10), // "on_polling"
QT_MOC_LITERAL(29, 408, 18), // "on_lock_all_opened"
QT_MOC_LITERAL(30, 427, 17), // "on_deadbolt_error"
QT_MOC_LITERAL(31, 445, 17) // "on_reset_deadbolt"

    },
    "WeightManager\0weight_trigger_proc\0\0"
    "usb_port_opened\0weight_sensor_error\0"
    "door_closed\0door_opened\0loadcell_trigger\0"
    "WeightChangedInfo\0loadcell_trigger_4vision\0"
    "reset_error_prop\0lock_all_opened\0"
    "deadbolt_error\0reset_deadbolt\0on_weight\0"
    "scale_id\0value\0on_delta_triggered\0"
    "on_mcu_alive\0on_usb_on\0on_usb_off\0"
    "on_usb_reset\0on_usb_status\0port_id\0"
    "on_door_open\0on_door_close\0"
    "on_usb_disconnected\0on_ioboard_polling\0"
    "on_polling\0on_lock_all_opened\0"
    "on_deadbolt_error\0on_reset_deadbolt"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WeightManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      30,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      13,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,  164,    2, 0x06 /* Public */,
       1,    3,  169,    2, 0x06 /* Public */,
       1,    4,  176,    2, 0x06 /* Public */,
       3,    1,  185,    2, 0x06 /* Public */,
       4,    2,  188,    2, 0x06 /* Public */,
       5,    0,  193,    2, 0x06 /* Public */,
       6,    0,  194,    2, 0x06 /* Public */,
       7,    1,  195,    2, 0x06 /* Public */,
       9,    1,  198,    2, 0x06 /* Public */,
      10,    0,  201,    2, 0x06 /* Public */,
      11,    0,  202,    2, 0x06 /* Public */,
      12,    0,  203,    2, 0x06 /* Public */,
      13,    0,  204,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      14,    2,  205,    2, 0x0a /* Public */,
      17,    2,  210,    2, 0x0a /* Public */,
      17,    3,  215,    2, 0x0a /* Public */,
      17,    4,  222,    2, 0x0a /* Public */,
      18,    0,  231,    2, 0x0a /* Public */,
      19,    0,  232,    2, 0x0a /* Public */,
      20,    0,  233,    2, 0x0a /* Public */,
      21,    0,  234,    2, 0x0a /* Public */,
      22,    1,  235,    2, 0x0a /* Public */,
      24,    0,  238,    2, 0x0a /* Public */,
      25,    0,  239,    2, 0x0a /* Public */,
      26,    0,  240,    2, 0x0a /* Public */,
      27,    0,  241,    2, 0x0a /* Public */,
      28,    0,  242,    2, 0x0a /* Public */,
      29,    0,  243,    2, 0x0a /* Public */,
      30,    0,  244,    2, 0x0a /* Public */,
      31,    0,  245,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,    2,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    2,    2,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    2,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    2,
    QMetaType::Void, 0x80000000 | 8,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   15,   16,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,    2,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    2,    2,    2,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   23,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void WeightManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WeightManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->weight_trigger_proc((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2]))); break;
        case 1: _t->weight_trigger_proc((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3]))); break;
        case 2: _t->weight_trigger_proc((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3])),(*reinterpret_cast< const int(*)>(_a[4]))); break;
        case 3: _t->usb_port_opened((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 4: _t->weight_sensor_error((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2]))); break;
        case 5: _t->door_closed(); break;
        case 6: _t->door_opened(); break;
        case 7: _t->loadcell_trigger((*reinterpret_cast< WeightChangedInfo(*)>(_a[1]))); break;
        case 8: _t->loadcell_trigger_4vision((*reinterpret_cast< WeightChangedInfo(*)>(_a[1]))); break;
        case 9: _t->reset_error_prop(); break;
        case 10: _t->lock_all_opened(); break;
        case 11: _t->deadbolt_error(); break;
        case 12: _t->reset_deadbolt(); break;
        case 13: _t->on_weight((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 14: _t->on_delta_triggered((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 15: _t->on_delta_triggered((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 16: _t->on_delta_triggered((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 17: _t->on_mcu_alive(); break;
        case 18: _t->on_usb_on(); break;
        case 19: _t->on_usb_off(); break;
        case 20: _t->on_usb_reset(); break;
        case 21: _t->on_usb_status((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 22: _t->on_door_open(); break;
        case 23: _t->on_door_close(); break;
        case 24: _t->on_usb_disconnected(); break;
        case 25: _t->on_ioboard_polling(); break;
        case 26: _t->on_polling(); break;
        case 27: _t->on_lock_all_opened(); break;
        case 28: _t->on_deadbolt_error(); break;
        case 29: _t->on_reset_deadbolt(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< WeightChangedInfo >(); break;
            }
            break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< WeightChangedInfo >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (WeightManager::*)(const int & , const int & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WeightManager::weight_trigger_proc)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (WeightManager::*)(const int & , const int & , const int & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WeightManager::weight_trigger_proc)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (WeightManager::*)(const int & , const int & , const int & , const int & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WeightManager::weight_trigger_proc)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (WeightManager::*)(const int & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WeightManager::usb_port_opened)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (WeightManager::*)(const QString & , const int & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WeightManager::weight_sensor_error)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (WeightManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WeightManager::door_closed)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (WeightManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WeightManager::door_opened)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (WeightManager::*)(WeightChangedInfo );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WeightManager::loadcell_trigger)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (WeightManager::*)(WeightChangedInfo );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WeightManager::loadcell_trigger_4vision)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (WeightManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WeightManager::reset_error_prop)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (WeightManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WeightManager::lock_all_opened)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (WeightManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WeightManager::deadbolt_error)) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (WeightManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WeightManager::reset_deadbolt)) {
                *result = 12;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject WeightManager::staticMetaObject = { {
    &SensorDAQImpl::staticMetaObject,
    qt_meta_stringdata_WeightManager.data,
    qt_meta_data_WeightManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *WeightManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WeightManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_WeightManager.stringdata0))
        return static_cast<void*>(this);
    return SensorDAQImpl::qt_metacast(_clname);
}

int WeightManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SensorDAQImpl::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 30)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 30;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 30)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 30;
    }
    return _id;
}

// SIGNAL 0
void WeightManager::weight_trigger_proc(const int & _t1, const int & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WeightManager::weight_trigger_proc(const int & _t1, const int & _t2, const int & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void WeightManager::weight_trigger_proc(const int & _t1, const int & _t2, const int & _t3, const int & _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void WeightManager::usb_port_opened(const int & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void WeightManager::weight_sensor_error(const QString & _t1, const int & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void WeightManager::door_closed()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void WeightManager::door_opened()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void WeightManager::loadcell_trigger(WeightChangedInfo _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void WeightManager::loadcell_trigger_4vision(WeightChangedInfo _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void WeightManager::reset_error_prop()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void WeightManager::lock_all_opened()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void WeightManager::deadbolt_error()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void WeightManager::reset_deadbolt()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
