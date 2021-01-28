/****************************************************************************
** Meta object code from reading C++ file 'videodepthrecovery.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../videodepthrecovery.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'videodepthrecovery.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VideoDepthRecovery[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x09,
      33,   19,   19,   19, 0x09,
      44,   19,   19,   19, 0x09,
      53,   19,   19,   19, 0x09,
      73,   19,   19,   19, 0x09,
      95,   19,   19,   19, 0x09,
     121,   19,   19,   19, 0x09,
     143,   19,   19,   19, 0x09,
     168,   19,   19,   19, 0x09,
     194,   19,   19,   19, 0x09,
     222,   19,   19,   19, 0x09,
     244,   19,   19,   19, 0x09,
     268,   19,   19,   19, 0x09,
     300,   19,   19,   19, 0x09,
     322,   19,   19,   19, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_VideoDepthRecovery[] = {
    "VideoDepthRecovery\0\0onFileOpen()\0"
    "onSaveAs()\0onExit()\0onDepthParameters()\0"
    "onRunInitialization()\0onRunBundleOptimization()\0"
    "onRunDepthExpansion()\0onLightstageParameters()\0"
    "onLightstageLoadProject()\0"
    "onLightstageRunVisualHull()\0"
    "onLightstageRunInit()\0onLightstageRunRefine()\0"
    "onLightstageRunDepthExpansion()\0"
    "onLightstageRunMesh()\0onLightstageRunAll()\0"
};

void VideoDepthRecovery::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VideoDepthRecovery *_t = static_cast<VideoDepthRecovery *>(_o);
        switch (_id) {
        case 0: _t->onFileOpen(); break;
        case 1: _t->onSaveAs(); break;
        case 2: _t->onExit(); break;
        case 3: _t->onDepthParameters(); break;
        case 4: _t->onRunInitialization(); break;
        case 5: _t->onRunBundleOptimization(); break;
        case 6: _t->onRunDepthExpansion(); break;
        case 7: _t->onLightstageParameters(); break;
        case 8: _t->onLightstageLoadProject(); break;
        case 9: _t->onLightstageRunVisualHull(); break;
        case 10: _t->onLightstageRunInit(); break;
        case 11: _t->onLightstageRunRefine(); break;
        case 12: _t->onLightstageRunDepthExpansion(); break;
        case 13: _t->onLightstageRunMesh(); break;
        case 14: _t->onLightstageRunAll(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData VideoDepthRecovery::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VideoDepthRecovery::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_VideoDepthRecovery,
      qt_meta_data_VideoDepthRecovery, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VideoDepthRecovery::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VideoDepthRecovery::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VideoDepthRecovery::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VideoDepthRecovery))
        return static_cast<void*>(const_cast< VideoDepthRecovery*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int VideoDepthRecovery::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
