/**
 * @file
 * @brief 实现 TabWidgetWidget 的 QWidget 对应部分。处理 GUI，而 TabWidget 处理状态。
 *
 * @author Sérgio Martins <sergio.martins@kdab.com>
 */

#ifndef KDTABWIDGETWIDGET_P_H
#define KDTABWIDGETWIDGET_P_H

#ifdef KDDOCKWIDGETS_QTWIDGETS // 仅当定义了 KDDOCKWIDGETS_QTWIDGETS 宏时才编译以下内容 (表示在 Qt Widgets 环境下)
#include "../TabWidget_p.h" // 包含 TabWidget 私有头文件 (TabWidgetWidget 的基类之一)

#include <QTabWidget> // 包含 QTabWidget 类，这是 Qt Widgets 中标准的标签页部件

QT_BEGIN_NAMESPACE      // Qt 命名空间开始
class QAbstractButton;  // 前向声明 QAbstractButton 类
class QHBoxLayout;      // 前向声明 QHBoxLayout 类
QT_END_NAMESPACE        // Qt 命名空间结束

namespace KDDockWidgets {

class DockWidgetBase; // 前向声明 DockWidgetBase 类
class Frame;          // 前向声明 Frame 类 (TabWidgetWidget 的父级通常是 Frame)
class TabBar;         // 前向声明 TabBar 类 (TabWidgetWidget 内部使用一个 TabBar)

/**
 * @brief TabWidgetWidget 类是 QTabWidget 的一个子类，专门用于 KDDockWidgets 框架。
 *
 * 它继承自 QTabWidget 以提供标准的标签页部件功能，并继承自 KDDockWidgets::TabWidget
 * (可能是一个接口或包含核心逻辑的抽象基类，在此文件中通过 #include "../TabWidget_p.h" 引入)
 * 以集成 KDDockWidgets 的特定行为，例如管理作为标签页的停靠部件 (DockWidgetBase)、
 * 处理标签页的拖放、以及提供自定义的标签栏按钮（如浮动、关闭按钮）。
 *
 * TabWidgetWidget 负责标签页的 GUI 展示和用户交互，而其基类 KDDockWidgets::TabWidget
 * 可能负责更多的状态管理和核心停靠逻辑。
 */
class DOCKS_EXPORT TabWidgetWidget
    : public QTabWidget, // 继承自 QTabWidget，提供标准的标签页 UI 和行为
      public TabWidget    // 继承自 KDDockWidgets::TabWidget (内部接口/基类)，实现停靠特定的标签页逻辑
{
    Q_OBJECT // 启用 Qt 元对象系统特性，如信号和槽
public:
    /**
     * @brief 构造一个 TabWidgetWidget 对象。
     * @param parent 指向父 Frame 对象的指针。TabWidgetWidget 通常位于一个 Frame 内部。
     * @param options 控制 TabWidget 行为的选项，默认为 TabWidgetOption_None。
     */
    explicit TabWidgetWidget(Frame *parent, TabWidgetOptions options = TabWidgetOption_None);

    /**
     * @brief 返回此 TabWidgetWidget 内部使用的自定义 TabBar。
     *
     * 重写 KDDockWidgets::TabWidget::tabBar()。
     * @return 指向内部 TabBar 对象的指针。
     */
    [[nodiscard]] TabBar *tabBar() const override;

    /**
     * @brief 返回此标签页部件中包含的停靠部件的数量。
     *
     * 重写 KDDockWidgets::TabWidget::numDockWidgets()。
     * @return 停靠部件的数量。
     */
    [[nodiscard]] int numDockWidgets() const override;

    /**
     * @brief 从此标签页部件中移除指定的停靠部件。
     *
     * 重写 KDDockWidgets::TabWidget::removeDockWidget()。
     * @param dockWidget 要移除的 DockWidgetBase 对象。
     */
    void removeDockWidget(DockWidgetBase *) override;

    /**
     * @brief 返回指定停靠部件在此标签页部件中的索引。
     *
     * 重写 KDDockWidgets::TabWidget::indexOfDockWidget()。
     * @param dockWidget 要查找索引的 DockWidgetBase 对象。
     * @return 如果找到，则返回其索引；否则返回 -1。
     */
    int indexOfDockWidget(const DockWidgetBase *) const override;

Q_SIGNALS:
    /**
     * @brief 当当前选中的标签页发生改变时发射此信号。
     *
     * 重写 KDDockWidgets::TabWidget::currentTabChanged()。
     * @param index 新的当前标签页的索引。
     */
    void currentTabChanged(int index) override;

    /**
     * @brief 当当前活动的停靠部件发生改变时发射此信号。
     *
     * 重写 KDDockWidgets::TabWidget::currentDockWidgetChanged()。
     * @param dw 指向新的当前活动 DockWidgetBase 的指针。
     */
    void currentDockWidgetChanged(KDDockWidgets::DockWidgetBase *dw) override;

protected:
    /**
     * @brief 重写 QWidget::mouseDoubleClickEvent()，处理鼠标双击事件。
     *
     * 通常用于在标签栏区域双击时创建新的浮动窗口（如果配置允许）。
     * @param event 鼠标双击事件对象。
     */
    void mouseDoubleClickEvent(QMouseEvent *) override;

    /**
     * @brief 重写 QWidget::mousePressEvent()，处理鼠标按下事件。
     *
     * 主要用于处理标签栏上可能发生的拖拽操作的起始。
     * @param event 鼠标按下事件对象。
     */
    void mousePressEvent(QMouseEvent *) override;

    /**
     * @brief 当一个标签页被插入到 QTabWidget 中时调用此受保护的槽函数。
     *
     * 重写 QTabWidget::tabInserted()。
     * @param index 新插入标签页的索引。
     */
    void tabInserted(int index) override;

    /**
     * @brief 当一个标签页从 QTabWidget 中被移除时调用此受保护的槽函数。
     *
     * 重写 QTabWidget::tabRemoved()。
     * @param index 被移除标签页的索引。
     */
    void tabRemoved(int index) override;

    /**
     * @brief 检查标签栏上的某个位置是否可以开始拖动。
     *
     * 重写 KDDockWidgets::TabWidget::isPositionDraggable()。
     * @param p 要检查的位置 (通常在标签栏的坐标系中)。
     * @return 如果该位置可以开始拖动，则返回 true。
     */
    [[nodiscard]] bool isPositionDraggable(QPoint p) const override;

    /**
     * @brief 设置当前活动的停靠部件（通过索引）。
     *
     * 重写 KDDockWidgets::TabWidget::setCurrentDockWidget()。
     * @param index 要设置为当前活动部件的标签页索引。
     */
    void setCurrentDockWidget(int index) override;

    /**
     * @brief 将一个停靠部件作为新的标签页插入到指定索引。
     *
     * 重写 KDDockWidgets::TabWidget::insertDockWidget()。
     * @param index 插入的目标索引。
     * @param dockWidget 要插入的 DockWidgetBase 对象。
     * @param icon 标签页的图标。
     * @param title 标签页的标题。
     * @return 如果插入成功，则返回 true。
     */
    bool insertDockWidget(int index, DockWidgetBase *, const QIcon &icon, const QString &title) override;

    /**
     * @brief 设置当只有一个标签页时是否自动隐藏标签栏。
     *
     * 重写 KDDockWidgets::TabWidget::setTabBarAutoHide()。
     * @param autoHide 如果为 true，则自动隐藏；否则不隐藏。
     */
    void setTabBarAutoHide(bool autoHide) override;

    /**
     * @brief 重命名指定索引处的标签页。
     *
     * 重写 KDDockWidgets::TabWidget::renameTab()。
     * @param index 要重命名的标签页的索引。
     * @param newName 新的标签页名称。
     */
    void renameTab(int index, const QString &newName) override;

    /**
     * @brief 更改指定索引处标签页的图标。
     *
     * 重写 KDDockWidgets::TabWidget::changeTabIcon()。
     * @param index 要更改图标的标签页的索引。
     * @param icon 新的图标。
     */
    void changeTabIcon(int index, const QIcon &icon) override;

    /**
     * @brief 返回指定索引处的停靠部件。
     *
     * 重写 KDDockWidgets::TabWidget::dockwidgetAt()。
     * @param index 目标索引。
     * @return 指向 DockWidgetBase 的指针；如果索引无效，则返回 nullptr。
     */
    [[nodiscard]] DockWidgetBase *dockwidgetAt(int index) const override;

    /**
     * @brief 返回当前活动标签页的索引。
     *
     * 重写 KDDockWidgets::TabWidget::currentIndex()。
     * @return 当前活动标签页的索引；如果没有活动标签页，则返回 -1。
     */
    [[nodiscard]] int currentIndex() const override;

    /**
     * @brief 显示上下文菜单。可以重写此方法来实现自定义的上下文菜单。
     *
     * 默认情况下，它用于支持 Config::Flag_AllowSwitchingTabsViaMenu 配置，
     * 即允许通过菜单切换标签页。
     * @param pos 请求显示上下文菜单的位置 (通常是全局坐标或相对于此部件的坐标)。
     */
    virtual void showContextMenu(QPoint pos);

private:
    /**
     * @brief 更新标签页部件的边距。
     */
    void updateMargins();

    /**
     * @brief 设置标签栏角落的小部件（例如浮动按钮、关闭按钮）。
     */
    void setupTabBarButtons();

    Q_DISABLE_COPY(TabWidgetWidget) // 禁止拷贝构造函数和赋值操作符

    TabBar *const m_tabBar;                       ///< 指向内部使用的自定义 TabBar 对象的常量指针。
    QHBoxLayout *m_cornerWidgetLayout = nullptr;  ///< 用于放置标签栏角落按钮的水平布局。
    QAbstractButton *m_floatButton = nullptr;     ///< 指向“浮动”按钮的指针 (如果存在)。
    QAbstractButton *m_closeButton = nullptr;     ///< 指向“关闭”按钮的指针 (如果存在)。
};
} // namespace KDDockWidgets

#endif // KDDOCKWIDGETS_QTWIDGETS


#endif // KDTABWIDGETWIDGET_P_H
