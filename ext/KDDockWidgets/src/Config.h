#ifndef KD_DOCKWIDGETS_CONFIG_H
#define KD_DOCKWIDGETS_CONFIG_H

#include "docks_export.h"          // 导入导出宏定义
#include "KDDockWidgets.h"         // KDDockWidgets 公共头文件，包含核心枚举和类型定义

#include <qglobal.h> // Qt 全局定义，例如 Q_DECLARE_FLAGS

QT_BEGIN_NAMESPACE
// 前向声明 Qt 类
class QQmlEngine; // Qt QML 引擎类 (用于 QtQuick 集成)
class QSize;      // Qt 尺寸类
QT_END_NAMESPACE

namespace KDDockWidgets {

// 前向声明 KDDockWidgets 内部类
class DockWidgetBase;         // 停靠小部件基类
class MainWindowBase;         // 主窗口基类
class FrameworkWidgetFactory; // 框架小部件工厂类 (用于自定义UI组件)
class DropArea;               // 放置区域类

/**
 * @brief 函数指针类型定义，用于创建 DockWidgetBase 实例的工厂函数。
 * @param name 要创建的停靠小部件的唯一名称。
 * @return 指向新创建的 DockWidgetBase 实例的指针。
 */
typedef KDDockWidgets::DockWidgetBase *(*DockWidgetFactoryFunc)(const QString &name);
/**
 * @brief 函数指针类型定义，用于创建 MainWindowBase 实例的工厂函数。
 * @param name 要创建的主窗口的唯一名称。
 * @return 指向新创建的 MainWindowBase 实例的指针。
 */
typedef KDDockWidgets::MainWindowBase *(*MainWindowFactoryFunc)(const QString &name);

/**
 * @brief 函数指针类型定义，用于更细粒度地控制允许哪些小部件在何处被放置。
 *
 * 默认情况下，小部件可以被放置到外部和内部的左/右/上/下以及中心位置。
 * 客户端应用程序可以通过 setDropIndicatorAllowedFunc 提供一个 lambda 表达式，
 * 以阻止（通过返回 false）任何他们不希望的特定放置位置。
 *
 * @param location 要允许或禁止的拖放指示器位置。
 * @param source 正在被拖拽的停靠小部件列表。
 * @param target 已停靠的标签组内的目标停靠小部件列表。
 * @param dropArea 目标放置区域。可以属于 MainWindow 或 FloatingWindow。
 * @return 如果允许停靠，则返回 true。
 * @sa setDropIndicatorAllowedFunc
 */
typedef bool (*DropIndicatorAllowedFunc)(DropLocation location,
                                         const QVector<DockWidgetBase *> &source,
                                         const QVector<DockWidgetBase *> &target,
                                         DropArea *dropArea);

/**
 * @deprecated 请改用 DropIndicatorAllowedFunc。
 * @brief 函数指针类型定义，允许用户更细粒度地禁止某些停靠小部件组合成标签页。
 * @param source 正在被拖拽的停靠小部件列表。
 * @param target 已停靠的标签组内的目标停靠小部件列表。
 * @return 如果允许组合成标签页，则返回 true。
 * @sa setTabbingAllowedFunc
 */
typedef bool (*TabbingAllowedFunc)(const QVector<DockWidgetBase *> &source,
                                   const QVector<DockWidgetBase *> &target);

/**
 * @brief Config 类是一个单例，用于调整框架的某些行为。
 *
 * Setter 方法应仅在创建任何 DockWidget 或 MainWindow 之前调用，
 * 最好是在创建 QApplication 之后立即调用。
 */
class DOCKS_EXPORT Config
{
public:
    /**
     * @brief 返回 Config 单例实例。
     * @return 对 Config 单例的引用。
     */
    static Config &self();

    /**
     * @brief 析构函数，在应用程序关闭时调用。
     */
    ~Config();

    /**
     * @brief 用于调整某些行为的标志枚举，默认值为 Flag_Default。
     * @warning 只有默认值在所有平台上都得到支持。并非所有选项都能在所有窗口管理器上正常工作，
     * Qt 尽力抽象这些差异，但这只是一种尽力而为的尝试。对于任何更改窗口标志的选项尤其如此。
     */
    enum Flag {
        Flag_None = 0, ///< 未设置任何选项。
        Flag_NativeTitleBar = 1, ///< 在支持的操作系统（Windows 10, macOS）上启用原生操作系统标题栏，在其他系统上忽略。
        Flag_AeroSnapWithClientDecos = 2, ///< 已废弃。此行为现在是默认行为且无法关闭。在 Windows 10 上移动窗口使用原生移动，因为它在不同 HDPI 设置的屏幕间工作良好。没有理由使用手动的客户端/Qt 窗口移动。
        Flag_AlwaysTitleBarWhenFloating = 4, ///< 即使指定了 Flag_HideTitleBarWhenTabsVisible，浮动窗口也将拥有标题栏。如果未指定 Flag_HideTitleBarWhenTabsVisible，则此选项非必需，因为这已经是默认行为。
        Flag_HideTitleBarWhenTabsVisible = 8, ///< 如果有可见的标签页，则隐藏标题栏。标签栏中的空白区域将变为可拖拽。
        Flag_AlwaysShowTabs = 16, ///< 即使只有一个标签页，也总是显示标签栏。
        Flag_AllowReorderTabs = 32, ///< 允许用户通过拖拽来重新排序标签页。
        Flag_TabsHaveCloseButton = 64, ///< 标签页将带有关闭按钮。等同于 QTabWidget::setTabsClosable(true)。
        Flag_DoubleClickMaximizes = 128, ///< 双击标题栏将最大化浮动窗口，而不是重新停靠它。
        Flag_TitleBarHasMaximizeButton = 256, ///< 浮动时，标题栏将带有最大化/恢复按钮。此选项与浮动按钮互斥（因为许多应用程序的行为如此）。
        Flag_TitleBarIsFocusable = 512, ///< 您可以点击标题栏，它将聚焦到焦点范围内的最后一个获得焦点的小部件。如果没有先前获得焦点的小部件，则它会聚焦用户停靠小部件的客户区，该客户区应接受焦点或使用焦点代理。
        Flag_LazyResize = 1024, ///< 停靠小部件以惰性方式调整大小。实际的大小调整仅在释放鼠标按钮时发生。
        Flag_DontUseUtilityFloatingWindows = 0x1000, ///< 浮动窗口将使用 Qt::Window 而不是 Qt::Tool 标志。这意味着它们通常会出现在任务栏中。
        Flag_TitleBarHasMinimizeButton = 0x2000 | Flag_DontUseUtilityFloatingWindows, ///< 浮动时，标题栏将带有最小化按钮。这也隐含了 Flag_DontUseUtilityFloatingWindows，否则它们不会出现在任务栏中。
        Flag_TitleBarNoFloatButton = 0x4000, ///< 标题栏将不显示浮动/停靠按钮。
        Flag_AutoHideSupport = 0x8000 | Flag_TitleBarNoFloatButton, ///< 支持将停靠小部件最小化到侧边栏（自动隐藏）。
                                                                    ///< 默认情况下，它也会关闭浮动按钮，但您可以移除 Flag_TitleBarNoFloatButton 以同时拥有两者。
        Flag_KeepAboveIfNotUtilityWindow = 0x10000, ///< 仅当设置了 Flag_DontUseUtilityFloatingWindows 时才有意义。如果浮动窗口是普通窗口，您可能仍希望它们保持在最前端，并且在主窗口获得焦点时不会最小化。
        Flag_CloseOnlyCurrentTab = 0x20000, ///< 标题栏的关闭按钮将仅关闭当前标签页，而不是关闭所有标签页。
        Flag_ShowButtonsOnTabBarIfTitleBarHidden = 0x40000, ///< 当使用 Flag_HideTitleBarWhenTabsVisible 时，关闭/浮动按钮会随标题栏一起消失。使用此标志后，它们将显示在标签栏上。
        Flag_AllowSwitchingTabsViaMenu = 0x80000, ///< 允许通过在标签区域右键单击上下文菜单来切换标签页。
        Flag_Default = Flag_AeroSnapWithClientDecos ///< 默认标志组合。
    };
    Q_DECLARE_FLAGS(Flags, Flag) // 将 Flag 枚举声明为可用于 QFlags 的类型。

    /**
     * @brief 可自定义小部件的列表。
     */
    enum CustomizableWidget {
        CustomizableWidget_None = 0,           ///< 无。
        CustomizableWidget_TitleBar,           ///< 标题栏。
        CustomizableWidget_DockWidget,         ///< 停靠小部件本身 (通常指其内容区域或外观)。
        CustomizableWidget_Frame,              ///< 容纳一组（一个或多个）标签化停靠小部件的容器。
        CustomizableWidget_TabBar,             ///< 标签栏，Frame 的子控件，包含一个或多个停靠小部件的标签。
        CustomizableWidget_TabWidget,          ///< 与标签栏相关的标签页小部件。
        CustomizableWidget_FloatingWindow,     ///< 顶层窗口，容纳一个或多个并排嵌套的 Frame。
        CustomizableWidget_Separator           ///< 布局中停靠小部件之间的可拖拽分隔符。
    };
    Q_DECLARE_FLAGS(CustomizableWidgets, CustomizableWidget) // 将 CustomizableWidget 枚举声明为可用于 QFlags 的类型。

    /**
     * @internal
     * @brief 用于额外调整的内部标志。
     * @warning 不供公众使用，支持将受到限制。
     */
    enum InternalFlag {
        InternalFlag_None = 0,                            ///< 默认值。
        InternalFlag_NoAeroSnap = 1,                      ///< 仅供开发使用。禁用 Aero-snap。
        InternalFlag_DontUseParentForFloatingWindows = 2, ///< 浮动窗口将没有顶层父窗口。
        InternalFlag_DontUseQtToolWindowsForFloatingWindows = 4, ///< 浮动窗口将使用 Qt::Window 而不是 Qt::Tool。
        InternalFlag_DontShowWhenUnfloatingHiddenWindow = 8, ///< 如果窗口已隐藏，DockWidget::setFloating(false) 将不执行任何操作。
        InternalFlag_UseTransparentFloatingWindow = 16,   ///< 仅适用于 QtQuick。允许圆角。与原生 Windows 拖放阴影一起使用时不稳定。
        InternalFlag_DisableTranslucency = 32,            ///< KDDW 尝试检测您的窗口管理器是否不支持透明窗口，但对于更奇特的设置，检测可能会失败。此标志可用于覆盖检测结果。
        InternalFlag_TopLevelIndicatorRubberBand = 64     ///< 使经典拖放指示器的橡皮筋成为顶层窗口。有助于解决 MFC 的 BUG。
    };
    Q_DECLARE_FLAGS(InternalFlags, InternalFlag) // 将 InternalFlag 枚举声明为可用于 QFlags 的类型。

    /**
     * @brief 返回当前选择的标志。
     * @return Flags 枚举值的组合。
     */
    [[nodiscard]] Flags flags() const;

    /**
     * @brief 设置行为标志。
     * @param flags 要设置的标志组合。
     * 并非所有标志都能保证被设置，因为操作系统可能不支持它们。
     * 如果需要知道实际设置了哪些标志，请在调用此setter后调用 @ref flags()。
     */
    void setFlags(Flags flags);

    /**
     * @brief 注册一个 DockWidgetFactoryFunc。
     *
     * 这是可选的，默认值为 nullptr。
     * DockWidgetFactoryFunc 是一个接收停靠小部件名称并返回 DockWidget 实例的函数。
     * 在恢复布局时，@ref LayoutSaver 要求所有停靠小部件都存在。
     * 如果某个 DockWidget 不存在，则需要一个 DockWidgetFactoryFunc 函数，
     * 以便布局保存器可以请求创建该 DockWidget 然后恢复它。
     * @param func 指向工厂函数的指针。
     */
    void setDockWidgetFactoryFunc(DockWidgetFactoryFunc func);

    /**
     * @brief 返回已注册的 DockWidgetFactoryFunc。
     * 默认为 nullptr。
     * @return 指向工厂函数的指针。
     */
    [[nodiscard]] DockWidgetFactoryFunc dockWidgetFactoryFunc() const;

    /**
     * @brief 与 DockWidgetFactoryFunc 对应，但用于主窗口。
     * 应该很少使用。在恢复布局之前拥有主窗口是良好的做法。
     * 它在这里是为了可以在 linter 可执行文件中使用。
     * @param func 指向主窗口工厂函数的指针。
     */
    void setMainWindowFactoryFunc(MainWindowFactoryFunc func);

    /**
     * @brief 返回已注册的 MainWindowFactoryFunc。
     * 默认为 nullptr。
     * @return 指向主窗口工厂函数的指针。
     */
    [[nodiscard]] MainWindowFactoryFunc mainWindowFactoryFunc() const;

    /**
     * @brief 设置小部件工厂 (WidgetFactory)。
     *
     * 默认使用 DefaultWidgetFactory，它提供 FrameWidget、TitleBarWidget、
     * TabBarWidget、TabWidgetWidget 等。您可以设置自己的工厂，以提供这些类的自定义变体，
     * 用于更改 GUI 外观等目的。
     *
     * 也可能用于返回 QtQuick 类而不是基于 QtWidget 的类。
     * KDDockWidgets 将获得所提供工厂的所有权。
     * @param factory 指向 FrameworkWidgetFactory 实例的指针。
     */
    void setFrameworkWidgetFactory(FrameworkWidgetFactory *factory);

    /**
     * @brief 获取框架小部件工厂。
     * @return 指向 FrameworkWidgetFactory 实例的指针。
     */
    [[nodiscard]] FrameworkWidgetFactory *frameworkWidgetFactory() const;

    /**
     * @brief 返回分隔符的厚度。
     *
     * 默认为 5 像素。
     * @return 分隔符厚度（像素）。
     */
    [[nodiscard]] static int separatorThickness();

    /**
     * @brief 设置分隔符的厚度。
     * 注意：仅在启动时，在创建任何 DockWidget 或 MainWindow 之前使用此函数。
     * @param value 新的厚度值（像素）。
     */
    static void setSeparatorThickness(int value);

    /**
     * @brief 设置被拖拽窗口的不透明度。
     * 1.0 表示完全不透明，0.0 表示完全透明。
     * @param opacity 不透明度值。
     */
    void setDraggedWindowOpacity(qreal opacity);

    /**
     * @brief 返回拖拽停靠小部件时使用的不透明度。
     * 默认为 1.0 (完全不透明)。
     * @return 不透明度值。
     */
    [[nodiscard]] qreal draggedWindowOpacity() const;

    /**
     * @brief 允许在拖拽时禁用拖放指示器的支持。
     * 默认情况下，在拖拽停靠小部件时会显示拖放指示器。
     * 此功能可以在需要时随时切换（它不是仅限启动时设置）。
     * @param inhibit 如果为 true，则禁止显示指示器。
     */
    void setDropIndicatorsInhibited(bool inhibit) const;

    /**
     * @brief 返回拖放指示器是否被禁止。
     * 默认情况下为 false，除非调用了 setDropIndicatorsInhibited(true)。
     * @return 如果被禁止，则返回 true。
     */
    [[nodiscard]] bool dropIndicatorsInhibited() const;

    /**
     * @deprecated 请改用 setDropIndicatorAllowedFunc()，并处理 DropLocation_Center 的情况。
     *
     * @brief 允许用户拦截尝试将小部件停靠到中心（形成标签页）的操作并禁止它。
     *
     * 每当用户尝试将两个小部件组合成标签页时，框架将调用 @p func。如果
     * 它返回 true，则允许组合；否则不允许。
     *
     * 示例:
     * @code
     * #include <kddockwidgets/Config.h>
     * (...)
     *
     * auto func = [] (const KDDockWidgets::DockWidgetBase::List &source,
     * const KDDockWidgets::DockWidgetBase::List &target)
     * {
     * // 禁止 dockFoo 与 dockBar 组合成标签页。
     * return !(source.contains(dockFoo) && target.contains(dockBar));
     * };
     *
     * KDDockWidgets::Config::self().setTabbingAllowedFunc(func);
     *
     * @endcode
     * @param func 用户提供的回调函数。
     */
    void setTabbingAllowedFunc(TabbingAllowedFunc func);

    /**
     * @brief 由框架内部使用。返回传递给 setTabbingAllowedFunc() 的函数。
     * 默认为 nullptr。
     * @sa setTabbingAllowedFunc()
     * @return 指向 TabbingAllowedFunc 的指针。
     */
    [[nodiscard]] TabbingAllowedFunc tabbingAllowedFunc() const;

    /**
     * @brief 允许客户端应用程序禁止某些停靠指示器。
     *
     * 例如，假设应用程序不希望为某个特定的停靠小部件显示外部指示器。
     *
     * @code
     * #include <kddockwidgets/Config.h>
     * (...)
     *
     * auto func = [] (KDDockWidgets::DropLocation loc,
     * const KDDockWidgets::DockWidgetBase::List &source,
     * const KDDockWidgets::DockWidgetBase::List &target,
     * KDDockWidgets::DropArea *)
     * {
     * // 禁止 dockFoo 停靠到外部区域。
     * return !((loc & KDDockWidgets::DropLocation_Outter) && source.contains(dockFoo));
     * };
     *
     * KDDockWidgets::Config::self().setDropIndicatorAllowedFunc(func);
     *
     * @endcode
     *
     * 运行 "kddockwidgets_example --hide-certain-docking-indicators" 查看实际效果。
     * @param func 用户提供的回调函数。
     */
    void setDropIndicatorAllowedFunc(DropIndicatorAllowedFunc func);

    /**
     * @brief 由框架内部使用。返回传递给 setDropIndicatorAllowedFunc() 的函数。
     * 默认为 nullptr。
     * @sa setDropIndicatorAllowedFunc()
     * @return 指向 DropIndicatorAllowedFunc 的指针。
     */
    [[nodiscard]] DropIndicatorAllowedFunc dropIndicatorAllowedFunc() const;

    /**
     * @brief 设置停靠小部件可以具有的最小尺寸。
     * 小部件仍然可以提供自己的最小尺寸，并且会得到尊重，但是它永远不能小于此值。
     * @param size 绝对最小尺寸。
     */
    static void setAbsoluteWidgetMinSize(QSize size);
    /**
     * @brief 获取停靠小部件的绝对最小尺寸。
     * @return 绝对最小 QSize。
     */
    [[nodiscard]] static QSize absoluteWidgetMinSize();

    /**
     * @brief 设置停靠小部件可以具有的最大尺寸。
     * 小部件仍然可以提供自己的最大尺寸，并且会得到尊重，但是它永远不能大于此值。
     * @param size 绝对最大尺寸。
     */
    static void setAbsoluteWidgetMaxSize(QSize size);
    /**
     * @brief 获取停靠小部件的绝对最大尺寸。
     * @return 绝对最大 QSize。
     */
    [[nodiscard]] static QSize absoluteWidgetMaxSize();

    /**
     * @brief 禁用 KDDockWidget 内部小部件的 paintEvent() 事件。
     * 默认情况下，KDDockWidget 的内部小部件会重新实现 paintEvent()。禁用它们
     * （这将导致调用基类 QWidget::paintEvent()）在您希望通过 CSS 样式表进行样式设置时可能很有用。
     * @param widgets 要禁用其 paintEvent 的可自定义小部件的组合。
     */
    void setDisabledPaintEvents(CustomizableWidgets widgets);
    /**
     * @brief 获取当前已禁用其 paintEvent 的可自定义小部件的组合。
     * @return Config::CustomizableWidgets 的组合。
     */
    [[nodiscard]] Config::CustomizableWidgets disabledPaintEvents() const;

    /**
     * @internal
     * @brief 返回内部标志。
     * @warning 不供公众使用，支持将受到限制。
     * @return InternalFlags 的组合。
     */
    [[nodiscard]] InternalFlags internalFlags() const;

    /**
     * @internal
     * @brief 设置内部标志。
     * @warning 不供公众使用，支持将受到限制。
     * @param flags 要设置的内部标志组合。
     */
    void setInternalFlags(InternalFlags flags);

    /**
     * @brief 设置 MDI 弹出阈值。当布局为 MDI 且您将停靠小部件拖动到窗口边缘之外 X 像素时，它将使停靠小部件浮动。
     * 默认情况下，此值为 250 像素。使用 -1 可禁用此功能。
     * @param threshold 阈值（像素），或 -1 禁用。
     */
    void setMDIPopupThreshold(int threshold);
    /**
     * @brief 获取 MDI 弹出阈值。
     * @return MDI 弹出阈值（像素）。
     */
    [[nodiscard]] int mdiPopupThreshold() const;

#ifdef KDDOCKWIDGETS_QTQUICK // 以下部分仅在 QtQuick 环境下编译
    /**
     * @brief 设置要使用的 QQmlEngine。仅在使​​用 QtQuick 时适用。
     * @param engine 指向 QQmlEngine 实例的指针。
     */
    void setQmlEngine(QQmlEngine *engine);
    /**
     * @brief 获取配置的 QQmlEngine。
     * @return 指向 QQmlEngine 实例的指针。
     */
    QQmlEngine *qmlEngine() const;
#endif

private:
    Q_DISABLE_COPY(Config) ///< 禁止拷贝构造函数和拷贝赋值操作符。
    /**
     * @brief 私有构造函数，确保单例模式。
     */
    Config();
    class Private;   ///< PIMPL (Private Implementation) 设计模式的私有实现类前向声明。
    Private *const d; ///< 指向私有实现类的常量指针。
};

} // namespace KDDockWidgets

// 为 KDDockWidgets::Config::Flags 声明 QFlags 操作符，使其可以像位掩码一样使用。
Q_DECLARE_OPERATORS_FOR_FLAGS(KDDockWidgets::Config::Flags)
Q_DECLARE_OPERATORS_FOR_FLAGS(KDDockWidgets::Config::InternalFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(KDDockWidgets::Config::CustomizableWidgets)


#endif // KD_DOCKWIDGETS_CONFIG_H
