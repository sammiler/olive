#ifndef KD_TITLEBAR_P_H
#define KD_TITLEBAR_P_H

#include "kddockwidgets/docks_export.h" // 导入导出宏定义

#include "kddockwidgets/QWidgetAdapter.h" // QWidget 和 QQuickItem 的适配器类
#include "kddockwidgets/DockWidgetBase.h" // 停靠小部件基类
#include "Draggable_p.h" // Draggable 私有头文件 (可拖拽对象接口)
#include "Frame_p.h" // Frame 私有头文件 (停靠小部件的容器)

#include <QVector> // Qt 动态数组容器
#include <QIcon> // Qt 图标类

QT_BEGIN_NAMESPACE
// 前向声明 Qt 类
class QHBoxLayout; // Qt 水平布局类
class QLabel; // Qt 标签控件类
QT_END_NAMESPACE

// 前向声明，用于测试目的
class TestDocks;

namespace KDDockWidgets {

// 前向声明 KDDockWidgets 内部类
class DockWidgetBase; // 停靠小部件基类
class Frame; // 框架类 (容纳 DockWidgetBase)
class Button; // KDDockWidgets 内部按钮类 (可能用于标题栏按钮)
class TabBar; // 标签栏类

/**
 * @brief TitleBar 类代表停靠小部件或浮动窗口的标题栏。
 *
 * 它显示标题、图标，并包含诸如关闭、浮动/停靠、最大化/最小化等操作按钮。
 * TitleBar 同时也是一个可拖拽 (Draggable) 的对象，允许用户通过拖拽标题栏来移动窗口或停靠小部件。
 * 它根据其父对象（Frame 或 FloatingWindow）以及包含的 DockWidgetBase 的状态来调整其外观和行为。
 */
class DOCKS_EXPORT TitleBar : public QWidgetAdapter, public Draggable
{
    Q_OBJECT
    /// @brief Q_PROPERTY 宏，暴露 title 属性给 Qt 元对象系统，表示标题栏显示的文本。
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    /// @brief Q_PROPERTY 宏，暴露 hasIcon 属性给 Qt 元对象系统，表示标题栏是否显示图标。
    Q_PROPERTY(bool hasIcon READ hasIcon NOTIFY iconChanged)
    /// @brief Q_PROPERTY 宏，暴露 closeButtonEnabled 属性，控制关闭按钮是否启用。
    Q_PROPERTY(bool closeButtonEnabled READ closeButtonEnabled WRITE setCloseButtonEnabled NOTIFY closeButtonEnabledChanged)
    /// @brief Q_PROPERTY 宏，暴露 floatButtonVisible 属性，控制浮动/停靠按钮是否可见。
    Q_PROPERTY(bool floatButtonVisible READ floatButtonVisible WRITE setFloatButtonVisible NOTIFY floatButtonVisibleChanged)
    /// @brief Q_PROPERTY 宏，暴露 floatButtonToolTip 属性，表示浮动/停靠按钮的工具提示文本。
    Q_PROPERTY(QString floatButtonToolTip READ floatButtonToolTip NOTIFY floatButtonToolTipChanged)
    /// @brief Q_PROPERTY 宏，暴露 isFocused 属性，表示标题栏关联的内容区域是否拥有焦点。
    Q_PROPERTY(bool isFocused READ isFocused NOTIFY isFocusedChanged)
public:
    /// @brief TitleBar 指针的 QVector 类型定义，方便使用。
    typedef QVector<TitleBar *> List;

    /**
     * @brief 构造函数，用于 Frame 的标题栏。
     * @param parent 此标题栏所属的 Frame 对象。
     */
    explicit TitleBar(Frame *parent);
    /**
     * @brief 构造函数，用于 FloatingWindow 的标题栏。
     * @param parent 此标题栏所属的 FloatingWindow 对象。
     */
    explicit TitleBar(FloatingWindow *parent);

    /**
     * @brief 允许 TitleBar 用于非停靠小部件应用程序，例如用于 EGLFS 上的无边框 QMessageBox。
     * 这样可以重用现有的样式。
     * @param parent 通用的父 WidgetType (QWidget 或 QQuickItem)。
     */
    explicit TitleBar(WidgetType *parent);

    /**
     * @brief 析构函数。
     */
    ~TitleBar() override;

    /**
     * @brief 设置标题栏的文本。
     * @param title 要设置的标题字符串。
     */
    void setTitle(const QString &title);
    /**
     * @brief 获取标题栏的文本。
     * @return 当前的标题字符串。
     */
    [[nodiscard]] QString title() const
    {
        return m_title;
    }

    /**
     * @brief 设置标题栏的图标。
     * @param icon 要设置的 QIcon 对象。
     */
    void setIcon(const QIcon &icon);
    /**
     * @brief 实现 Draggable 接口：将此标题栏（通常代表其关联的框架或窗口）包装成一个 WindowBeingDragged 对象。
     * @return 一个包含被拖拽窗口信息的 std::unique_ptr<WindowBeingDragged>。
     */
    std::unique_ptr<WindowBeingDragged> makeWindow() override;
    /**
     * @brief 实现 Draggable 接口：返回此对象是否已经是一个窗口。
     * 对于附加到 Frame 的 TitleBar，这通常返回 false。对于 FloatingWindow 的 TitleBar，其行为可能不同。
     * @return 如果是独立窗口则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isWindow() const override;

    /**
     * @brief 实现 Draggable 接口：如果此标题栏关联的 Frame 或 FloatingWindow 仅包含一个停靠小部件，则返回它。
     * @return 指向单个 DockWidgetBase 的指针，如果不满足条件则返回 nullptr。
     */
    [[nodiscard]] DockWidgetBase *singleDockWidget() const override;

    /**
     * @brief 返回此标题栏是否是浮动窗口的标题栏。
     * @return 如果是浮动窗口的标题栏，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isFloating() const;

    /**
     * @brief 获取此标题栏下管理的停靠小部件列表。
     * 应该总是至少有一个。如果多于一个，则它们是以标签页形式组织的。
     * @return DockWidgetBase 指针的列表 (DockWidgetBase::List)。
     */
    [[nodiscard]] DockWidgetBase::List dockWidgets() const;

    /**
     * @brief 返回此标题栏是否支持浮动/停靠按钮。
     * @return 如果支持，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool supportsFloatingButton() const;

    /**
     * @brief 返回此标题栏是否支持最大化/恢复按钮。
     * @return 如果支持，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool supportsMaximizeButton() const;

    /**
     * @brief 返回此标题栏是否支持最小化按钮。
     * @return 如果支持，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool supportsMinimizeButton() const;

    /**
     * @brief 返回此标题栏是否支持自动隐藏按钮。
     * @return 如果支持，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool supportsAutoHideButton() const;

    /**
     * @brief 返回此标题栏是否有图标。
     * @return 如果有图标，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool hasIcon() const;

    /**
     * @brief 返回此 TitleBar 控制的任何 DockWidget 是否具有子焦点。
     * 不要与 QWidget::hasFocus() 混淆，后者仅指一个窗口小部件。
     * 这更像是 QtQuick 的 FocusScope 的工作方式。
     * @return 如果关联内容有焦点，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isFocused() const;

    /**
     * @brief 获取标题栏的图标。
     * @return QIcon 对象。
     */
    [[nodiscard]] QIcon icon() const;

    /**
     * @brief 切换浮动状态（通常通过双击标题栏触发）。
     * @return 如果操作成功（例如，成功切换状态），则可能返回 true。
     */
    Q_INVOKABLE bool onDoubleClicked();


    /**
     * @brief 返回位于此标题栏下方的标签栏 (TabBar)。
     * 仅当浮动窗口包含多个嵌套 Frame 时，此值才可能为 nullptr。
     * @return 指向 TabBar 对象的指针，如果不存在则为 nullptr。
     */
    [[nodiscard]] TabBar *tabBar() const;

    /**
     * @brief 获取此标题栏所属的 Frame。
     * @return 指向 Frame 对象的指针，如果此标题栏不属于 Frame (例如属于 FloatingWindow)，则为 nullptr。
     */
    [[nodiscard]] Frame *frame() const
    {
        return m_frame;
    }

    /**
     * @brief 获取此标题栏所属的 FloatingWindow。
     * @return 指向 FloatingWindow 对象的指针，如果此标题栏不属于 FloatingWindow，则为 nullptr。
     */
    [[nodiscard]] FloatingWindow *floatingWindow() const
    {
        return m_floatingWindow;
    }

    /**
     * @brief 如果此标题栏属于停靠在主窗口中的停靠小部件，则返回主窗口。
     * 否则返回 nullptr。
     * @return 指向 MainWindowBase 的指针，或者 nullptr。
     */
    [[nodiscard]] MainWindowBase *mainWindow() const;

    /**
     * @brief (重写 Draggable 接口) 返回此标题栏是否属于处于 MDI 模式（在 MDI 区域内）的停靠小部件。
     * 例如，在以 MainWindowOption_MDI 选项创建的主窗口中。
     * @return 如果是 MDI 模式，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isMDI() const override;

    /**
     * @brief 更新标题栏上按钮（例如关闭、浮动等）的启用/禁用和可见性状态。
     */
    void updateButtons();

Q_SIGNALS:
    /**
     * @brief 当标题文本改变时发射此信号。
     */
    void titleChanged();
    /**
     * @brief 当图标改变时发射此信号。
     */
    void iconChanged();
    /**
     * @brief 当标题栏关联内容的焦点状态改变时发射此信号。
     */
    void isFocusedChanged();
    /**
     * @brief 当关闭按钮的启用状态改变时发射此信号。
     * @param enabled 新的启用状态。
     */
    void closeButtonEnabledChanged(bool enabled);
    /**
     * @brief 当浮动/停靠按钮的可见性状态改变时发射此信号。
     * @param visible 新的可见性状态。
     */
    void floatButtonVisibleChanged(bool visible);
    /**
     * @brief 当浮动/停靠按钮的工具提示文本改变时发射此信号。
     * @param toolTip 新的工具提示文本。
     */
    void floatButtonToolTipChanged(const QString &toolTip);

protected:
    /**
     * @brief 当关闭按钮被点击时的槽函数或处理函数。
     */
    Q_INVOKABLE void onCloseClicked();
    /**
     * @brief 当浮动/停靠按钮被点击时的槽函数或处理函数。
     */
    Q_INVOKABLE void onFloatClicked();
    /**
     * @brief 当最大化按钮被点击时的槽函数或处理函数。
     */
    Q_INVOKABLE void onMaximizeClicked();
    /**
     * @brief 当最小化按钮被点击时的槽函数或处理函数。
     */
    Q_INVOKABLE void onMinimizeClicked();
    /**
     * @brief 切换窗口的最大化/正常状态。
     */
    Q_INVOKABLE void toggleMaximized();
    /**
     * @brief 当自动隐藏按钮被点击时的槽函数或处理函数。
     */
    Q_INVOKABLE void onAutoHideClicked();

    /**
     * @brief 获取关闭按钮的启用状态。
     * @return 如果关闭按钮启用，则返回 true。
     */
    [[nodiscard]] bool closeButtonEnabled() const;
    /**
     * @brief 获取浮动/停靠按钮的可见性状态。
     * @return 如果浮动按钮可见，则返回 true。
     */
    [[nodiscard]] bool floatButtonVisible() const;
    /**
     * @brief 获取浮动/停靠按钮的工具提示文本。
     * @return 工具提示字符串。
     */
    [[nodiscard]] QString floatButtonToolTip() const;

    /**
     * @brief (虚函数) 更新最大化按钮的状态（例如，根据窗口是否已最大化）。
     * 子类（如 TitleBarWidget）应实现此方法以更新实际的按钮 UI。
     */
    virtual void updateMaximizeButton()
    {
    }
    /**
     * @brief (虚函数) 更新最小化按钮的状态。
     * 子类应实现此方法。
     */
    virtual void updateMinimizeButton()
    {
    }
    /**
     * @brief (虚函数) 更新自动隐藏按钮的状态。
     * 子类应实现此方法。
     */
    virtual void updateAutoHideButton()
    {
    }

#ifdef DOCKS_DEVELOPER_MODE // 仅在开发者模式下编译
    // 以下函数主要用于单元测试
    /**
     * @brief (纯虚函数，仅开发者模式) 返回关闭按钮是否可见。
     */
    virtual bool isCloseButtonVisible() const = 0;
    /**
     * @brief (纯虚函数，仅开发者模式) 返回关闭按钮是否启用。
     */
    virtual bool isCloseButtonEnabled() const = 0;
    /**
     * @brief (纯虚函数，仅开发者模式) 返回浮动按钮是否可见。
     */
    virtual bool isFloatButtonVisible() const = 0;
    /**
     * @brief (纯虚函数，仅开发者模式) 返回浮动按钮是否启用。
     */
    virtual bool isFloatButtonEnabled() const = 0;
#endif

    /**
     * @brief 处理焦点进入事件。
     * @param event 焦点事件对象。
     */
    void focusInEvent(QFocusEvent *event) override;
    /**
     * @brief 返回此标题栏是否属于一个覆盖层显示的框架。
     * @return 如果是覆盖层，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isOverlayed() const;

private:
    // 声明友元类，允许它们访问此类的私有成员
    friend class ::TestDocks; // 测试类
    friend class FloatingWindowWidget; // Qt Widgets 版本的 FloatingWindow 实现
    friend class TabWidgetWidget; // Qt Widgets 版本的 TabWidget 实现

    /**
     * @brief 更新浮动/停靠按钮的状态和工具提示。
     */
    void updateFloatButton();
    /**
     * @brief 更新关闭按钮的启用状态和可见性。
     */
    void updateCloseButton();
    /**
     * @brief 设置关闭按钮的启用状态。
     * @param enabled true 表示启用，false 表示禁用。
     */
    void setCloseButtonEnabled(bool enabled);
    /**
     * @brief 设置关闭按钮的可见性。
     * @param visible true 表示可见，false 表示隐藏。
     */
    void setCloseButtonVisible(bool visible);
    /**
     * @brief 设置浮动/停靠按钮的可见性。
     * @param visible true 表示可见，false 表示隐藏。
     */
    void setFloatButtonVisible(bool visible);
    /**
     * @brief 设置浮动/停靠按钮的工具提示文本。
     * @param toolTip 工具提示字符串。
     */
    void setFloatButtonToolTip(const QString &toolTip);

    /**
     * @brief 初始化标题栏的内部组件和连接。
     */
    void init();

    QPoint m_pressPos; ///< 鼠标按下的初始位置（用于拖拽判断）。
    QString m_title; ///< 标题栏显示的文本。
    QIcon m_icon; ///< 标题栏显示的图标。

    Frame *const m_frame; ///< 指向此标题栏所属的 Frame 的常量指针 (如果适用)。
    FloatingWindow *const m_floatingWindow; ///< 指向此标题栏所属的 FloatingWindow 的常量指针 (如果适用)。
    WidgetType *const m_genericWidget; ///< 指向通用父 WidgetType 的常量指针 (用于非 Frame/FloatingWindow 的情况)。
    const bool m_supportsAutoHide; ///< 标记此标题栏是否支持自动隐藏按钮 (基于其父对象类型)。
    bool m_closeButtonEnabled = true; ///< 关闭按钮的启用状态。
    bool m_floatButtonVisible = true; ///< 浮动/停靠按钮的可见状态。
    QString m_floatButtonToolTip; ///< 浮动/停靠按钮的工具提示文本。
};


} // namespace KDDockWidgets

#endif // KD_TITLEBAR_P_H
