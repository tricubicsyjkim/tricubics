/****************************************************************************
** Meta object code from reading C++ file 'SmartVM.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../system/SmartVM.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SmartVM.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SmartVM_t {
    QByteArrayData data[44];
    char stringdata0[672];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SmartVM_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SmartVM_t qt_meta_stringdata_SmartVM = {
    {
QT_MOC_LITERAL(0, 0, 7), // "SmartVM"
QT_MOC_LITERAL(1, 8, 18), // "decision_processed"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 16), // "open_cam_process"
QT_MOC_LITERAL(4, 45, 18), // "data_analysis_proc"
QT_MOC_LITERAL(5, 64, 12), // "vision_out_t"
QT_MOC_LITERAL(6, 77, 14), // "gpio_open_door"
QT_MOC_LITERAL(7, 92, 15), // "gpio_close_door"
QT_MOC_LITERAL(8, 108, 10), // "play_audio"
QT_MOC_LITERAL(9, 119, 7), // "on_init"
QT_MOC_LITERAL(10, 127, 18), // "on_delayed_cam_run"
QT_MOC_LITERAL(11, 146, 2), // "id"
QT_MOC_LITERAL(12, 149, 7), // "on_quit"
QT_MOC_LITERAL(13, 157, 14), // "on_door_opened"
QT_MOC_LITERAL(14, 172, 14), // "on_door_closed"
QT_MOC_LITERAL(15, 187, 18), // "on_usb_port_opened"
QT_MOC_LITERAL(16, 206, 7), // "port_id"
QT_MOC_LITERAL(17, 214, 20), // "on_aivison_processed"
QT_MOC_LITERAL(18, 235, 22), // "on_weight_trigger_proc"
QT_MOC_LITERAL(19, 258, 18), // "on_https_processed"
QT_MOC_LITERAL(20, 277, 25), // "on_shopping_finished_proc"
QT_MOC_LITERAL(21, 303, 15), // "on_device_error"
QT_MOC_LITERAL(22, 319, 17), // "on_transmit_image"
QT_MOC_LITERAL(23, 337, 17), // "on_event_off_proc"
QT_MOC_LITERAL(24, 355, 16), // "resolveEventType"
QT_MOC_LITERAL(25, 372, 10), // "EVENT_TYPE"
QT_MOC_LITERAL(26, 383, 5), // "event"
QT_MOC_LITERAL(27, 389, 16), // "on_audio_timeout"
QT_MOC_LITERAL(28, 406, 19), // "on_loadcell_trigger"
QT_MOC_LITERAL(29, 426, 17), // "WeightChangedInfo"
QT_MOC_LITERAL(30, 444, 11), // "changedInfo"
QT_MOC_LITERAL(31, 456, 27), // "on_loadcell_trigger_4vision"
QT_MOC_LITERAL(32, 484, 18), // "on_lock_all_opened"
QT_MOC_LITERAL(33, 503, 17), // "on_deadbolt_error"
QT_MOC_LITERAL(34, 521, 15), // "get_sensor_type"
QT_MOC_LITERAL(35, 537, 8), // "dev_name"
QT_MOC_LITERAL(36, 546, 17), // "on_reset_deadbolt"
QT_MOC_LITERAL(37, 564, 14), // "loop_open_door"
QT_MOC_LITERAL(38, 579, 17), // "on_button_pressed"
QT_MOC_LITERAL(39, 597, 18), // "on_button_released"
QT_MOC_LITERAL(40, 616, 14), // "on_gpio_opened"
QT_MOC_LITERAL(41, 631, 22), // "on_opened_door_timeout"
QT_MOC_LITERAL(42, 654, 8), // "setTimer"
QT_MOC_LITERAL(43, 663, 8) // "duration"

    },
    "SmartVM\0decision_processed\0\0"
    "open_cam_process\0data_analysis_proc\0"
    "vision_out_t\0gpio_open_door\0gpio_close_door\0"
    "play_audio\0on_init\0on_delayed_cam_run\0"
    "id\0on_quit\0on_door_opened\0on_door_closed\0"
    "on_usb_port_opened\0port_id\0"
    "on_aivison_processed\0on_weight_trigger_proc\0"
    "on_https_processed\0on_shopping_finished_proc\0"
    "on_device_error\0on_transmit_image\0"
    "on_event_off_proc\0resolveEventType\0"
    "EVENT_TYPE\0event\0on_audio_timeout\0"
    "on_loadcell_trigger\0WeightChangedInfo\0"
    "changedInfo\0on_loadcell_trigger_4vision\0"
    "on_lock_all_opened\0on_deadbolt_error\0"
    "get_sensor_type\0dev_name\0on_reset_deadbolt\0"
    "loop_open_door\0on_button_pressed\0"
    "on_button_released\0on_gpio_opened\0"
    "on_opened_door_timeout\0setTimer\0"
    "duration"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SmartVM[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      34,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  184,    2, 0x06 /* Public */,
       3,    1,  187,    2, 0x06 /* Public */,
       4,    1,  190,    2, 0x06 /* Public */,
       6,    0,  193,    2, 0x06 /* Public */,
       7,    0,  194,    2, 0x06 /* Public */,
       8,    1,  195,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,  198,    2, 0x0a /* Public */,
      10,    1,  199,    2, 0x0a /* Public */,
      12,    0,  202,    2, 0x0a /* Public */,
      13,    0,  203,    2, 0x0a /* Public */,
      14,    0,  204,    2, 0x0a /* Public */,
      15,    1,  205,    2, 0x0a /* Public */,
      17,    1,  208,    2, 0x0a /* Public */,
      18,    4,  211,    2, 0x0a /* Public */,
      19,    1,  220,    2, 0x0a /* Public */,
      20,    1,  223,    2, 0x0a /* Public */,
      21,    2,  226,    2, 0x0a /* Public */,
      22,    2,  231,    2, 0x0a /* Public */,
      22,    3,  236,    2, 0x0a /* Public */,
      23,    0,  243,    2, 0x0a /* Public */,
      24,    1,  244,    2, 0x0a /* Public */,
      27,    0,  247,    2, 0x0a /* Public */,
      28,    1,  248,    2, 0x0a /* Public */,
      31,    1,  251,    2, 0x0a /* Public */,
      32,    0,  254,    2, 0x0a /* Public */,
      33,    0,  255,    2, 0x0a /* Public */,
      34,    1,  256,    2, 0x0a /* Public */,
      36,    0,  259,    2, 0x0a /* Public */,
      37,    0,  260,    2, 0x0a /* Public */,
      38,    0,  261,    2, 0x0a /* Public */,
      39,    0,  262,    2, 0x0a /* Public */,
      40,    1,  263,    2, 0x0a /* Public */,
      41,    0,  266,    2, 0x0a /* Public */,
      42,    1,  267,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, 0x80000000 | 5,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   16,
    QMetaType::Void, 0x80000000 | 5,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    2,    2,    2,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::Int,    2,    2,    2,
    QMetaType::Void,
    0x80000000 | 25, QMetaType::QString,   26,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 29,   30,
    QMetaType::Void, 0x80000000 | 29,   30,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::QString, QMetaType::QString,   35,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   43,

       0        // eod
};

void SmartVM::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SmartVM *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->decision_processed((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 1: _t->open_cam_process((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->data_analysis_proc((*reinterpret_cast< const vision_out_t(*)>(_a[1]))); break;
        case 3: _t->gpio_open_door(); break;
        case 4: _t->gpio_close_door(); break;
        case 5: _t->play_audio((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 6: _t->on_init(); break;
        case 7: _t->on_delayed_cam_run((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->on_quit(); break;
        case 9: _t->on_door_opened(); break;
        case 10: _t->on_door_closed(); break;
        case 11: _t->on_usb_port_opened((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 12: _t->on_aivison_processed((*reinterpret_cast< const vision_out_t(*)>(_a[1]))); break;
        case 13: _t->on_weight_trigger_proc((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3])),(*reinterpret_cast< const int(*)>(_a[4]))); break;
        case 14: _t->on_https_processed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: _t->on_shopping_finished_proc((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 16: _t->on_device_error((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2]))); break;
        case 17: _t->on_transmit_image((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2]))); break;
        case 18: _t->on_transmit_image((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3]))); break;
        case 19: _t->on_event_off_proc(); break;
        case 20: { EVENT_TYPE _r = _t->resolveEventType((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< EVENT_TYPE*>(_a[0]) = std::move(_r); }  break;
        case 21: _t->on_audio_timeout(); break;
        case 22: _t->on_loadcell_trigger((*reinterpret_cast< WeightChangedInfo(*)>(_a[1]))); break;
        case 23: _t->on_loadcell_trigger_4vision((*reinterpret_cast< WeightChangedInfo(*)>(_a[1]))); break;
        case 24: _t->on_lock_all_opened(); break;
        case 25: _t->on_deadbolt_error(); break;
        case 26: { QString _r = _t->get_sensor_type((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 27: _t->on_reset_deadbolt(); break;
        case 28: _t->loop_open_door(); break;
        case 29: _t->on_button_pressed(); break;
        case 30: _t->on_button_released(); break;
        case 31: _t->on_gpio_opened((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        case 32: _t->on_opened_door_timeout(); break;
        case 33: _t->setTimer((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< vision_out_t >(); break;
            }
            break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< vision_out_t >(); break;
            }
            break;
        case 22:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< WeightChangedInfo >(); break;
            }
            break;
        case 23:
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
            using _t = void (SmartVM::*)(const int & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SmartVM::decision_processed)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SmartVM::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SmartVM::open_cam_process)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SmartVM::*)(const vision_out_t & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SmartVM::data_analysis_proc)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (SmartVM::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SmartVM::gpio_open_door)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (SmartVM::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SmartVM::gpio_close_door)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (SmartVM::*)(const int & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SmartVM::play_audio)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SmartVM::staticMetaObject = { {
    &SignalActorImpl::staticMetaObject,
    qt_meta_stringdata_SmartVM.data,
    qt_meta_data_SmartVM,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SmartVM::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SmartVM::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SmartVM.stringdata0))
        return static_cast<void*>(this);
    return SignalActorImpl::qt_metacast(_clname);
}

int SmartVM::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SignalActorImpl::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 34)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 34;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 34)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 34;
    }
    return _id;
}

// SIGNAL 0
void SmartVM::decision_processed(const int & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SmartVM::open_cam_process(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SmartVM::data_analysis_proc(const vision_out_t & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SmartVM::gpio_open_door()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void SmartVM::gpio_close_door()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void SmartVM::play_audio(const int & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
