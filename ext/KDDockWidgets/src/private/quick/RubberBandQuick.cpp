#ifdef KDDOCKWIDGETS_QTQUICK


#include "RubberBandQuick.h"
#include "Config.h"

using namespace KDDockWidgets;

RubberBandQuick::RubberBandQuick(QQuickItem *parent)
    : QWidgetAdapter(parent)
{
    setVisible(false);
    setZ(1000);
    QQuickItem *visualItem = createItem(Config::self().qmlEngine(), QStringLiteral("qrc:/kddockwidgets/private/quick/qml/RubberBand.qml"));
    visualItem->setParent(this);
    visualItem->setParentItem(this);
}

#endif
