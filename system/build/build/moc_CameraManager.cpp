/****************************************************************************
** Meta object code from reading C++ file 'CameraManager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../CameraManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CameraManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CameraManager_t {
    QByteArrayData data[9];
    char stringdata0[113];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CameraManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CameraManager_t qt_meta_stringdata_CameraManager = {
    {
QT_MOC_LITERAL(0, 0, 13), // "CameraManager"
QT_MOC_LITERAL(1, 14, 10), // "py_ai_proc"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 14), // "vision_input_t"
QT_MOC_LITERAL(4, 41, 12), // "camera_error"
QT_MOC_LITERAL(5, 54, 14), // "transmit_image"
QT_MOC_LITERAL(6, 69, 11), // "on_captured"
QT_MOC_LITERAL(7, 81, 11), // "cam_timeout"
QT_MOC_LITERAL(8, 93, 19) // "on_usb_disconnected"

    },
    "CameraManager\0py_ai_proc\0\0vision_input_t\0"
    "camera_error\0transmit_image\0on_captured\0"
    "cam_timeout\0on_usb_disconnected"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CameraManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,
       4,    2,   62,    2, 0x06 /* Public */,
       5,    2,   67,    2, 0x06 /* Public */,
       5,    3,   72,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    2,   79,    2, 0x0a /* Public */,
       6,    3,   84,    2, 0x0a /* Public */,
       7,    0,   91,    2, 0x0a /* Public */,
       8,    2,   92,    2, 0x0a /* Public */,
       8,    3,   97,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::Int,    2,    2,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::Int,    2,    2,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int,    2,    2,    2,

       0        // eod
};

void CameraManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CameraManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->py_ai_proc((*reinterpret_cast< const vision_input_t(*)>(_a[1]))); break;
        case 1: _t->camera_error((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2]))); break;
        case 2: _t->transmit_image((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2]))); break;
        case 3: _t->transmit_image((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3]))); break;
        case 4: _t->on_captured((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->on_captured((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 6: _t->cam_timeout(); break;
        case 7: _t->on_usb_disconnected((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 8: _t->on_usb_disconnected((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< vision_input_t >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CameraManager::*)(const vision_input_t & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CameraManager::py_ai_proc)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CameraManager::*)(const QString & , const int & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CameraManager::camera_error)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CameraManager::*)(const QString & , const int & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CameraManager::transmit_image)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (CameraManager::*)(int , const QString & , const int & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CameraManager::transmit_image)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CameraManager::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_CameraManager.data,
    qt_meta_data_CameraManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CameraManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CameraManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CameraManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int CameraManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void CameraManager::py_ai_proc(const vision_input_t & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CameraManager::camera_error(const QString & _t1, const int & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CameraManager::transmit_image(const QString & _t1, const int & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CameraManager::transmit_image(int _t1, const QString & _t2, const int & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
