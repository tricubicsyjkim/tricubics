/****************************************************************************
** Meta object code from reading C++ file 'DoorCtrlManager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../system/DoorCtrlManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DoorCtrlManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DoorCtrlManager_t {
    QByteArrayData data[9];
    char stringdata0[134];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DoorCtrlManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DoorCtrlManager_t qt_meta_stringdata_DoorCtrlManager = {
    {
QT_MOC_LITERAL(0, 0, 15), // "DoorCtrlManager"
QT_MOC_LITERAL(1, 16, 11), // "gpio_opened"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 8), // "isOpened"
QT_MOC_LITERAL(4, 38, 19), // "opened_door_timeout"
QT_MOC_LITERAL(5, 58, 17), // "on_gpio_open_door"
QT_MOC_LITERAL(6, 76, 18), // "on_gpio_close_door"
QT_MOC_LITERAL(7, 95, 15), // "checkDoorStatus"
QT_MOC_LITERAL(8, 111, 22) // "on_opened_door_timeout"

    },
    "DoorCtrlManager\0gpio_opened\0\0isOpened\0"
    "opened_door_timeout\0on_gpio_open_door\0"
    "on_gpio_close_door\0checkDoorStatus\0"
    "on_opened_door_timeout"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DoorCtrlManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       4,    0,   47,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   48,    2, 0x0a /* Public */,
       6,    0,   49,    2, 0x0a /* Public */,
       7,    0,   50,    2, 0x0a /* Public */,
       8,    0,   51,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DoorCtrlManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DoorCtrlManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->gpio_opened((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->opened_door_timeout(); break;
        case 2: _t->on_gpio_open_door(); break;
        case 3: _t->on_gpio_close_door(); break;
        case 4: _t->checkDoorStatus(); break;
        case 5: _t->on_opened_door_timeout(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DoorCtrlManager::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DoorCtrlManager::gpio_opened)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DoorCtrlManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DoorCtrlManager::opened_door_timeout)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DoorCtrlManager::staticMetaObject = { {
    &QThread::staticMetaObject,
    qt_meta_stringdata_DoorCtrlManager.data,
    qt_meta_data_DoorCtrlManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DoorCtrlManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DoorCtrlManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DoorCtrlManager.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int DoorCtrlManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void DoorCtrlManager::gpio_opened(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DoorCtrlManager::opened_door_timeout()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
