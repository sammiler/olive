#ifdef KDDOCKWIDGETS_QTQUICK
#include "Rubberband_quick.h"

using namespace Layouting;

RubberBand::RubberBand(Layouting::Widget *parent)
    : QQuickItem(parent ? qobject_cast<QQuickItem *>(parent->asQObject()) : nullptr)
    , Layouting::Widget_quick(this)
{
}

RubberBand::~RubberBand() = default;


#endif
