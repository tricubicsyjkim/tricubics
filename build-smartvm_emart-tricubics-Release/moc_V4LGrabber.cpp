/****************************************************************************
** Meta object code from reading C++ file 'V4LGrabber.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../system/V4LGrabber.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'V4LGrabber.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_V4LGrabber_t {
    QByteArrayData data[9];
    char stringdata0[69];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_V4LGrabber_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_V4LGrabber_t qt_meta_stringdata_V4LGrabber = {
    {
QT_MOC_LITERAL(0, 0, 10), // "V4LGrabber"
QT_MOC_LITERAL(1, 11, 7), // "capture"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 1), // "s"
QT_MOC_LITERAL(4, 22, 6), // "cam_id"
QT_MOC_LITERAL(5, 29, 8), // "mt_index"
QT_MOC_LITERAL(6, 38, 16), // "usb_disconnected"
QT_MOC_LITERAL(7, 55, 5), // "error"
QT_MOC_LITERAL(8, 61, 7) // "timeout"

    },
    "V4LGrabber\0capture\0\0s\0cam_id\0mt_index\0"
    "usb_disconnected\0error\0timeout"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_V4LGrabber[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       1,    2,   52,    2, 0x06 /* Public */,
       1,    3,   57,    2, 0x06 /* Public */,
       6,    2,   64,    2, 0x06 /* Public */,
       6,    3,   69,    2, 0x06 /* Public */,
       7,    1,   76,    2, 0x06 /* Public */,
       8,    1,   79,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::Int,    5,    3,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int,    2,    2,    2,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void V4LGrabber::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<V4LGrabber *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->capture((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->capture((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->capture((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 3: _t->usb_disconnected((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->usb_disconnected((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 5: _t->error((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->timeout((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (V4LGrabber::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&V4LGrabber::capture)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (V4LGrabber::*)(const QString & , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&V4LGrabber::capture)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (V4LGrabber::*)(int , const QString & , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&V4LGrabber::capture)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (V4LGrabber::*)(const QString & , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&V4LGrabber::usb_disconnected)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (V4LGrabber::*)(const QString & , int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&V4LGrabber::usb_disconnected)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (V4LGrabber::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&V4LGrabber::error)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (V4LGrabber::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&V4LGrabber::timeout)) {
                *result = 6;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject V4LGrabber::staticMetaObject = { {
    &QThread::staticMetaObject,
    qt_meta_stringdata_V4LGrabber.data,
    qt_meta_data_V4LGrabber,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *V4LGrabber::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *V4LGrabber::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_V4LGrabber.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int V4LGrabber::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void V4LGrabber::capture(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void V4LGrabber::capture(const QString & _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void V4LGrabber::capture(int _t1, const QString & _t2, int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void V4LGrabber::usb_disconnected(const QString & _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void V4LGrabber::usb_disconnected(const QString & _t1, int _t2, int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void V4LGrabber::error(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void V4LGrabber::timeout(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
