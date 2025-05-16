#ifndef KD_FLOATING_WINDOW_P_H
#define KD_FLOATING_WINDOW_P_H

#include "kddockwidgets/docks_export.h"      // 导入导出宏定义
#include "kddockwidgets/QWidgetAdapter.h"  // QWidget 和 QQuickItem 的适配器类
#include "kddockwidgets/LayoutSaver.h"     // 布局保存与恢复相关的类
#include "kddockwidgets/Qt5Qt6Compat_p.h"  // Qt5/Qt6 兼容性私有头文件
#include "Frame_p.h"                       // Frame 私有头文件
#include "Draggable_p.h"                   // Draggable 私有头文件 (可拖拽对象接口)
#include "DropArea_p.h"                    // DropArea 私有头文件 (放置区域)

QT_BEGIN_NAMESPACE
// 前向声明 Qt 类
class QAbstractNativeEventFilter; // Qt 抽象原生事件过滤器
class QWindowStateChangeEvent;    // Qt 窗口状态改变事件
QT_END_NAMESPACE

namespace KDDockWidgets {

// 前向声明 KDDockWidgets 内部类
class MainWindowBase; // 主窗口基类
class DropArea;       // 放置区域类
class Frame;          // 框架类 (容纳 DockWidgetBase)
class MultiSplitter;  // 多重分割器类
class LayoutWidget;   // 布局小部件类

/**
 * @brief 代表一个浮动窗口。
 *
 * 浮动窗口可以包含一个或多个停靠小部件 (DockWidgetBase)，并且可以在主窗口外部自由移动。
 * 它本身也是一个可拖拽 (Draggable) 的对象。
 */
class DOCKS_EXPORT FloatingWindow
    : public QWidgetAdapter, // 继承自 QWidgetAdapter 以支持 QWidget 和 QQuickItem
      public Draggable      // 继承自 Draggable 以支持被拖拽
{
    Q_OBJECT
    /// @brief Q_PROPERTY 宏，暴露 titleBar 属性给 Qt 元对象系统，表示此浮动窗口的标题栏。
    Q_PROPERTY(KDDockWidgets::TitleBar *titleBar READ titleBar CONSTANT)
    /// @brief Q_PROPERTY 宏，暴露 dropArea 属性给 Qt 元对象系统，表示此浮动窗口内部的放置区域。
    Q_PROPERTY(KDDockWidgets::DropArea *dropArea READ dropArea CONSTANT)
public:
    /**
     * @brief 构造函数，根据建议的几何形状和父主窗口创建浮动窗口。
     * @param suggestedGeometry 建议的初始几何位置和大小。
     * @param parent 父 MainWindowBase 对象，默认为 nullptr。
     * @param requestedFlags 请求的浮动窗口标志，默认为从全局配置获取。
     */
    explicit FloatingWindow(QRect suggestedGeometry, MainWindowBase *parent = nullptr,
                            FloatingWindowFlags requestedFlags = FloatingWindowFlag::FromGlobalConfig);
    /**
     * @brief 构造函数，根据已有的 Frame 创建浮动窗口。
     * @param frame 要包含在此浮动窗口中的 Frame 对象。
     * @param suggestedGeometry 建议的初始几何位置和大小。
     * @param parent 父 MainWindowBase 对象，默认为 nullptr。
     */
    explicit FloatingWindow(Frame *frame, QRect suggestedGeometry, MainWindowBase *parent = nullptr);
    /**
     * @brief 析构函数。
     */
    ~FloatingWindow() override;

    /**
     * @brief 从保存的布局数据中反序列化浮动窗口的状态。
     * @param data 包含浮动窗口布局信息的 LayoutSaver::FloatingWindow 对象。
     * @return 如果反序列化成功则返回 true，否则返回 false。
     */
    bool deserialize(const LayoutSaver::FloatingWindow &data);
    /**
     * @brief 将当前浮动窗口的状态序列化为布局数据。
     * @return 包含浮动窗口布局信息的 LayoutSaver::FloatingWindow 对象。
     */
    [[nodiscard]] LayoutSaver::FloatingWindow serialize() const;

    // Draggable 接口实现:
    /**
     * @brief 实现 Draggable 接口：将此浮动窗口包装成一个 WindowBeingDragged 对象。
     * @return 一个包含此浮动窗口信息的 std::unique_ptr<WindowBeingDragged>。
     */
    std::unique_ptr<WindowBeingDragged> makeWindow() override;
    /**
     * @brief 实现 Draggable 接口：如果此浮动窗口仅包含一个停靠小部件，则返回它。
     * @return 指向单个 DockWidgetBase 的指针，如果不满足条件则返回 nullptr。
     */
    [[nodiscard]] DockWidgetBase *singleDockWidget() const override;
    /**
     * @brief 实现 Draggable 接口：返回此对象是否已经是一个窗口 (对于 FloatingWindow 总是 true)。
     * @return 总是返回 true。
     */
    [[nodiscard]] bool isWindow() const override;

    /**
     * @brief 获取此浮动窗口中包含的所有停靠小部件。
     * @return DockWidgetBase 指针的 QVector。
     */
    [[nodiscard]] QVector<DockWidgetBase *> dockWidgets() const;
    /**
     * @brief 获取此浮动窗口中包含的所有框架 (Frame)。
     * @return Frame 指针的列表 (Frame::List)。
     */
    [[nodiscard]] Frame::List frames() const;
    /**
     * @brief 获取此浮动窗口内部的放置区域 (DropArea)。
     * @return 指向 DropArea 对象的指针。
     */
    [[nodiscard]] DropArea *dropArea() const
    {
        return m_dropArea;
    }

    /**
     * @brief 获取与此浮动窗口关联的用户定义类型。
     * @return 用户定义的整数类型。
     */
    [[nodiscard]] int userType() const;

    /**
     * @brief 返回此窗口是否为一个工具窗口 (Utility Window)。
     * 工具窗口通常不会出现在任务栏中。
     * @return 如果是工具窗口，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isUtilityWindow() const;

    /**
     * @brief 确保给定的矩形区域在屏幕可见范围内。
     * @param geometry 要调整的矩形区域（引用传递）。
     */
    static void ensureRectIsOnScreen(QRect &geometry);

#ifdef Q_OS_WIN
    /**
     * @brief (仅 Windows) 设置最后的命中测试结果。
     * 用于处理 Windows 特定的非客户区消息。
     * @param hitTest 命中测试的结果代码 (例如 HTCAPTION)。
     */
    void setLastHitTest(int hitTest)
    {
        m_lastHitTest = hitTest;
    }
#endif
    /**
     * @brief 返回此浮动窗口的标题栏。
     *
     * 如果使用的是原生操作系统标题栏，则此 KDDockWidgets 内部的 TitleBar 是隐藏的。
     * @return 指向 TitleBar 对象的指针。
     */
    [[nodiscard]] TitleBar *titleBar() const
    {
        return m_titleBar;
    }

    /**
     * @brief 类似于 setGeometry()，但值可能会被调整。
     *
     * 例如，如果建议的矩形大于最大尺寸，我们会将其缩小。
     * @param suggestedRect 建议的几何位置和大小。
     * @param hints 调整几何形状时的提示，默认为 SuggestedGeometryHint_None。
     */
    void setSuggestedGeometry(QRect suggestedRect, SuggestedGeometryHints hints = SuggestedGeometryHint_None);

    /**
     * @brief 检查此浮动窗口中是否包含任何不可关闭的停靠小部件。
     * @return 如果至少有一个不可关闭的停靠小部件，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool anyNonClosable() const;
    /**
     * @brief 检查此浮动窗口中是否包含任何不可停靠的停靠小部件。
     * @return 如果至少有一个不可停靠的停靠小部件，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool anyNonDockable() const;

    /**
     * @brief 检查此浮动窗口是否只有一个框架 (Frame)。
     * 如果为 true，意味着这里没有并排停靠的小部件，只有一个框架。
     * 注意，即使只有一个框架，它仍然可以包含多个 DockWidget，因为它们可以以标签页的形式组织在单个框架中。
     * @return 如果此浮动窗口只有一个框架，则返回 true。
     */
    [[nodiscard]] bool hasSingleFrame() const;

    /**
     * @brief 检查此浮动窗口是否只有一个停靠小部件。
     * 这是比 hasSingleFrame() 更具体的情况，它不仅意味着单个框架，
     * 而且该框架必须只包含一个停靠小部件。
     * @return 如果此浮动窗口只有一个停靠小部件，则返回 true。
     */
    [[nodiscard]] bool hasSingleDockWidget() const;

    /**
     * @brief 如果此浮动窗口只有一个框架 (Frame)，则返回该框架；否则返回 nullptr。
     * @return 指向单个 Frame 的指针，或者 nullptr。
     */
    [[nodiscard]] Frame *singleFrame() const;

    /**
     * @brief 返回是否已经为此窗口发出了 deleteLater() 调用。
     * @return 如果已计划删除，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool beingDeleted() const;

    /**
     * @brief 类似于 deleteLater()，但同时会将 beingDeleted() 标记设置为 true。
     */
    void scheduleDeleteLater();

    /**
     * @brief 返回此浮动窗口内部的 MultiSplitter。
     * @return 指向 MultiSplitter 对象的指针。
     */
    [[nodiscard]] MultiSplitter *multiSplitter() const;

    /**
     * @brief 返回此浮动窗口内部的 LayoutWidget。
     * @return 指向 LayoutWidget 对象的指针。
     */
    [[nodiscard]] LayoutWidget *layoutWidget() const;

    /**
     * @brief 检查给定的全局点是否位于标题栏内（或者，在没有标题栏时，位于标签栏的可拖拽空白区域内）。
     * @param globalPoint 要检查的全局点坐标。
     * @return 如果点在可拖拽区域内，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isInDragArea(QPoint globalPoint) const;

    /**
     * @brief 实现 Draggable 接口：返回此对象是否为 MDI 窗口 (对于 FloatingWindow 通常为 false，除非有特殊 MDI 行为)。
     * @return 如果是 MDI 窗口，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isMDI() const override;

    /**
     * @brief 更新窗口的标题和图标。
     *
     * 通常基于其包含的停靠小部件的标题和图标。
     */
    void updateTitleAndIcon();
    /**
     * @brief 更新自定义标题栏的可见性。
     *
     * 例如，根据配置或停靠小部件的数量来显示或隐藏标题栏。
     */
    void updateTitleBarVisibility();

    /**
     * @brief 获取此浮动窗口的亲和性 (affinities) 列表。
     * @return 亲和性名称的字符串列表。
     */
    [[nodiscard]] QStringList affinities() const;

    /**
     * @brief 返回可拖拽区域的矩形（全局坐标）。
     *
     * 通常是标题栏的矩形。然而，当使用 Config::Flag_HideTitleBarWhenTabsVisible 时，它将是标签栏的背景区域。
     * 返回的是全局坐标。
     * @return 可拖拽区域的 QRect。
     */
    [[nodiscard]] QRect dragRect() const;

    /**
     * @brief 返回是否所有停靠小部件都设置了指定的选项。
     * @param option 要检查的 DockWidgetBase::Option。
     * @return 如果所有停靠小部件都有该选项，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool allDockWidgetsHave(DockWidgetBase::Option option) const;

    /**
     * @brief 返回是否至少有一个停靠小部件设置了指定的选项。
     * @param option 要检查的 DockWidgetBase::Option。
     * @return 如果至少有一个停靠小部件有该选项，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool anyDockWidgetsHas(DockWidgetBase::Option option) const;

    /**
     * @brief 返回是否所有停靠小部件都设置了指定的布局保存器选项。
     * @param option 要检查的 DockWidgetBase::LayoutSaverOption。
     * @return 如果所有停靠小部件都有该布局保存器选项，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool allDockWidgetsHave(DockWidgetBase::LayoutSaverOption option) const;

    /**
     * @brief 返回是否至少有一个停靠小部件设置了指定的布局保存器选项。
     * @param option 要检查的 DockWidgetBase::LayoutSaverOption。
     * @return 如果至少有一个停靠小部件有该布局保存器选项，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool anyDockWidgetsHas(DockWidgetBase::LayoutSaverOption option) const;

    /**
     * @brief 将指定的停靠小部件添加到浮动窗口的指定位置。
     * @param dockWidget 要添加的停靠小部件。
     * @param location 停靠的位置。
     * @param relativeTo 相对于哪个停靠小部件进行停靠（如果适用）。
     * @param option 初始化选项，默认为空。
     */
    void addDockWidget(DockWidgetBase *dockWidget, KDDockWidgets::Location location,
                       DockWidgetBase *relativeTo, InitialOption option = {});

    /**
     * @brief 返回作为此浮动窗口瞬时父窗口的主窗口。
     *
     * 如果在主窗口之前创建停靠小部件，则可能为 nullptr。如果存在多个主窗口，也可能返回任意值。
     * @return 指向 MainWindowBase 的指针。
     */
    [[nodiscard]] MainWindowBase *mainWindow() const;

    /**
     * @brief 获取浮动窗口的内容边距。
     * @return QMargins 对象，表示内容区域与窗口边框的距离。
     */
    [[nodiscard]] static QMargins contentMargins();

    /**
     * @brief 允许用户重写 QWindow::isMaximized() 的行为。
     *
     * 用于解决某些窗口管理器不支持最大化/最小化 Qt::Tool 类型窗口的问题。
     * 默认情况下，此函数仅调用 QWindow::isMaximized()。
     * @sa QTBUG-95478 (相关的 Qt Bug 报告)
     * @return 如果窗口被认为是最大化的，则返回 true。
     */
    [[nodiscard]] virtual bool isMaximizedOverride() const;

    /**
     * @brief 允许用户重写 QWindow::isMinimized() 的行为。
     *
     * 用于解决某些窗口管理器不支持最大化/最小化 Qt::Tool 类型窗口的问题。
     * 默认情况下，此函数仅调用 QWindow::isMinimized()。
     * @sa QTBUG-95478
     * @return 如果窗口被认为是最小化的，则返回 true。
     */
    [[nodiscard]] virtual bool isMinimizedOverride() const;

    /**
     * @brief 默认情况下等同于 QWindow::showMaximized()。
     *
     * 但允许用户重写它以解决特定窗口管理器的 BUG。
     * @sa QTBUG-95478
     */
    virtual void showMaximized();

    /**
     * @brief 默认情况下等同于 QWindow::showNormal()。
     *
     * 但允许用户重写它以解决特定窗口管理器的 BUG。
     * @sa QTBUG-95478
     */
    virtual void showNormal();

    /**
     * @brief 默认情况下等同于 QWindow::showMinimized()。
     *
     * 但允许用户重写它以解决特定窗口管理器的 BUG。
     * @sa QTBUG-95478
     */
    virtual void showMinimized();

    /**
     * @brief 默认情况下等同于 QWidget::normalGeometry()。
     *
     * 派生类可以在此处实现不同的行为，以解决 Qt::Tool 类型窗口在某些窗口管理器下的问题。
     * 对于 QtQuick 也很有用，最终可以在保存/恢复最大化窗口时保留正常几何形状。
     * 因为 QWindow 没有正常几何形状的概念，所以我们需要在这里实现它。
     * @sa QTBUG-95478
     * @return 窗口在最大化/最小化之前的正常几何形状。
     */
    [[nodiscard]] virtual QRect normalGeometry() const;

    /**
     * @brief 获取窗口管理器报告的最后窗口状态。
     *
     * QWidget 报告的状态由于异步性质可能与窗口管理器的状态不同。
     * 此方法返回窗口管理器本身报告的最后状态。
     * @return Qt::WindowState 枚举值。
     */
    [[nodiscard]] Qt::WindowState lastWindowManagerState() const;

    /// @brief 允许用户应用程序指定要使用的窗口标志，而不是 KDDW 的默认标志。
    /// 由此引起的 BUG 将不受支持，因为可能出错的组合数量可能是无限的。
    static Qt::WindowFlags s_windowFlagsOverride;

    /**
     * @brief 返回此浮动窗口是否支持显示最小化按钮。
     * @return 如果支持，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool supportsMinimizeButton() const;

    /**
     * @brief 返回此浮动窗口是否支持显示最大化按钮。
     * @return 如果支持，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool supportsMaximizeButton() const;

Q_SIGNALS:
    /**
     * @brief 当窗口的激活状态改变时发射此信号。
     */
    void activatedChanged();
    /**
     * @brief 当窗口内的框架 (Frame) 数量改变时发射此信号。
     */
    void numFramesChanged();
    /**
     * @brief 当窗口状态 (例如最小化、最大化、正常) 改变时发射此信号。
     */
    void windowStateChanged();

protected:
    /**
     * @brief 如果需要，创建大小调整处理器 (WidgetResizeHandler)。
     */
    void maybeCreateResizeHandler();

#if defined(Q_OS_WIN) && defined(KDDOCKWIDGETS_QTWIDGETS)
    /**
     * @brief (仅 Windows QWidget 版本) 处理原生窗口事件。
     * @param eventType 事件类型。
     * @param message 事件消息。
     * @param result 事件处理结果。
     * @return 如果事件被处理，则返回 true。
     */
    bool nativeEvent(const QByteArray &eventType, void *message, Qt5Qt6Compat::qintptr *result) override;
#endif

    /**
     * @brief Qt 事件处理函数。
     * @param ev 事件对象。
     * @return 如果事件被处理，则返回 true。
     */
    bool event(QEvent *ev) override;
    /**
     * @brief 处理关闭事件。
     * @param event 关闭事件对象。
     */
    void onCloseEvent(QCloseEvent *event) override;

    const FloatingWindowFlags m_flags;     ///< 此浮动窗口的标志。
    QPointer<DropArea> m_dropArea;         ///< 指向内部 DropArea 的 QPointer，用于自动跟踪其生命周期。
    TitleBar *const m_titleBar;            ///< 指向内部标题栏的常量指针。
    Qt::WindowState m_lastWindowManagerState = Qt::WindowNoState; ///< 窗口管理器报告的最后窗口状态。

private:
    Q_DISABLE_COPY(FloatingWindow) ///< 禁止拷贝构造函数和拷贝赋值操作符。
    /**
     * @brief 获取最大尺寸提示。
     * @return QSize 对象，表示窗口的最大尺寸限制。
     */
    [[nodiscard]] QSize maxSizeHint() const;
    /**
     * @brief 更新窗口的大小约束。
     */
    void updateSizeConstraints();
    /**
     * @brief 当框架数量发生变化时的槽函数。
     * @param count 新的框架数量。
     */
    void onFrameCountChanged(int count);
    /**
     * @brief 当可见框架数量发生变化时的槽函数。
     * @param count 新的可见框架数量。
     */
    void onVisibleFrameCountChanged(int count);

    bool m_disableSetVisible = false;      ///< 标记是否禁用 setVisible 调用，用于内部逻辑控制。
    bool m_deleteScheduled = false;        ///< 标记是否已计划删除此窗口。
    bool m_inDtor = false;                 ///< 标记是否正在析构过程中。
    bool m_updatingTitleBarVisibility = false; ///< 标记是否正在更新标题栏可见性，防止重入。
    QMetaObject::Connection m_layoutDestroyedConnection; ///< 用于连接布局销毁信号的连接对象。
    QAbstractNativeEventFilter *m_nchittestFilter = nullptr; ///< (仅 Windows) 用于非客户区命中测试的原生事件过滤器。
    /**
     * @brief 获取重写后的窗口状态。
     * @return Qt::WindowState 枚举值。
     */
    [[nodiscard]] Qt::WindowState windowStateOverride() const;
#ifdef Q_OS_WIN
    int m_lastHitTest = 0; ///< (仅 Windows) 存储最后的命中测试结果。
#endif
};

} // namespace KDDockWidgets

#endif // KD_FLOATING_WINDOW_P_H