#ifndef KD_FRAME_P_H
#define KD_FRAME_P_H

#include "kddockwidgets/docks_export.h" // 导入导出宏定义
#include "kddockwidgets/QWidgetAdapter.h" // QWidget 和 QQuickItem 的适配器类
#include "kddockwidgets/FocusScope.h" // 焦点范围管理类
#include "kddockwidgets/DockWidgetBase.h" // 停靠小部件基类
#include "kddockwidgets/LayoutSaver.h" // 布局保存与恢复相关的类
#include "multisplitter/Widget.h" // MultiSplitter 中的小部件基类 (LayoutGuestWidget)

#include <QVector> // Qt 动态数组容器
#include <QDebug> // Qt 调试输出类
#include <QPointer> // Qt QObject 指针的弱引用，用于安全地跟踪对象生命周期

// 前向声明，用于测试目的
class TestDocks;

namespace KDDockWidgets {

// 前向声明 KDDockWidgets 内部类
class TitleBar; // 标题栏类
class TabWidget; // 标签页小部件类
class DockWidgetBase; // 停靠小部件基类
class FloatingWindow; // 浮动窗口类
class MainWindowBase; // 主窗口基类
class MDILayoutWidget; // MDI (多文档界面) 布局小部件类
class WidgetResizeHandler; // 小部件大小调整处理器类

/**
 * @brief 一个包装 DockWidget 的小部件，为其添加了 QTabWidget 和 TitleBar。
 *
 * Frame 是实际放入 MultiSplitter 中的小部件。它提供了一个 TitleBar（标题栏），
 * 你可以用它来分离（detach）框架，并且还提供了一个 QTabWidget（标签页小部件），
 * 这样你就可以将多个停靠小部件以标签页的形式组织在一起。
 *
 * 这个类本身不添加窗口边框，并且它从来不是一个顶层小部件。
 * 一个 Frame 总是位于一个 LayoutWidget 内部，这个 LayoutWidget 可以属于主窗口的 MultiSplitter，
 * 也可以属于浮动窗口的 MultiSplitter。
 */
class DOCKS_EXPORT Frame
    : public LayoutGuestWidget, // 继承自 LayoutGuestWidget，表示可以被 MultiSplitter 管理
      public FocusScope // 继承自 FocusScope，用于管理内部焦点
{
    Q_OBJECT
    /// @brief Q_PROPERTY 宏，暴露 titleBar 属性给 Qt 元对象系统，表示此框架的标题栏。
    Q_PROPERTY(KDDockWidgets::TitleBar *titleBar READ titleBar CONSTANT)
    /// @brief Q_PROPERTY 宏，暴露 actualTitleBar 属性给 Qt 元对象系统，表示实际显示的标题栏（可能因 MDI 等情况而不同）。
    Q_PROPERTY(KDDockWidgets::TitleBar *actualTitleBar READ actualTitleBar NOTIFY actualTitleBarChanged)
    /// @brief Q_PROPERTY 宏，暴露 currentIndex 属性给 Qt 元对象系统，表示当前激活的标签页索引。
    Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentDockWidgetChanged)
    /// @brief Q_PROPERTY 宏，暴露 userType 属性给 Qt 元对象系统，表示用户定义的类型。
    Q_PROPERTY(int userType READ userType CONSTANT)
    /// @brief Q_PROPERTY 宏，暴露 isMDI 属性给 Qt 元对象系统，表示此框架是否处于 MDI 模式。
    Q_PROPERTY(bool isMDI READ isMDI NOTIFY isMDIChanged)
public:
    /// @brief Frame 指针的 QList 类型定义，方便使用。
    typedef QList<Frame *> List;

    /**
     * @brief 构造函数。
     * @param parent 父 QWidgetOrQuick 对象，默认为 nullptr。
     * @param options 框架选项，默认为 FrameOption_None。
     * @param userType 用户定义的类型，默认为 0。
     */
    explicit Frame(QWidgetOrQuick *parent = nullptr, FrameOptions options = FrameOption_None,
                   int userType = 0);
    /**
     * @brief 析构函数。
     */
    ~Frame() override;

    /**
     * @brief 从保存的布局数据中反序列化框架的状态。
     * @param data 包含框架布局信息的 LayoutSaver::Frame 对象。
     * @return 指向新创建并恢复状态的 Frame 对象的指针。
     */
    static Frame *deserialize(const LayoutSaver::Frame &data);
    /**
     * @brief 将当前框架的状态序列化为布局数据。
     * @return 包含框架布局信息的 LayoutSaver::Frame 对象。
     */
    [[nodiscard]] LayoutSaver::Frame serialize() const;

    /**
     * @brief 将一个小部件添加到框架的 TabWidget 中。
     * @param dockWidget 要添加的 DockWidgetBase 对象。
     * @param option 初始化选项，默认为空。
     */
    void addWidget(DockWidgetBase *dockWidget, InitialOption option = {});
    /**
     * @brief (重载) 将另一个 Frame 合并到此 Frame 的 TabWidget 中。
     * @param frame 要添加的 Frame 对象。
     * @param option 初始化选项，默认为空。
     */
    void addWidget(Frame *frame, InitialOption option = {});
    /**
     * @brief (重载) 将一个浮动窗口的内容（通常是其内部的 Frame）合并到此 Frame 的 TabWidget 中。
     * @param floatingWindow 要添加的 FloatingWindow 对象。
     * @param option 初始化选项，默认为空。
     */
    void addWidget(FloatingWindow *floatingWindow, InitialOption option = {});

    /**
     * @brief 将一个小部件插入到框架 TabWidget 的指定索引处。
     * @param dockWidget 要插入的 DockWidgetBase 对象。
     * @param index 要插入的目标索引。
     * @param option 初始化选项，默认为空。
     */
    void insertWidget(DockWidgetBase *dockWidget, int index, InitialOption option = {});

    /**
     * @brief 从框架中移除一个停靠小部件。
     * @param dockWidget 要移除的 DockWidgetBase 对象。
     */
    void removeWidget(DockWidgetBase *dockWidget);

    /**
     * @brief 分离此框架中的指定标签页（停靠小部件），使其成为一个新的浮动窗口。
     * @param dockWidget 要分离的 DockWidgetBase 对象。
     * @return 指向新创建的 FloatingWindow 的指针。
     */
    FloatingWindow *detachTab(DockWidgetBase *dockWidget);

    /**
     * @brief 返回指定停靠小部件在此框架标签页中的索引。
     * @param dockWidget 要查找索引的 DockWidgetBase 对象。
     * @return 如果找到，则返回索引；否则返回 -1。
     */
    int indexOfDockWidget(const DockWidgetBase *dockWidget);

    /**
     * @brief 返回当前激活的标签页的索引。
     * @return 当前标签页的索引。
     */
    [[nodiscard]] int currentIndex() const;

    /**
     * @brief 设置当前激活的标签页索引。
     * @param index 要设置的索引。
     */
    void setCurrentTabIndex(int index);

    /**
     * @brief 将指定的停靠小部件设置为当前激活的标签页。
     * @param dockWidget 要设置为当前标签页的 DockWidgetBase 对象。
     */
    void setCurrentDockWidget(DockWidgetBase *dockWidget);

    /**
     * @brief (已废弃) 请使用 insertWidget。将一个停靠小部件插入到指定的索引。
     * @param dockWidget 要插入的 DockWidgetBase 对象。
     * @param index 要插入的目标索引。
     */
    void insertDockWidget(DockWidgetBase *dockWidget, int index);

    /**
     * @brief 返回位于指定索引处的停靠小部件。
     * @param index 索引。
     * @return 指向 DockWidgetBase 对象的指针，如果索引无效则返回 nullptr。
     */
    [[nodiscard]] DockWidgetBase *dockWidgetAt(int index) const;

    /**
     * @brief 返回当前激活的停靠小部件。
     * @return 指向当前 DockWidgetBase 对象的指针。
     */
    [[nodiscard]] DockWidgetBase *currentDockWidget() const;

    /**
     * @brief 返回框架内包含的停靠小部件的数量。
     * @return 停靠小部件的数量。
     */
    [[nodiscard]] int dockWidgetCount() const;

    /**
     * @brief 返回此框架内部的标签页小部件 (TabWidget)。
     * @return 指向 TabWidget 对象的指针。
     */
    [[nodiscard]] TabWidget *tabWidget() const;

    /**
     * @brief 更新框架的标题和图标，通常基于当前激活的停靠小部件。
     */
    void updateTitleAndIcon();
    /**
     * @brief 当某个子停靠小部件的标题改变时调用此方法以更新框架自身。
     */
    void onDockWidgetTitleChanged();
    /**
     * @brief 更新标题栏的可见性。
     */
    void updateTitleBarVisibility();
    /**
     * @brief 更新所有子停靠小部件的浮动动作 (QAction) 状态。
     */
    void updateFloatingActions() const;
    /**
     * @brief 检查指定的全局鼠标位置是否在此框架内。
     * @param globalPos 全局鼠标位置。
     * @return 如果在此框架内，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool containsMouse(QPoint globalPos) const;
    /**
     * @brief 获取此框架的标题栏。
     * @return 指向 TitleBar 对象的指针。
     */
    [[nodiscard]] TitleBar *titleBar() const;
    /**
     * @brief 获取实际显示的标题栏。在 MDI 模式下，这可能是 MDI 包装器的标题栏。
     * @return 指向实际 TitleBar 对象的指针。
     */
    [[nodiscard]] TitleBar *actualTitleBar() const;
    /**
     * @brief 获取框架的标题。
     * @return 框架的标题字符串。
     */
    [[nodiscard]] QString title() const;
    /**
     * @brief 获取框架的图标。
     * @return 框架的 QIcon 对象。
     */
    [[nodiscard]] QIcon icon() const;
    /**
     * @brief 获取此框架中包含的所有停靠小部件。
     * @return DockWidgetBase 指针的 QVector。
     */
    [[nodiscard]] QVector<DockWidgetBase *> dockWidgets() const;

    /**
     * @brief 检查此框架是否是其所在布局中的唯一框架。
     * @return 如果是唯一框架，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isTheOnlyFrame() const;

    /**
     * @brief 返回此框架是否作为自动隐藏侧边栏覆盖在主窗口之上。
     * @return 如果是覆盖层，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isOverlayed() const;

    /**
     * @brief 清除 FrameOption_IsOverlayed 标志。
     * 例如，如果拖动一个侧边栏覆盖层，它就会变成一个普通的浮动窗口。
     */
    void unoverlay();

    /**
     * @brief 返回此框架是否处于浮动状态。
     *
     * 浮动框架不依附于任何主窗口。如果它依附于一个 FloatingWindow，
     * 并且它是该窗口中唯一的框架，则认为它是浮动的。
     * 一个浮动的框架可以包含多个标签页形式的停靠小部件，此时每个 DockWidget::isFloating()
     * 会返回 false，但 isInFloatingWindow() 仍然会返回 true。
     * @return 如果框架是浮动的，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isFloating() const;

    /**
     * @brief 返回此框架是否位于一个 FloatingWindow 中（而不是 MainWindow）。
     *
     * 初始化后，等同于 !isInMainWindow()。
     * @return 如果在浮动窗口中，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isInFloatingWindow() const;

    /**
     * @brief 返回此框架是否停靠在主窗口 (MainWindow) 内。
     * @return 如果停靠在主窗口内，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isInMainWindow() const;

    /**
     * @brief 返回此小部件是否是中央框架。
     *
     * MainWindow 支持一种模式，即即使没有停靠小部件，中间的框架也保持持久。
     * @return 如果此小部件是主窗口中的中央框架，则返回 true。
     */
    [[nodiscard]] bool isCentralFrame() const
    {
        return m_options & FrameOption_IsCentralFrame;
    }

    /**
     * @brief 返回是否可以将停靠小部件拖放到此框架并以标签页形式停靠。
     * @return 如果可停靠，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isDockable() const
    {
        return !(m_options & FrameOption_NonDockable);
    }

    /**
     * @brief 返回标签页小部件是否总是显示标签，即使只有一个停靠小部件。
     *
     * 虽然从技术上讲，一个非浮动的停靠小部件总是以标签页形式存在，但在大多数情况下，
     * 由于只有一个小部件被标签化，用户不会看到标签。但对于主窗口的中央框架，
     * 通常希望即使只有一个小部件（每个小部件代表一个“文档”）也能看到标签。
     * @return 如果标签页小部件总是显示标签，则返回 true。
     */
    [[nodiscard]] bool alwaysShowsTabs() const
    {
        return m_options & FrameOption_AlwaysShowsTabs;
    }

    /**
     * @brief 返回停靠小部件 @p w 是否在此框架内。
     * @param dockWidget 要检查的 DockWidgetBase 对象。
     * @return 如果包含，则返回 true；否则返回 false。
     */
    bool containsDockWidget(DockWidgetBase *dockWidget) const;

    /**
     * @brief 返回此框架所在的浮动窗口（如果有的话）。
     * @return 指向 FloatingWindow 的指针，如果不在浮动窗口中则返回 nullptr。
     */
    [[nodiscard]] FloatingWindow *floatingWindow() const;

    /**
     * @brief 返回此框架所在的主窗口。
     * 如果不在主窗口内，则返回 nullptr。
     * @return 指向 MainWindowBase 的指针，或者 nullptr。
     */
    [[nodiscard]] MainWindowBase *mainWindow() const;

    /**
     * @brief 将框架放回其先前的主窗口位置。
     *
     * 通常使用 DockWidget::Private::restoreToPreviousPosition()，
     * 但是当我们有一个带标签页的浮动框架时，我们会重用该框架，而不是逐个移动标签化的停靠小部件。
     */
    void restoreToPreviousPosition();

    /**
     * @brief 处理关闭事件。
     * @param e 关闭事件对象。
     */
    void onCloseEvent(QCloseEvent *e) override;
    /**
     * @brief 获取当前标签页的索引。
     * @return 当前标签页的索引。
     */
    [[nodiscard]] int currentTabIndex() const;

    /**
     * @brief 获取框架的选项。
     * @return FrameOptions 枚举值。
     */
    [[nodiscard]] FrameOptions options() const
    {
        return m_options;
    }
    /**
     * @brief 检查此框架中是否包含任何不可关闭的停靠小部件。
     * @return 如果至少有一个不可关闭的停靠小部件，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool anyNonClosable() const;
    /**
     * @brief 检查此框架中是否包含任何不可停靠的停靠小部件。
     * @return 如果至少有一个不可停靠的停靠小部件，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool anyNonDockable() const;

    /**
     * @brief 返回框架内是否没有停靠小部件。如果框架不是持久的，则会自动删除自身。
     * @return 如果为空，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isEmpty() const
    {
        return dockWidgetCount() == 0;
    }

    /**
     * @brief 返回框架内是否只有一个停靠小部件。
     * @return 如果只有一个停靠小部件，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool hasSingleDockWidget() const
    {
        return dockWidgetCount() == 1;
    }

    /**
     * @brief 当子停靠小部件 @p w 显示时调用。
     * @param w 显示的 DockWidgetBase 对象。
     */
    void onDockWidgetShown(DockWidgetBase *w);

    /**
     * @brief 当子停靠小部件 @p w 隐藏时调用。
     * @param w 隐藏的 DockWidgetBase 对象。
     */
    void onDockWidgetHidden(DockWidgetBase *w);

    /**
     * @brief 返回包含此 Frame 的布局项，或者是一个占位符。
     * @return 指向 Layouting::Item 的指针。
     */
    [[nodiscard]] Layouting::Item *layoutItem() const;

    /**
     * @brief (仅供测试使用) 返回整个应用程序中 Frame 实例的数量。
     * @return Frame 实例的数量。
     */
    static int dbg_numFrames();

    /**
     * @brief 返回是否已经为此框架发出了 deleteLater() 调用。
     * @return 如果已计划删除，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool beingDeletedLater() const;

    /**
     * @brief 返回标签页是否可见。
     * @sa hasTabsVisibleChanged()
     * @return 如果标签页可见，则返回 true。
     **/
    [[nodiscard]] bool hasTabsVisible() const;

    /**
     * @brief 获取此框架的亲和性 (affinities) 列表。
     * @return 亲和性名称的字符串列表。
     */
    [[nodiscard]] QStringList affinities() const;

    /**
     * @brief 设置包含此 Frame 的布局项或占位符。
     * @param item 指向布局项的指针。
     */
    void setLayoutItem(Layouting::Item *item) override;

    /**
     * @brief 返回可拖拽区域的矩形（全局坐标）。
     *
     * 通常是标题栏的矩形。然而，当使用 Config::Flag_HideTitleBarWhenTabsVisible 时，它将是标签栏的背景区域。
     * 返回的是全局坐标。
     * @return 可拖拽区域的 QRect。
     */
    [[nodiscard]] virtual QRect dragRect() const;

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
     * @brief 通常我们通过原生窗口管理器进行大小调整，但如果小部件像 MDI 模式或覆盖模式那样停靠，
     * 那么我们允许用户通过鼠标调整大小。
     * @param sides 允许调整大小的边。
     */
    void setAllowedResizeSides(CursorPositions sides);

    /**
     * @brief 返回此框架是否在 MDI 布局中。
     * 通常不是，除非您正在使用 MDI 主窗口。
     * @return 如果在 MDI 布局中，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isMDI() const;

    /**
     * @brief 返回此框架是否是为支持 DockWidget::Option_MDINestable 而自动创建的。
     * @return 如果是 MDI 包装器框架，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isMDIWrapper() const;

    /**
     * @brief 如果这是一个 MDI 包装器框架，则返回 MDI 包装器 DockWidget。
     * @sa isMDIWrapper
     * @return 指向 MDI 包装器 DockWidgetBase 的指针。
     */
    [[nodiscard]] DockWidgetBase *mdiDockWidgetWrapper() const;

    /**
     * @brief 如果这是一个 MDI 包装器框架，则返回 MDI 包装器 DropArea。
     * @sa isMDIWrapper
     * @return 指向 MDI 包装器 DropArea 的指针。
     */
    [[nodiscard]] DropArea *mdiDropAreaWrapper() const;

    /**
     * @brief 如果此框架是一个 MDI 包装器，则返回 MDI 框架。即您实际在 MDI 区域内拖动的框架。
     * @return 指向 MDI 框架的指针。
     */
    [[nodiscard]] Frame *mdiFrame() const;

    /**
     * @brief 返回此框架所在的 MDI 布局。如果此框架不在 MDI 布局中，则返回 nullptr。
     * @return 指向 MDILayoutWidget 的指针，或者 nullptr。
     */
    [[nodiscard]] MDILayoutWidget *mdiLayoutWidget() const;

    /**
     * @brief 如果此框架是一个 MDI 框架 (isMDI() == true)，则返回它是否包含嵌套的停靠小部件 (DockWidget::Option_MDINestable)。
     * @sa isMDI()
     * @return 如果包含嵌套的 MDI 停靠小部件，则返回 true。
     */
    [[nodiscard]] bool hasNestedMDIDockWidgets() const;

    /**
     * @brief 获取用户定义的类型。
     * @sa DockWidgetBase::userType()
     * @return 用户定义的整数类型。
     */
    [[nodiscard]] int userType() const;

    /**
     * @brief 返回大小调整处理器。主要在 MDI 模式下使用。
     * @return 指向 WidgetResizeHandler 的指针。
     */
    [[nodiscard]] WidgetResizeHandler *resizeHandler() const;

    /**
     * @brief 当此组浮动时，用户请求的浮动窗口标志。
     * @return FloatingWindowFlags 枚举值。
     */
    [[nodiscard]] FloatingWindowFlags requestedFloatingWindowFlags() const;

Q_SIGNALS:
    /**
     * @brief 当当前激活的停靠小部件改变时发射此信号。
     * @param dockWidget 指向新的当前停靠小部件。
     */
    void currentDockWidgetChanged(KDDockWidgets::DockWidgetBase *dockWidget);
    /**
     * @brief 当框架内停靠小部件的数量改变时发射此信号。
     */
    void numDockWidgetsChanged();
    /**
     * @brief 当标签页的可见性状态改变时发射此信号。
     */
    void hasTabsVisibleChanged();
    /**
     * @brief 当布局失效时（例如，需要重新计算大小时）发射此信号。
     */
    void layoutInvalidated();
    /**
     * @brief 当框架是否在主窗口内的状态改变时发射此信号。
     */
    void isInMainWindowChanged();
    /**
     * @brief 当框架的焦点状态改变时发射此信号 (继承自 FocusScope)。
     */
    void isFocusedChanged();
    /**
     * @brief 当框架内的焦点小部件改变时发射此信号 (继承自 FocusScope)。
     */
    void focusedWidgetChanged();
    /**
     * @brief 当框架的 MDI 状态改变时发射此信号。
     */
    void isMDIChanged();
    /**
     * @brief 当实际显示的标题栏改变时发射此信号。
     */
    void actualTitleBarChanged();

protected:
    /**
     * @brief 当焦点状态改变时的回调函数 (来自 FocusScope)。
     */
    void isFocusedChangedCallback() final;
    /**
     * @brief 当焦点小部件改变时的回调函数 (来自 FocusScope)。
     */
    void focusedWidgetChangedCallback() final;

protected Q_SLOTS:
    /**
     * @brief 当停靠小部件数量改变时的槽函数。
     */
    void onDockWidgetCountChanged();
    /**
     * @brief 当当前标签页改变时的槽函数。
     * @param index 新的当前标签页索引。
     */
    void onCurrentTabChanged(int index);

protected:
    /**
     * @brief 重命名指定索引处的标签页 (纯虚函数，由具体实现如 FrameWidget 提供)。
     * @param index 要重命名的标签页索引。
     * @param text 新的标签页文本。
     */
    virtual void renameTab(int index, const QString &text) = 0;
    /**
     * @brief 更改指定索引处标签页的图标 (纯虚函数，由具体实现如 FrameWidget 提供)。
     * @param index 要更改图标的标签页索引。
     * @param icon 新的图标。
     */
    virtual void changeTabIcon(int index, const QIcon &icon) = 0;

    /**
     * @brief 返回停靠小部件的最小尺寸。
     * 由于 QTabWidget 可能有边距和标签栏，这个尺寸可能比 Frame::minSize() 略小。
     * @return 停靠小部件的最小 QSize。
     */
    [[nodiscard]] QSize dockWidgetsMinSize() const;

    /**
     * @brief 返回所有停靠小部件中最大的组合 maxSize。
     *
     * 示例:
     * dock 1, max=2000x1000
     * dock 2, max=3000x400
     * dock 3, max= (无限制)
     * 结果=3000,1000
     *
     * 任何具有 16777215x16777215 (QWIDGETSIZE_MAX，表示没有最大尺寸) 的小部件都将被忽略。
     * @return 最大的组合 QSize。
     */
    [[nodiscard]] QSize biggestDockWidgetMaxSize() const;

    // 以下为纯虚函数或受保护的虚函数，由具体的 Frame 实现（例如 FrameWidget）提供
    /**
     * @brief 移除停靠小部件的具体实现。
     */
    virtual void removeWidget_impl(DockWidgetBase *) = 0;
    /**
     * @brief 获取停靠小部件索引的具体实现。
     */
    [[nodiscard]] virtual int indexOfDockWidget_impl(const DockWidgetBase *) = 0;
    /**
     * @brief 获取当前标签页索引的具体实现。
     */
    [[nodiscard]] virtual int currentIndex_impl() const = 0;
    /**
     * @brief 设置当前标签页索引的具体实现。
     */
    virtual void setCurrentTabIndex_impl(int index) = 0;
    /**
     * @brief 设置当前停靠小部件的具体实现。
     */
    virtual void setCurrentDockWidget_impl(DockWidgetBase *) = 0;
    /**
     * @brief 插入停靠小部件的具体实现。
     */
    virtual void insertDockWidget_impl(DockWidgetBase *, int index) = 0;
    /**
     * @brief 获取指定索引处停靠小部件的具体实现。
     */
    [[nodiscard]] virtual DockWidgetBase *dockWidgetAt_impl(int index) const = 0;
    /**
     * @brief 获取当前停靠小部件的具体实现。
     */
    [[nodiscard]] virtual DockWidgetBase *currentDockWidget_impl() const = 0;
    /**
     * @brief 获取非内容区域的高度（例如标题栏和标签栏的高度总和）的具体实现。
     */
    [[nodiscard]] virtual int nonContentsHeight() const = 0;

private:
    /// @brief 标记是否在构造函数中，由 UBSAN 指出需要早期初始化。
    bool m_inCtor = true;

protected:
    /// @brief 标记是否在析构函数中。
    bool m_inDtor = false;

    /// @brief 指向内部标签页小部件的常量指针。
    TabWidget *const m_tabWidget;
    /// @brief 指向内部标题栏的常量指针。
    TitleBar *const m_titleBar;

private:
    Q_DISABLE_COPY(Frame) ///< 禁止拷贝构造函数和拷贝赋值操作符。
    // 声明友元类，允许它们访问此类的私有成员
    friend class ::TestDocks; // 测试类
    friend class TabWidget; // 标签页小部件类

    /**
     * @brief 安排延迟删除此框架。
     */
    void scheduleDeleteLater();
    /**
     * @brief Qt 事件处理函数。
     * @param event 事件对象。
     * @return 如果事件被处理，则返回 true。
     */
    bool event(QEvent *event) override;

    /**
     * @brief 设置此框架所在的 LayoutWidget。
     * @param layoutWidget 指向 LayoutWidget 的指针。
     */
    void setLayoutWidget(LayoutWidget *layoutWidget);

    /// @brief 指向此框架所在的 LayoutWidget。
    LayoutWidget *m_layoutWidget = nullptr;
    /// @brief 指向大小调整处理器的指针。
    WidgetResizeHandler *m_resizeHandler = nullptr;
    /// @brief 此框架的选项。
    FrameOptions m_options = FrameOption_None;
    /// @brief 指向关联的布局项的 QPointer。
    QPointer<Layouting::Item> m_layoutItem;
    /// @brief 标记是否正在更新标题栏，防止重入。
    bool m_updatingTitleBar = false;
    /// @brief 标记此框架是否已被安排延迟删除。
    bool m_beingDeleted = false;
    /// @brief 用户定义的类型。
    int m_userType = 0;
    /// @brief 用于连接可见小部件数量改变信号的连接对象。
    QMetaObject::Connection m_visibleWidgetCountChangedConnection;
};

} // namespace KDDockWidgets

/**
 * @brief QDebug 操作符重载，用于方便地输出 Frame 对象的信息。
 * @param d QDebug 对象。
 * @param frame 指向 Frame 对象的指针。
 * @return QDebug 对象，以便链式调用。
 */
inline QDebug operator<<(QDebug d, KDDockWidgets::Frame *frame)
{
    if (frame) {
        d << static_cast<QObject *>(frame); // 输出 QObject 信息
        d << "; window=" << frame->window(); // 输出所在窗口信息
        d << "; options=" << frame->options(); // 输出框架选项
        d << "; dockwidgets=" << frame->dockWidgets(); // 输出包含的停靠小部件列表
    } else {
        d << "nullptr"; // 如果指针为空，则输出 "nullptr"
    }

    return d;
}

#endif // KD_FRAME_P_H