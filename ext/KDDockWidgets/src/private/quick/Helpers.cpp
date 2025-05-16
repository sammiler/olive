#ifdef KDDOCKWIDGETS_QTQUICK


#include "Helpers_p.h"
#include "QWidgetAdapter.h"

#include <QQuickItem>

using namespace KDDockWidgets;

qreal QtQuickHelpers::logicalDpiFactor(const QQuickItem *) const
{
    return 1; // TODO: Support the actual dpi factor.
    // return KDDockWidgets::logicalDpiFactor(item);
}

#endif
