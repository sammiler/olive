/**
 * @file
 * @brief 实现一个 QTabWidget 派生类，支持将 KDockWidget::DockWidget 作为标签页进行停靠和取消停靠。
 *
 * @author Sérgio Martins <sergio.martins@kdab.com>
 */

#ifndef KD_TABBAR_WIDGET_P_H
#define KD_TABBAR_WIDGET_P_H

#ifdef KDDOCKWIDGETS_QTWIDGETS // 仅当定义了 KDDOCKWIDGETS_QTWIDGETS 宏时才编译以下内容 (表示在 Qt Widgets 环境下)

#include "../TabWidget_p.h" // 包含 TabWidget 私有头文件 (TabBarWidget 可能与之交互或作为其一部分)

#include <QTabBar> // 包含 QTabBar 类，这是 Qt Widgets 中标准的标签栏组件

QT_BEGIN_NAMESPACE     // Qt 命名空间开始
class QMouseEvent;     // 前向声明 QMouseEvent 类，用于处理鼠标事件
QT_END_NAMESPACE       // Qt 命名空间结束

// clazy:excludeall=ctor-missing-parent-argument
// 上一行是一个 clazy (静态分析器) 指令，用于在此文件中排除特定的警告，
// 即关于构造函数缺少父 QObject* 参数的警告。

namespace KDDockWidgets {

class DockWidget;     // 前向声明 DockWidget 类
class DockWidgetBase; // 前向声明 DockWidgetBase 类 (TabBarWidget 管理的是 DockWidgetBase)
class TabWidget;      // 前向声明 TabWidget 类 (TabBarWidget 的父级)

/**
 * @brief TabBarWidget 类是 QTabBar 的一个子类，专门用于 KDDockWidgets 框架。
 *
 * 它继承自 QTabBar 以提供标准的标签栏功能，并继承自 KDDockWidgets::TabBar (可能是一个接口或抽象基类，
 * 在此文件中未直接包含其定义，但从上下文推断) 以集成 KDDockWidgets 的特定行为，
 * 例如处理停靠部件 (DockWidgetBase) 作为标签页、支持拖拽停靠等。
 * 此类是 KDDockWidgets 中标签式停靠用户界面的核心组件。
 */
class DOCKS_EXPORT TabBarWidget
    : public QTabBar,  // 继承自 QTabBar，提供标准的标签栏 UI 和行为
      public TabBar     // 继承自 KDDockWidgets::TabBar (内部接口)，实现停靠特定的标签栏逻辑
{
    Q_OBJECT // 启用 Qt 元对象系统特性，如信号和槽
public:
    /**
     * @brief 构造一个 TabBarWidget 对象。
     * @param parent 指向父 TabWidget 的指针，默认为 nullptr。
     * TabBarWidget 通常是 TabWidget 的一部分。
     */
    explicit TabBarWidget(TabWidget *parent = nullptr);

    /**
     * @brief 返回位于指定局部坐标点的标签页索引。
     *
     * 重写 KDDockWidgets::TabBar::tabAt() (如果 TabBar 是一个定义的接口)。
     * @param localPos 相对于此 TabBarWidget 的局部坐标。
     * @return 对应位置的标签页索引；如果没有标签页在该位置，则返回 -1。
     */
    [[nodiscard]] int tabAt(QPoint localPos) const override;

    /**
     * @brief 返回当前选中的标签页所关联的 DockWidgetBase。
     * @return 指向当前活动 DockWidgetBase 的指针；如果没有选中的标签页或关联的部件，则返回 nullptr。
     */
    [[nodiscard]] DockWidgetBase *currentDockWidget() const;

    /**
     * @brief 返回指定索引处标签页的文本。
     *
     * 重写 KDDockWidgets::TabBar::text() (如果 TabBar 是一个定义的接口)。
     * @param index 标签页的索引。
     * @return 标签页的文本。
     */
    [[nodiscard]] QString text(int index) const override;

    /**
     * @brief 返回指定索引处标签页的几何矩形区域。
     *
     * 重写 KDDockWidgets::TabBar::rectForTab() (如果 TabBar 是一个定义的接口)。
     * @param index 标签页的索引。
     * @return 标签页的矩形区域。
     */
    [[nodiscard]] QRect rectForTab(int index) const override;

    /**
     * @brief 将标签页从一个索引位置移动到另一个索引位置。
     *
     * 重写 KDDockWidgets::TabBar::moveTabTo() (如果 TabBar 是一个定义的接口)。
     * @param from 原始索引。
     * @param to 目标索引。
     */
    void moveTabTo(int from, int to) override;

Q_SIGNALS:
    /**
     * @brief 当一个新的停靠部件作为标签页被插入时发射此信号。
     * @param index 新插入标签页的索引。
     */
    void dockWidgetInserted(int index);

    /**
     * @brief 当一个停靠部件从标签页中被移除时发射此信号。
     * @param index 被移除标签页的索引。
     */
    void dockWidgetRemoved(int index);

protected:
    /**
     * @brief 判断是否可以根据当前的鼠标按下位置和移动位置开始拖动操作。
     *
     * 重写 KDDockWidgets::TabBar::dragCanStart() (如果 TabBar 是一个定义的接口)。
     * @param pressPos 鼠标按下的初始位置。
     * @param pos 当前鼠标位置。
     * @return 如果可以开始拖动，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool dragCanStart(QPoint pressPos, QPoint pos) const override;

    /**
     * @brief 重写 QWidget::mousePressEvent()，处理鼠标按下事件。
     *
     * 用于启动标签页的拖放操作或选择标签页。
     * @param event 鼠标按下事件对象。
     */
    void mousePressEvent(QMouseEvent *) override;

    /**
     * @brief 重写 QWidget::mouseMoveEvent()，处理鼠标移动事件。
     *
     * 用于处理标签页的拖动。
     * @param e 鼠标移动事件对象。
     */
    void mouseMoveEvent(QMouseEvent *e) override;

    /**
     * @brief 重写 QWidget::mouseDoubleClickEvent()，处理鼠标双击事件。
     *
     * 可能用于触发特定操作，例如最大化/恢复停靠部件。
     * @param e 鼠标双击事件对象。
     */
    void mouseDoubleClickEvent(QMouseEvent *e) override;

    /**
     * @brief 重写 QObject::event()，用于更通用的事件处理。
     * @param event 事件对象。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    bool event(QEvent *) override;

    /**
     * @brief 当一个标签页被插入到 QTabBar 中时调用此受保护的槽函数。
     *
     * 重写 QTabBar::tabInserted()。
     * @param index 新插入标签页的索引。
     */
    void tabInserted(int index) override;

    /**
     * @brief 当一个标签页从 QTabBar 中被移除时调用此受保护的槽函数。
     *
     * 重写 QTabBar::tabRemoved()。
     * @param index 被移除标签页的索引。
     */
    void tabRemoved(int index) override;

private:
    TabWidget *const m_tabWidget; ///< 指向包含此标签栏的父 TabWidget 的常量指针。
};
} // namespace KDDockWidgets


#endif // KDDOCKWIDGETS_QTWIDGETS

#endif // KD_TABBAR_WIDGET_P_H
