#include "Draggable_p.h"
#include "DragController_p.h"
#include "FloatingWindow_p.h"
#include "WidgetResizeHandler_p.h"
#include "Utils_p.h"


using namespace KDDockWidgets;

class Draggable::Private
{
public:
    explicit Private(QWidgetOrQuick *_thisWidget, bool _enabled)
        : thisWidget(_thisWidget)
        , enabled(_enabled)
    {
        Q_ASSERT(thisWidget);
    }

    QPointer<WidgetResizeHandler> widgetResizeHandler;
    QWidgetOrQuick *const thisWidget;
    const bool enabled;
};

Draggable::Draggable(QWidgetOrQuick *thisWidget, bool enabled)
    : d(new Private(thisWidget, enabled))
{
    if (thisWidget && d->enabled)
        DragController::instance()->registerDraggable(this);
}

Draggable::~Draggable()
{
    if (d->thisWidget && d->enabled)
        DragController::instance()->unregisterDraggable(this);
    delete d;
}

QWidgetOrQuick *Draggable::asWidget() const
{
    return d->thisWidget;
}

bool Draggable::dragCanStart(QPoint pressPos, QPoint globalPos) const
{
    return (globalPos - pressPos).manhattanLength() > KDDockWidgets::startDragDistance();
}

void Draggable::setWidgetResizeHandler(WidgetResizeHandler *w)
{
    Q_ASSERT(!d->widgetResizeHandler);
    Q_ASSERT(w);
    d->widgetResizeHandler = w;
}
