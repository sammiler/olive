/**
 * @file
 * @brief 一个工厂类，允许用户自定义一些内部小部件。
 *
 * @author Sérgio Martins <sergio.martins@kdab.com>
 */

#ifndef KDDOCKWIDGETS_FRAMEWORKWIDGETFACTORY_H
#define KDDOCKWIDGETS_FRAMEWORKWIDGETFACTORY_H

#include "docks_export.h" // 导入导出宏定义
#include "KDDockWidgets.h" // KDDockWidgets 公共头文件，包含核心枚举和类型定义
#include "QWidgetAdapter.h" // QWidget 和 QQuickItem 的适配器类

#include <QMap> // Qt 映射容器 (用于缓存图标等)
#include <utility> // C++ 标准库，包含 std::pair (用于 QMap 的键)

// clazy:excludeall=ctor-missing-parent-argument // clazy 静态分析器指令：排除所有关于构造函数缺少父参数的警告

QT_BEGIN_NAMESPACE
// 前向声明 Qt 类
class QAbstractButton; // Qt 抽象按钮基类
QT_END_NAMESPACE

// 前向声明内部布局引擎相关的类
namespace Layouting {
class Separator; // 分隔符类
class Widget; // 布局引擎中小部件的基类包装器
}

namespace KDDockWidgets {

// 前向声明 KDDockWidgets 内部类
class MainWindowBase; // 主窗口基类
class DropIndicatorOverlayInterface; // 拖放指示器覆盖层接口类
class FloatingWindow; // 浮动窗口类
class TabWidget; // 标签页小部件类
class TitleBar; // 标题栏类
class Frame; // 框架类 (容纳 DockWidgetBase)
class DropArea; // 放置区域类
class SideBar; // 侧边栏类
class TabBar; // 标签栏类
class TabWidgetQuick; // QtQuick 版本的 TabWidget (如果适用)

/**
 * @brief 一个工厂类，允许用户自定义一些内部小部件。
 * 这是可选的，如果未提供，则将使用默认的工厂，即 @ref DefaultWidgetFactory。
 *
 * 建议从 @ref DefaultWidgetFactory 派生，除非您需要重写所有方法。
 *
 * 通过子类化 FrameworkWidgetFactory，可以对一些非公开的小部件进行细粒度的自定义和样式设置，
 * 例如标题栏、停靠小部件框架和标签页小部件。
 *
 * 要设置您自己的工厂，请参见 Config::setFrameworkWidgetFactory()。
 *
 * 将来也可能用于提供 QtQuickWidget 的工厂。
 *
 * @sa Config::setFrameworkWidgetFactory()
 */
class DOCKS_EXPORT FrameworkWidgetFactory : public QObject
{
Q_OBJECT // Q_OBJECT 宏，用于启用 Qt 元对象系统特性
    public :
    /**
     * @brief 默认构造函数。
     */
    FrameworkWidgetFactory() = default;

    /**
     * @brief 析构函数。不要直接删除 FrameworkWidgetFactory，它由框架拥有。
     */
    ~FrameworkWidgetFactory() override;

    /**
     * @brief 由框架内部调用以创建一个 Frame 类实例。
     * 重写此方法以提供您自己的 Frame 子类。Frame 是持有标题栏和标签页小部件（用于容纳 DockWidget）的小部件。
     * @param parent 直接传递给 Frame 的构造函数。
     * @param options 直接传递给 Frame 的构造函数。
     * @return 指向新创建的 Frame 实例的指针。
     */
    virtual Frame *createFrame(QWidgetOrQuick *parent = nullptr, FrameOptions options = FrameOption_None) const = 0;

    /**
     * @brief 由框架内部调用以创建一个 TitleBar 实例。
     * 重写此方法以提供您自己的 TitleBar 子类。如果重写此方法，则还需要重写下面的重载版本。
     * @param frame 直接传递给 TitleBar 的构造函数（表示此标题栏属于哪个 Frame）。
     * @return 指向新创建的 TitleBar 实例的指针。
     */
    virtual TitleBar *createTitleBar(Frame *frame) const = 0;

    /**
     * @brief 由框架内部调用以创建一个 TitleBar 实例。
     * 重写此方法以提供您自己的 TitleBar 子类。如果重写此方法，则还需要重写上面的重载版本。
     * @param floatingWindow 直接传递给 TitleBar 的构造函数（表示此标题栏属于哪个 FloatingWindow）。
     * @return 指向新创建的 TitleBar 实例的指针。
     */
    virtual TitleBar *createTitleBar(FloatingWindow *floatingWindow) const = 0;

    /**
     * @brief 由框架内部调用以创建一个 TabWidget 实例。
     * 重写此方法以提供您自己的 TabWidget 子类。
     * @param parent 直接传递给 TabWidget 的构造函数（通常是一个 Frame）。
     * @param options TabWidget 的选项。
     * @return 指向新创建的 TabWidget 实例的指针。
     */
    virtual TabWidget *createTabWidget(Frame *parent, TabWidgetOptions options = TabWidgetOption_None) const = 0;

    /**
     * @brief 由框架内部调用以创建一个 TabBar 实例。
     * 重写此方法以提供您自己的 TabBar 子类。
     * @param parent 直接传递给 TabBar 的构造函数（通常是一个 TabWidget）。
     * @return 指向新创建的 TabBar 实例的指针。
     */
    virtual TabBar *createTabBar(TabWidget *parent = nullptr) const = 0;

    /**
     * @brief 由框架内部调用以创建一个 Separator (分隔符) 实例。
     * 重写此方法以提供您自己的 Separator 子类。Separator 允许用户调整嵌套停靠小部件的大小。
     * @param parent 直接传递给 Separator 的构造函数 (通常是 MultiSplitter 或其内部项)。
     * @return 指向新创建的 Layouting::Separator 实例的指针。
     */
    virtual Layouting::Separator *createSeparator(Layouting::Widget *parent = nullptr) const = 0;

    /**
     * @brief 由框架内部调用以创建一个 FloatingWindow 实例。
     * 重写此方法以提供您自己的 FloatingWindow 子类。如果重写此方法，则还需要重写下面的重载版本。
     * @param parent 直接传递给 FloatingWindow 的构造函数 (通常是 MainWindowBase)。
     * @param flags 浮动窗口的标志。
     * @return 指向新创建的 FloatingWindow 实例的指针。
     */
    virtual FloatingWindow *createFloatingWindow(MainWindowBase *parent = nullptr,
                                                 FloatingWindowFlags flags = FloatingWindowFlag::FromGlobalConfig) const = 0;

    /**
     * @brief 由框架内部调用以创建一个 FloatingWindow 实例。
     * 重写此方法以提供您自己的 FloatingWindow 子类。如果重写此方法，则还需要重写上面的重载版本。
     * @param frame 要包含在此浮动窗口中的 Frame 对象。
     * @param parent 直接传递给 FloatingWindow 的构造函数。
     * @param suggestedGeometry 建议的初始几何位置和大小，默认为空。
     * @return 指向新创建的 FloatingWindow 实例的指针。
     */
    virtual FloatingWindow *createFloatingWindow(Frame *frame, MainWindowBase *parent = nullptr, QRect suggestedGeometry = {}) const = 0;

    /**
     * @brief 由框架内部调用以创建一个 DropIndicatorOverlayInterface 实例。
     * 重写此方法以提供您自己的 DropIndicatorOverlayInterface 子类。
     * @param dropArea 直接传递给 DropIndicatorOverlayInterface 的构造函数。
     * @return 指向新创建的 DropIndicatorOverlayInterface 实例的指针。
     */
    virtual DropIndicatorOverlayInterface *createDropIndicatorOverlay(DropArea *dropArea) const = 0;

    /**
     * @brief 由框架内部调用以创建一个用于显示为放置区域的橡皮筋 (RubberBand)。
     * @param parent 橡皮筋的父 QWidgetOrQuick。
     * @return 指向新创建的 QWidgetOrQuick (橡皮筋) 实例的指针。
     */
    virtual QWidgetOrQuick *createRubberBand(QWidgetOrQuick *parent) const = 0;

    /**
     * @brief 由框架内部调用以创建一个 SideBar 实例。
     * @param loc 侧边栏在主窗口中的位置。直接传递给您的 SideBar 子类的构造函数。
     * @param parent 主窗口。直接传递给您的 SideBar 子类的构造函数。
     * @return 指向新创建的 SideBar 实例的指针。
     */
    virtual SideBar *createSideBar(SideBarLocation loc, MainWindowBase *parent) const = 0;

#ifdef KDDOCKWIDGETS_QTWIDGETS // 以下部分仅在 Qt Widgets 环境下编译
    /**
     * @brief 由框架内部调用以创建一个标题栏按钮。
     * @param parent 按钮的父 QWidget。
     * @param type 要创建的按钮类型 (例如关闭、浮动等)。
     * @return 指向新创建的 QAbstractButton 实例的指针。
     */
    virtual QAbstractButton *createTitleBarButton(QWidget *parent, TitleBarButtonType type) const = 0;
#else // 以下部分在 QtQuick 环境下编译
    /**
     * @brief (仅 QtQuick) 返回标题栏 QML 文件的 URL。
     * @return QUrl 指向 QML 文件。
     */
    virtual QUrl titleBarFilename() const = 0;
    /**
     * @brief (仅 QtQuick) 返回停靠小部件 QML 文件的 URL。
     * @return QUrl 指向 QML 文件。
     */
    virtual QUrl dockwidgetFilename() const = 0;
    /**
     * @brief (仅 QtQuick) 返回框架 QML 文件的 URL。
     * @return QUrl 指向 QML 文件。
     */
    virtual QUrl frameFilename() const = 0;
    /**
     * @brief (仅 QtQuick) 返回浮动窗口 QML 文件的 URL。
     * @return QUrl 指向 QML 文件。
     */
    virtual QUrl floatingWindowFilename() const = 0;
#endif

    /**
     * @brief 返回用于指定按钮类型的图标。
     * @param type 按钮的类型 (例如关闭、浮动等)。
     * @param dpr 按钮所在屏幕的设备像素比，用于选择高清图标。
     * @return 对应按钮类型的 QIcon。
     */
    [[nodiscard]] virtual QIcon iconForButtonType(TitleBarButtonType type, qreal dpr) const = 0;

private:
    Q_DISABLE_COPY(FrameworkWidgetFactory) // 禁止拷贝构造函数和拷贝赋值操作符。
};

/**
 * @brief 如果用户没有指定自定义工厂，则使用此默认的小部件工厂。
 *
 * DefaultWidgetFactory 提供了 KDDockWidgets 框架中所有可自定义小部件的默认实现。
 * 用户如果只想修改部分小部件的创建行为，可以从此类派生并重写相应的方法。
 */
class DOCKS_EXPORT DefaultWidgetFactory : public FrameworkWidgetFactory
{
    Q_OBJECT
public:
    /**
     * @brief 默认构造函数。
     */
    DefaultWidgetFactory() = default;
    // 重写 FrameworkWidgetFactory 中的所有纯虚方法，提供默认实现
    Frame *createFrame(QWidgetOrQuick *parent, FrameOptions options) const override;
    TitleBar *createTitleBar(Frame *frame) const override;
    TitleBar *createTitleBar(FloatingWindow *floatingWindow) const override;
    TabWidget *createTabWidget(Frame *parent, TabWidgetOptions options = TabWidgetOption_None) const override;
    TabBar *createTabBar(TabWidget *parent) const override;
    Layouting::Separator *createSeparator(Layouting::Widget *parent = nullptr) const override;
    FloatingWindow *createFloatingWindow(MainWindowBase *parent = nullptr, FloatingWindowFlags flags = FloatingWindowFlag::FromGlobalConfig) const override;
    FloatingWindow *createFloatingWindow(Frame *frame, MainWindowBase *parent = nullptr, QRect suggestedGeometry = {}) const override;
    DropIndicatorOverlayInterface *createDropIndicatorOverlay(DropArea *dropArea) const override;
    QWidgetOrQuick *createRubberBand(QWidgetOrQuick *parent) const override;
    SideBar *createSideBar(SideBarLocation loc, MainWindowBase *parent) const override;

#ifdef KDDOCKWIDGETS_QTWIDGETS
    QAbstractButton *createTitleBarButton(QWidget *parent, TitleBarButtonType type) const override;
#else
    /**
     * @brief (仅 QtQuick, 可调用) 返回标题栏 QML 文件的 URL。
     * @return QUrl 指向 QML 文件。
     */
    Q_INVOKABLE QUrl titleBarFilename() const override;
    /**
     * @brief (仅 QtQuick) 返回停靠小部件 QML 文件的 URL。
     * @return QUrl 指向 QML 文件。
     */
    QUrl dockwidgetFilename() const override;
    /**
     * @brief (仅 QtQuick) 返回框架 QML 文件的 URL。
     * @return QUrl 指向 QML 文件。
     */
    QUrl frameFilename() const override;
    /**
     * @brief (仅 QtQuick) 返回浮动窗口 QML 文件的 URL。
     * @return QUrl 指向 QML 文件。
     */
    QUrl floatingWindowFilename() const override;
#endif

    /**
     * @brief (重写) 返回用于指定按钮类型的图标。
     * @param type 按钮的类型。
     * @param dpr 设备像素比。
     * @return 对应按钮类型的 QIcon。
     */
    QIcon iconForButtonType(TitleBarButtonType type, qreal dpr) const override;
    /**
     * @brief 清除图标缓存。
     * 当主题或图标资源可能发生变化时调用。
     */
    void clearIconCache();

    /// @brief (静态成员) 指定默认使用的拖放指示器类型。
    static DropIndicatorType s_dropIndicatorType;

private:
    Q_DISABLE_COPY(DefaultWidgetFactory) // 禁止拷贝构造函数和拷贝赋值操作符。
    /// @brief 用于缓存已创建图标的可变成员，以提高性能。键是 (按钮类型, 设备像素比) 的组合。
    mutable QMap<std::pair<TitleBarButtonType, qreal>, QIcon> m_cachedIcons;
};

} // namespace KDDockWidgets

#endif // KDDOCKWIDGETS_FRAMEWORKWIDGETFACTORY_H
