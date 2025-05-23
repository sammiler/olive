#ifndef KD_QTQUICK_HELPERS_P_H
#define KD_QTQUICK_HELPERS_P_H

#ifdef KDDOCKWIDGETS_QTQUICK


#include <QObject>

class QQuickItem;

namespace KDDockWidgets {
class QtQuickHelpers : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    Q_INVOKABLE qreal logicalDpiFactor(const QQuickItem *item) const;
};
}

#endif

#endif
