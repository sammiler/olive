#ifndef KDDOCKWIDGETS_QWIDGETADAPTER_H
#define KDDOCKWIDGETS_QWIDGETADAPTER_H

#if !defined(KDDOCKWIDGETS_QTWIDGETS) && !defined(KDDOCKWIDGETS_QTQUICK)
#define KDDOCKWIDGETS_QTWIDGETS
#endif

#include <QWindow>

/**
 * @file
 * @brief Abstraction for supporting both QtWidgets and QtQuick.
 *
 * @author Sérgio Martins \<sergio.martins@kdab.com\>
 */


namespace KDDockWidgets::Private {

inline bool isMinimized(QWindow *window)
{
    return window && (window->windowStates() & Qt::WindowMinimized);
}

}


#ifdef KDDOCKWIDGETS_QTWIDGETS
#include "private/multisplitter/Widget_qwidget.h"
#include "private/widgets/QWidgetAdapter_widgets_p.h"
#include <QMainWindow>
namespace KDDockWidgets {
class MainWindow;
class DockWidget;
typedef QWidget QWidgetOrQuick;
typedef QMainWindow QMainWindowOrQuick;
typedef Layouting::Widget_qwidget LayoutGuestWidgetBase;
typedef KDDockWidgets::MainWindow MainWindowType;
typedef KDDockWidgets::MainWindow MDIMainWindowBase;
typedef KDDockWidgets::DockWidget DockWidgetType;
typedef QWidget WidgetType;
}
#else
#include "private/multisplitter/Widget_quick.h"
#include "private/quick/QWidgetAdapter_quick_p.h"
namespace KDDockWidgets {
class MainWindowQuick;
class DockWidgetQuick;
typedef KDDockWidgets::QWidgetAdapter QWidgetOrQuick;
typedef QWidgetOrQuick QMainWindowOrQuick;
typedef Layouting::Widget_quick LayoutGuestWidgetBase;
typedef KDDockWidgets::MainWindowQuick MainWindowType;
typedef KDDockWidgets::MainWindowQuick MDIMainWindowBase;
typedef KDDockWidgets::DockWidgetQuick DockWidgetType;
typedef QQuickItem WidgetType;
}
#endif

namespace KDDockWidgets {
/// @brief LayoutGuestWidget is the type that Item will host.
///
/// The layouting deals in items, represented by Item. Each item wraps a QWidget (or QQuickItem),
/// such widgets derive from LayoutGuestWidget.
class LayoutGuestWidget : public KDDockWidgets::QWidgetAdapter, public LayoutGuestWidgetBase
{
    Q_OBJECT
public:
    explicit LayoutGuestWidget(QWidgetOrQuick *parent)
        : QWidgetAdapter(parent)
        , LayoutGuestWidgetBase(this)
    {
    }

    ~LayoutGuestWidget() override;
};
}

#endif
