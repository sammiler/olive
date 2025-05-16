/**
 * @file
 * @brief 为支持 QtWidgets 和 QtQuick 提供抽象。
 *
 * @author Sérgio Martins <sergio.martins@kdab.com>
 */

#ifndef KDDOCKWIDGETS_QWIDGETADAPTER_H
#define KDDOCKWIDGETS_QWIDGETADAPTER_H

// 条件编译：如果既没有定义 KDDOCKWIDGETS_QTWIDGETS 也没有定义 KDDOCKWIDGETS_QTQUICK,
// 则默认定义 KDDOCKWIDGETS_QTWIDGETS。
// 这确保了在没有明确指定UI技术栈时，默认使用 Qt Widgets。
#if !defined(KDDOCKWIDGETS_QTWIDGETS) && !defined(KDDOCKWIDGETS_QTQUICK)
#define KDDOCKWIDGETS_QTWIDGETS
#endif

#include <QWindow> // 包含 Qt 窗口基类

namespace KDDockWidgets::Private { // KDDockWidgets 内部私有命名空间

/**
 * @brief (内联函数) 检查给定的 QWindow 是否处于最小化状态。
 * @param window 指向要检查的 QWindow 的指针。
 * @return 如果窗口有效且其状态包含 Qt::WindowMinimized，则返回 true；否则返回 false。
 */
inline bool isMinimized(QWindow *window)
{
    return window && (window->windowStates() & Qt::WindowMinimized);
}

} // namespace KDDockWidgets::Private


#ifdef KDDOCKWIDGETS_QTWIDGETS // 条件编译：如果定义了 KDDOCKWIDGETS_QTWIDGETS (即使用 Qt Widgets)

#include "private/multisplitter/Widget_qwidget.h" // MultiSplitter 模块中 QWidget 的包装器私有头文件
#include "private/widgets/QWidgetAdapter_widgets_p.h" // QWidgetAdapter 的 QtWidgets 特定私有实现头文件
#include <QMainWindow> // Qt 主窗口类

namespace KDDockWidgets {
// 前向声明 QtWidgets 版本的特定类
class MainWindow;
class DockWidget;

// 类型别名，在 QtWidgets 环境下：
typedef QWidget QWidgetOrQuick; ///< QWidgetOrQuick 在此上下文中代表 QWidget。
typedef QMainWindow QMainWindowOrQuick; ///< QMainWindowOrQuick 在此上下文中代表 QMainWindow。
typedef Layouting::Widget_qwidget LayoutGuestWidgetBase; ///< 布局系统中的客户小部件基类，对应 QWidget 的包装。
typedef KDDockWidgets::MainWindow MainWindowType; ///< MainWindowType 代表 QtWidgets 版本的 MainWindow。
typedef KDDockWidgets::MainWindow MDIMainWindowBase; ///< MDIMainWindowBase 代表 QtWidgets 版本的 MDI 主窗口基类 (通常是 MainWindow 本身，根据 MDI 选项配置)。
typedef KDDockWidgets::DockWidget DockWidgetType; ///< DockWidgetType 代表 QtWidgets 版本的 DockWidget。
typedef QWidget WidgetType; ///< WidgetType 代表通用的 UI 元素类型，在此为 QWidget。
} // namespace KDDockWidgets

#else // 如果定义了 KDDOCKWIDGETS_QTQUICK (即使用 Qt Quick)

#include "private/multisplitter/Widget_quick.h" // MultiSplitter 模块中 QQuickItem 的包装器私有头文件
#include "private/quick/QWidgetAdapter_quick_p.h" // QWidgetAdapter 的 QtQuick 特定私有实现头文件
// QQuickItem 通常在 QWidgetAdapter_quick_p.h 或其包含的文件中引入

namespace KDDockWidgets {
// 前向声明 QtQuick 版本的特定类
class MainWindowQuick;
class DockWidgetQuick;

// 类型别名，在 QtQuick 环境下：
typedef KDDockWidgets::QWidgetAdapter QWidgetOrQuick; ///< QWidgetOrQuick 在此上下文中代表 QWidgetAdapter (QQuickItem 的包装)。
typedef QWidgetOrQuick QMainWindowOrQuick; ///< QMainWindowOrQuick 在此上下文中也代表 QWidgetAdapter (包装 QQuickWindow 或作为主窗口的 QQuickItem)。
typedef Layouting::Widget_quick LayoutGuestWidgetBase; ///< 布局系统中的客户小部件基类，对应 QQuickItem 的包装。
typedef KDDockWidgets::MainWindowQuick MainWindowType; ///< MainWindowType 代表 QtQuick 版本的 MainWindowQuick。
typedef KDDockWidgets::MainWindowQuick MDIMainWindowBase; ///< MDIMainWindowBase 代表 QtQuick 版本的 MDI 主窗口基类。
typedef KDDockWidgets::DockWidgetQuick DockWidgetType; ///< DockWidgetType 代表 QtQuick 版本的 DockWidgetQuick。
typedef QQuickItem WidgetType; ///< WidgetType 代表通用的 UI 元素类型，在此为 QQuickItem。
} // namespace KDDockWidgets

#endif // KDDOCKWIDGETS_QTWIDGETS


namespace KDDockWidgets {
/**
 * @brief LayoutGuestWidget 是布局项 (Layouting::Item) 将要承载的小部件类型。
 *
 * 布局系统处理的是“项”(Item)，由 Layouting::Item 表示。每个项包装一个 QWidget (或 QQuickItem)，
 * 这些被包装的小部件派生自 LayoutGuestWidget。
 * LayoutGuestWidget 结合了 QWidgetAdapter (提供 QWidget/QQuickItem 的统一接口)
 * 和 LayoutGuestWidgetBase (提供与布局引擎交互的特定于 QtWidgets 或 QtQuick 的功能)。
 */
class LayoutGuestWidget : public KDDockWidgets::QWidgetAdapter, public LayoutGuestWidgetBase
{
Q_OBJECT // Q_OBJECT 宏，用于启用 Qt 元对象系统特性
    public :
    /**
     * @brief 构造函数。
     * @param parent 指向实际的 QWidget 或 QQuickItem (通过 QWidgetOrQuick 传递)。
     * QWidgetAdapter 和 LayoutGuestWidgetBase 都会使用这个 parent。
     */
    explicit LayoutGuestWidget(QWidgetOrQuick *parent)
    : QWidgetAdapter(parent) // 初始化 QWidgetAdapter 基类
    , LayoutGuestWidgetBase(this) // 初始化 LayoutGuestWidgetBase 基类，将自身作为参数
    {
    }

    /**
     * @brief 析构函数。
     */
    ~LayoutGuestWidget() override;
};
} // namespace KDDockWidgets

#endif // KDDOCKWIDGETS_QWIDGETADAPTER_H
