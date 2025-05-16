/**
 * @file
 * @brief 包含 KDDockWidgets 命名空间级别的枚举和方法的文件。
 *
 * @author Sérgio Martins <sergio.martins@kdab.com>
 */

#ifndef KD_KDDOCKWIDGETS_H
#define KD_KDDOCKWIDGETS_H

#include "docks_export.h"         // 导入导出宏定义，用于库的符号可见性
#include "Qt5Qt6Compat_p.h"       // Qt5/Qt6 兼容性相关的私有助手宏或类型定义

#include <QObject> // Qt 对象模型基类
#include <QDebug>  // Qt 调试输出类

#ifdef Q_OS_WIN // 条件编译：仅在 Windows 操作系统下
// 仅在 Windows 上，此功能较为流行。在 Linux 上，Qt::Tool 窗口需要重新设置父窗口。在 macOS 上未经测试。
#define KDDOCKWIDGETS_SUPPORTS_NESTED_MAINWINDOWS // 定义宏，表示支持嵌套主窗口的特性
#endif

// 前向声明内部布局引擎相关的类
namespace Layouting {
class Item;             // 布局中的一个基本项
class ItemBoxContainer; // 布局中可以包含其他项的盒式容器
}

namespace KDDockWidgets {
DOCKS_EXPORT // 导出 KDDockWidgets 命名空间 (如果需要)
Q_NAMESPACE  // 将此命名空间注册到 Qt 元对象系统，允许在 QML 和属性系统中使用其枚举

// 前向声明 KDDockWidgets 内部或公共类
class MultiSplitter; // 多重分割器类
class DropArea;      // 放置区域类

/**
 * @brief 定义停靠小部件可以停靠到的位置。
 */
enum Location {
    Location_None,      ///< 无特定位置，或表示无效位置。
    Location_OnLeft,    ///< 左侧停靠位置。
    Location_OnTop,     ///< 顶部停靠位置。
    Location_OnRight,   ///< 右侧停靠位置。
    Location_OnBottom   ///< 底部停靠位置。
};
Q_ENUM_NS(Location) // 将 Location 枚举注册到 Qt 元对象系统 (在 KDDockWidgets 命名空间内)。

/**
 * @brief 主窗口 (MainWindow) 的构造选项。
 */
enum MainWindowOption {
    MainWindowOption_None = 0, ///< 无选项设置。
    MainWindowOption_HasCentralFrame = 1, ///< 使主窗口总是拥有一个中央框架，用于以标签页形式组织文档或视图。
    MainWindowOption_MDI = 2, ///< 布局将是 MDI (多文档界面) 风格。停靠小部件可以拥有任意位置，不受传统分割布局的限制。
    MainWindowOption_HasCentralWidget = 4 | MainWindowOption_HasCentralFrame, ///< 类似于 MainWindowOption_HasCentralFrame，
                                                                            ///< 但您将拥有一个不能被分离的中央小部件（类似于常规的 QMainWindow）。
                                                                            ///< @sa MainWindowBase::setPersistentCentralWidget()
};
Q_DECLARE_FLAGS(MainWindowOptions, MainWindowOption) // 将 MainWindowOption 枚举声明为可用于 QFlags 的类型。
Q_ENUM_NS(MainWindowOptions) // 将 MainWindowOptions 枚举注册到 Qt 元对象系统。

/**
 * @internal
 * @brief 描述布局引擎的一些尺寸调整策略。
 * 这是内部使用的枚举。处理尺寸调整的公共 API 是 InitialOption。
 * @sa InitialOption
 */
enum class DefaultSizeMode {
    ItemSize,     ///< 简单地使用被添加项的 Item::size()。如果窗口不够大，实际使用的尺寸可能会更小。
    Fair,         ///< 给予与布局中已存在项相等的相对尺寸。
    FairButFloor, ///< 等同于 Fair，但如果正在添加的项小于 Fair 建议的尺寸，则使用该较小尺寸。
    NoDefaultSizeMode, ///< 不执行任何默认的尺寸调整。
};
Q_ENUM_NS(DefaultSizeMode) // 将 DefaultSizeMode 枚举注册到 Qt 元对象系统。

/**
 * @brief 仅为与 v1.2 版本源码兼容而保留。请勿使用。
 * 请改用 InitialVisibilityOption。
 */
enum AddingOption {
    AddingOption_None = 0,        ///< 无选项。
    AddingOption_StartHidden      ///< 添加时初始状态为隐藏。
};
Q_ENUM_NS(AddingOption) // 将 AddingOption 枚举注册到 Qt 元对象系统。

/**
 * @brief 定义停靠小部件添加到布局时的初始可见性选项。
 */
enum class InitialVisibilityOption {
    StartVisible = 0,       ///< 停靠小部件在停靠时立即使其可见。
    StartHidden,            ///< 添加停靠小部件时不显示它（例如，仅创建占位符）。
    PreserveCurrentTab      ///< 当作为标签页添加时，不改变当前激活的标签页索引。
};
Q_ENUM_NS(InitialVisibilityOption) // 将 InitialVisibilityOption 枚举注册到 Qt 元对象系统。

/**
 * @brief 结构体，描述了将停靠小部件添加到布局时的首选尺寸和可见性。
 *
 * 您可以将此结构体传递给 MainWindowBase::addDockWidget() 以提供首选尺寸和可见性的提示。
 *
 * 请参见下面 InitialOption::visibility 和 InitialOption::preferredSize 的文档。
 *
 * @sa MainWindowBase::addDockWidget()
 */
struct InitialOption
{
    // 为方便起见提供的隐式构造函数：

    /**
     * @brief 默认构造函数。
     * visibility 默认为 StartVisible，preferredSize 默认为无效尺寸。
     */
    InitialOption() = default;

    /**
     * @brief 使用初始可见性选项构造。
     * @param v 初始可见性选项。
     */
    explicit InitialOption(InitialVisibilityOption v)
        : visibility(v)
    {
    }

    /**
     * @brief 使用首选尺寸构造。
     * @param size 首选尺寸。
     */
    explicit InitialOption(QSize size)
        : preferredSize(size)
    {
    }

    /**
     * @brief 使用初始可见性选项和首选尺寸构造。
     * @param v 初始可见性选项。
     * @param size 首选尺寸。
     */
    InitialOption(InitialVisibilityOption v, QSize size)
        : visibility(v)
        , preferredSize(size)
    {
    }

    /**
     * @brief (已废弃) 使用 AddingOption 构造。
     * AddingOption 已废弃，将在 v1.5 版本中移除。请改用 InitialVisibilityOption。
     * @param opt 旧的 AddingOption。
     */
    QT_DEPRECATED_X("AddingOption is deprecated and will be removed in v1.5. Use InitialVisibilityOption instead.")
    explicit InitialOption(AddingOption opt)
        : visibility(opt == AddingOption_StartHidden ? InitialVisibilityOption::StartHidden
                                                     : InitialVisibilityOption::StartVisible)
    {
    }

    /**
     * @brief 检查停靠小部件是否以隐藏状态开始。
     * @return 如果 visibility 是 InitialVisibilityOption::StartHidden，则返回 true。
     */
    [[nodiscard]] bool startsHidden() const
    {
        return visibility == InitialVisibilityOption::StartHidden;
    }

    /**
     * @brief 检查在添加为标签页时是否保留当前标签页。
     * @return 如果 visibility 是 InitialVisibilityOption::PreserveCurrentTab，则返回 true。
     */
    [[nodiscard]] bool preservesCurrentTab() const
    {
        return visibility == InitialVisibilityOption::PreserveCurrentTab;
    }

    /**
     * @brief 获取指定方向上的首选长度。
     * @param o 方向 (Qt::Horizontal 或 Qt::Vertical)。
     * @return 如果是水平方向，则返回 preferredSize.width()；否则返回 preferredSize.height()。
     */
    [[nodiscard]] int preferredLength(Qt::Orientation o) const
    {
        return o == Qt::Horizontal ? preferredSize.width()
                                   : preferredSize.height();
    }

    /**
     * @brief 检查是否为指定方向设置了有效的首选长度。
     * @param o 方向。
     * @return 如果对应方向的首选长度大于0，则返回 true。
     */
    [[nodiscard]] bool hasPreferredLength(Qt::Orientation o) const
    {
        return preferredLength(o) > 0;
    }

    /**
     * @brief 允许停靠小部件在停靠时处于隐藏状态。
     *
     * 下次调用 DockWidget::show() 时，它将显示在那个位置。
     * 这避免了闪烁，因为不需要 show()/hide() 的变通方法。
     */
    InitialVisibilityOption visibility = InitialVisibilityOption::StartVisible;

    /**
     * @brief 允许控制停靠小部件在停靠时应具有的尺寸。
     *
     * 如果传递了无效或空的尺寸，则 KDDW 会应用其默认的启发式方法。
     *
     * 注意，通常只有宽度或高度会被采纳：例如，当将停靠小部件添加到左侧时，
     * 只有首选宽度会被考虑，因为高度将简单地填充整个布局。
     */
    QSize preferredSize;

private:
    // 声明友元类，允许它们访问私有成员
    friend class Layouting::Item;
    friend class Layouting::ItemBoxContainer;
    friend class KDDockWidgets::MultiSplitter;
    friend class KDDockWidgets::DropArea;

    /**
     * @brief (私有) 使用 DefaultSizeMode 构造。
     * @param mode 尺寸调整模式。
     */
    explicit InitialOption(DefaultSizeMode mode)
        : sizeMode(mode)
    {
    }

    /// @brief 内部使用的尺寸调整模式，默认为 Fair。
    DefaultSizeMode sizeMode = DefaultSizeMode::Fair;
};

/**
 * @brief 定义恢复布局时的选项。
 */
enum RestoreOption {
    RestoreOption_None = 0,                            ///< 无特殊选项。
    RestoreOption_RelativeToMainWindow = 1 << 0,       ///< 跳过恢复主窗口的几何形状，恢复的停靠小部件将使用相对尺寸。
                                                       ///< 加载布局不会改变主窗口的几何形状，而只使用用户当前的设置。
    RestoreOption_AbsoluteFloatingDockWindows = 1 << 1, ///< 跳过相对于主窗口缩放浮动停靠窗口的步骤。
};
Q_DECLARE_FLAGS(RestoreOptions, RestoreOption) // 将 RestoreOption 枚举声明为可用于 QFlags 的类型。
Q_ENUM_NS(RestoreOptions) // 将 RestoreOptions 枚举注册到 Qt 元对象系统。

/**
 * @brief 定义拖放指示器的类型。
 */
enum class DropIndicatorType {
    Classic,   ///< 默认类型，通常是箭头形状的指示器。
    Segmented, ///< 分段式指示器。
    None       ///< 拖拽时不显示任何拖放指示器。
};
Q_ENUM_NS(DropIndicatorType) // 将 DropIndicatorType 枚举注册到 Qt 元对象系统。

/**
 * @internal
 * @brief 建议几何形状时的提示。
 */
enum SuggestedGeometryHint {
    SuggestedGeometryHint_None = 0,                 ///< 无提示。
    SuggestedGeometryHint_PreserveCenter = 1,       ///< 在调整大小时尽量保持中心点不变。
    SuggestedGeometryHint_GeometryIsFromDocked = 2  ///< 几何形状是从停靠状态转换而来的（例如，浮动时）。
};
Q_DECLARE_FLAGS(SuggestedGeometryHints, SuggestedGeometryHint) // 将 SuggestedGeometryHint 枚举声明为可用于 QFlags 的类型。
Q_ENUM_NS(SuggestedGeometryHint) // 将 SuggestedGeometryHint 枚举注册到 Qt 元对象系统。

/**
 * @brief 每个主窗口支持四个侧边栏位置。
 */
enum class SideBarLocation {
    None,  ///< 无侧边栏位置，或表示不在侧边栏中。
    North, ///< 北侧（顶部）侧边栏。
    East,  ///< 东侧（右侧）侧边栏。
    West,  ///< 西侧（左侧）侧边栏。
    South  ///< 南侧（底部）侧边栏。
};
// Q_ENUM_NS(SideBarLocation) // 注意：此枚举似乎未在此处使用 Q_ENUM_NS 注册，可能在其他地方或有意为之。

/**
 * @brief 描述了标题栏上可以拥有的按钮类型。
 */
enum class TitleBarButtonType {
    Close,      ///< 关闭按钮。
    Float,      ///< 浮动/停靠切换按钮。
    Minimize,   ///< 最小化按钮。
    Maximize,   ///< 最大化按钮。
    Normal,     ///< 从最大化状态恢复到正常大小的按钮。
    AutoHide,   ///< 自动隐藏（最小化到侧边栏）按钮。
    UnautoHide  ///< 从自动隐藏状态恢复的按钮。
};
Q_ENUM_NS(TitleBarButtonType) // 将 TitleBarButtonType 枚举注册到 Qt 元对象系统。

/**
 * @brief 枚举，描述了不同的拖放指示器位置。
 *
 * 这些值可以组合使用（位操作），表示多个可能的放置区域。
 */
enum DropLocation {
    DropLocation_None = 0,         ///< 无放置位置。
    DropLocation_Left = 1,         ///< 内部左侧。
    DropLocation_Top = 2,          ///< 内部顶部。
    DropLocation_Right = 4,        ///< 内部右侧。
    DropLocation_Bottom = 8,       ///< 内部底部。
    DropLocation_Center = 16,      ///< 中心区域（通常用于标签化停靠）。
    DropLocation_OutterLeft = 32,  ///< 外部左侧。
    DropLocation_OutterTop = 64,   ///< 外部顶部。
    DropLocation_OutterRight = 128,///< 外部右侧。
    DropLocation_OutterBottom = 256,///< 外部底部。
    // 组合值
    DropLocation_Inner = DropLocation_Left | DropLocation_Right | DropLocation_Top | DropLocation_Bottom, ///< 所有内部位置。
    DropLocation_Outter = DropLocation_OutterLeft | DropLocation_OutterRight | DropLocation_OutterTop | DropLocation_OutterBottom, ///< 所有外部位置。
    DropLocation_Horizontal = DropLocation_Left | DropLocation_Right | DropLocation_OutterLeft | DropLocation_OutterRight, ///< 所有水平方向的位置。
    DropLocation_Vertical = DropLocation_Top | DropLocation_Bottom | DropLocation_OutterTop | DropLocation_OutterBottom ///< 所有垂直方向的位置。
};
Q_ENUM_NS(DropLocation) // 将 DropLocation 枚举注册到 Qt 元对象系统。

/**
 * @internal
 * @brief 为 SideBarLocation 提供 qHash 函数，使其可用作 QHash 的键。
 * @param loc 要计算哈希值的 SideBarLocation。
 * @param seed 哈希种子。
 * @return 计算得到的哈希值。
 */
inline Qt5Qt6Compat::qhashtype qHash(SideBarLocation loc, Qt5Qt6Compat::qhashtype seed)
{
    return ::qHash(static_cast<uint>(loc), seed); // 将枚举转换为 uint 后计算哈希
}

/**
 * @internal
 * @brief 定义鼠标光标在小部件边缘或角落时的位置，用于调整大小。
 */
enum CursorPosition {
    CursorPosition_Undefined = 0,      ///< 未定义或不在调整大小区域。
    CursorPosition_Left = 1,           ///< 左边缘。
    CursorPosition_Right = 2,          ///< 右边缘。
    CursorPosition_Top = 4,            ///< 上边缘。
    CursorPosition_Bottom = 8,         ///< 下边缘。
    CursorPosition_TopLeft = CursorPosition_Top | CursorPosition_Left,     ///< 左上角。
    CursorPosition_TopRight = CursorPosition_Top | CursorPosition_Right,   ///< 右上角。
    CursorPosition_BottomRight = CursorPosition_Bottom | CursorPosition_Right, ///< 右下角。
    CursorPosition_BottomLeft = CursorPosition_Bottom | CursorPosition_Left,   ///< 左下角。
    CursorPosition_Horizontal = CursorPosition_Right | CursorPosition_Left,   ///< 水平调整（左右边缘）。
    CursorPosition_Vertical = CursorPosition_Top | CursorPosition_Bottom,     ///< 垂直调整（上下边缘）。
    CursorPosition_All = CursorPosition_Left | CursorPosition_Right | CursorPosition_Top | CursorPosition_Bottom ///< 所有边缘（通常不直接使用，而是组合）。
};
Q_DECLARE_FLAGS(CursorPositions, CursorPosition) // 将 CursorPosition 枚举声明为可用于 QFlags 的类型。
Q_ENUM_NS(CursorPosition) // 将 CursorPosition 枚举注册到 Qt 元对象系统。


/**
 * @internal
 * @brief Frame (框架) 的选项。
 */
enum FrameOption {
    FrameOption_None = 0,             ///< 无选项。
    FrameOption_AlwaysShowsTabs = 1,  ///< 即使只有一个停靠小部件，也总是显示标签栏。
    FrameOption_IsCentralFrame = 2,   ///< 标记此框架是主窗口的中央框架。
    FrameOption_IsOverlayed = 4,      ///< 标记此框架是作为覆盖层显示的（例如，自动隐藏的侧边栏面板）。
    FrameOption_NonDockable = 8       ///< 不能通过拖放将其他东西停靠或标签化到此框架中。
};
Q_DECLARE_FLAGS(FrameOptions, FrameOption) // 将 FrameOption 枚举声明为可用于 QFlags 的类型。
Q_ENUM_NS(FrameOptions) // 将 FrameOptions 枚举注册到 Qt 元对象系统。

/**
 * @internal
 * @brief TabWidget (标签页小部件) 的选项。
 */
enum TabWidgetOption {
    TabWidgetOption_None = 0,         ///< 无选项。
    TabWidgetOption_DocumentMode = 1  ///< 启用 QTabWidget::documentMode() 样式。
};
Q_DECLARE_FLAGS(TabWidgetOptions, TabWidgetOption) // 将 TabWidgetOption 枚举声明为可用于 QFlags 的类型。
Q_ENUM_NS(TabWidgetOptions) // 将 TabWidgetOptions 枚举注册到 Qt 元对象系统。

/**
 * @internal
 * @brief FloatingWindow (浮动窗口) 的标志。
 */
enum class FloatingWindowFlag {
    None = 0,                                 ///< 无标志。
    FromGlobalConfig = 1,                     ///< 使用 KDDockWidgets::Config 中的全局配置。
    TitleBarHasMinimizeButton = 2,            ///< 标题栏带有最小化按钮。
    TitleBarHasMaximizeButton = 4,            ///< 标题栏带有最大化按钮。
    KeepAboveIfNotUtilityWindow = 8,          ///< 如果不是工具窗口 (Qt::Tool)，则保持在最前端。
    NativeTitleBar = 16,                      ///< 使用原生操作系统标题栏。
    HideTitleBarWhenTabsVisible = 32,         ///< 当标签页可见时隐藏标题栏。
    AlwaysTitleBarWhenFloating = 64,          ///< 浮动时总是显示标题栏（即使设置了 HideTitleBarWhenTabsVisible）。
    DontUseParentForFloatingWindows = 128,    ///< 浮动窗口不设置父顶层窗口。
    UseQtWindow = 256,                        ///< 浮动窗口使用 Qt::Window 标志（而不是 Qt::Tool）。
    UseQtTool = 512,                          ///< 浮动窗口使用 Qt::Tool 标志。
};
Q_DECLARE_FLAGS(FloatingWindowFlags, FloatingWindowFlag) // 将 FloatingWindowFlag 枚举声明为可用于 QFlags 的类型。

/**
 * @internal
 * @brief 将 Location 枚举值转换为对应的字符串表示。
 * @param loc 要转换的 Location 值。
 * @return 表示位置的 QString，如果 loc 无效则为空字符串。
 */
inline QString locationStr(Location loc)
{
    switch (loc) {
    case KDDockWidgets::Location_None:
        return QStringLiteral("none");
    case KDDockWidgets::Location_OnLeft:
        return QStringLiteral("left");
    case KDDockWidgets::Location_OnTop:
        return QStringLiteral("top");
    case KDDockWidgets::Location_OnRight:
        return QStringLiteral("right");
    case KDDockWidgets::Location_OnBottom:
        return QStringLiteral("bottom");
    }

    return {}; // 对于无效的 loc 或未处理的情况
}
} // namespace KDDockWidgets

QT_BEGIN_NAMESPACE
/**
 * @internal
 * @brief QDebug 操作符重载，用于方便地输出 KDDockWidgets::InitialOption 对象的信息。
 * @param d QDebug 对象。
 * @param o 要输出的 InitialOption 对象。
 * @return QDebug 对象，以便链式调用。
 */
inline QDebug operator<<(QDebug d, KDDockWidgets::InitialOption o)
{
    // 主要输出其是否以隐藏状态开始
    d.nospace() << "InitialOption(startsHidden=" << o.startsHidden()
                << ", preferredSize=" << o.preferredSize
                << ", preservesCurrentTab=" << o.preservesCurrentTab() << ')';
    return d;
}
QT_END_NAMESPACE

// 为 KDDockWidgets 命名空间内的 QFlags 类型声明操作符，使其可以像位掩码一样使用
Q_DECLARE_OPERATORS_FOR_FLAGS(KDDockWidgets::MainWindowOptions)
Q_DECLARE_OPERATORS_FOR_FLAGS(KDDockWidgets::RestoreOptions)
Q_DECLARE_OPERATORS_FOR_FLAGS(KDDockWidgets::SuggestedGeometryHints)
Q_DECLARE_OPERATORS_FOR_FLAGS(KDDockWidgets::CursorPositions)
Q_DECLARE_OPERATORS_FOR_FLAGS(KDDockWidgets::FrameOptions)
Q_DECLARE_OPERATORS_FOR_FLAGS(KDDockWidgets::TabWidgetOptions)
Q_DECLARE_OPERATORS_FOR_FLAGS(KDDockWidgets::FloatingWindowFlags)

// 将 KDDockWidgets::InitialVisibilityOption 类型注册到 Qt 元对象系统，以便可以在信号/槽和属性系统中使用。
Q_DECLARE_METATYPE(KDDockWidgets::InitialVisibilityOption)

#endif // KD_KDDOCKWIDGETS_H
