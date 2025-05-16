#ifndef KD_MULTISPLITTER_RUBBERBAND_QUICK_H
#define KD_MULTISPLITTER_RUBBERBAND_QUICK_H

#ifdef KDDOCKWIDGETS_QTQUICK

#include "kddockwidgets/docks_export.h"
#include "Widget_quick.h"

#include <QQuickItem>

namespace Layouting {

// TODO: Finish rubberband
class DOCKS_EXPORT_FOR_UNIT_TESTS RubberBand
    : public QQuickItem,
      public Layouting::Widget_quick
{
    Q_OBJECT
public:
    explicit RubberBand(Layouting::Widget *parent);
    ~RubberBand() override;
};

}


#endif

#endif
