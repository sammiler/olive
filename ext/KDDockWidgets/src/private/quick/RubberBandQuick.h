#ifndef KDDOCKWIDGETS_RUBBERBANDQUICK_H
#define KDDOCKWIDGETS_RUBBERBANDQUICK_H

#ifdef KDDOCKWIDGETS_QTQUICK


#include "QWidgetAdapter.h"

namespace KDDockWidgets {

class RubberBandQuick : public QWidgetAdapter
{
    Q_OBJECT
public:
    explicit RubberBandQuick(QQuickItem *parent = nullptr);
};

}


#endif

#endif
