/****************************************************************************
** Meta object code from reading C++ file 'runconfigdlg.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../runconfigdlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'runconfigdlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DepthRecoveryMultiRuner[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_DepthRecoveryMultiRuner[] = {
    "DepthRecoveryMultiRuner\0"
};

void DepthRecoveryMultiRuner::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData DepthRecoveryMultiRuner::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DepthRecoveryMultiRuner::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DepthRecoveryMultiRuner,
      qt_meta_data_DepthRecoveryMultiRuner, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DepthRecoveryMultiRuner::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DepthRecoveryMultiRuner::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DepthRecoveryMultiRuner::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DepthRecoveryMultiRuner))
        return static_cast<void*>(const_cast< DepthRecoveryMultiRuner*>(this));
    return QObject::qt_metacast(_clname);
}

int DepthRecoveryMultiRuner::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_RunConfigDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x0a,
      23,   13,   13,   13, 0x0a,
      36,   13,   32,   13, 0x0a,
      43,   13,   13,   13, 0x0a,
      56,   13,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_RunConfigDlg[] = {
    "RunConfigDlg\0\0accept()\0reject()\0int\0"
    "exec()\0onFinished()\0onStarted()\0"
};

void RunConfigDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        RunConfigDlg *_t = static_cast<RunConfigDlg *>(_o);
        switch (_id) {
        case 0: _t->accept(); break;
        case 1: _t->reject(); break;
        case 2: { int _r = _t->exec();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 3: _t->onFinished(); break;
        case 4: _t->onStarted(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData RunConfigDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject RunConfigDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_RunConfigDlg,
      qt_meta_data_RunConfigDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RunConfigDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RunConfigDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RunConfigDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RunConfigDlg))
        return static_cast<void*>(const_cast< RunConfigDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int RunConfigDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
