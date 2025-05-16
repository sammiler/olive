#ifndef KDDOCKWIDGETS_QT5QT6_COMPAT_P_H
#define KDDOCKWIDGETS_QT5QT6_COMPAT_P_H

#include <QMouseEvent>
#include <QDropEvent>


namespace KDDockWidgets::Qt5Qt6Compat {

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)

#define QQUICKITEMgeometryChanged geometryChange

using QEnterEvent = QEnterEvent;
using qintptr = ::qintptr;
using qhashtype = size_t;

inline QPoint eventPos(QDropEvent *ev)
{
    return ev->position().toPoint();
}

inline QPoint eventGlobalPos(QMouseEvent *ev)
{
    return ev->globalPosition().toPoint();
}

#else // Qt 5:

#define QQUICKITEMgeometryChanged geometryChanged

using QEnterEvent = QEvent;
using qintptr = long;
using qhashtype = uint;

inline QPoint eventPos(QDropEvent *ev)
{
    return ev->pos();
}

inline QPoint eventGlobalPos(QMouseEvent *ev)
{
    return ev->globalPos();
}

#endif

}


#endif
