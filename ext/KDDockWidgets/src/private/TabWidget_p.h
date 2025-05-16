#ifndef KD_TAB_WIDGET_P_H
#define KD_TAB_WIDGET_P_H

#include "kddockwidgets/docks_export.h"    // 导入导出宏定义
#include "kddockwidgets/DockWidgetBase.h"  // 停靠小部件基类

#include "Draggable_p.h"                   // Draggable 私有头文件 (可拖拽对象接口)
#include "Frame_p.h"                       // Frame 私有头文件 (停靠小部件的容器)

#include <QVector> // Qt 动态数组容器

#include <memory>  // C++ 标准库智能指针

namespace KDDockWidgets {

// 前向声明
class DockWidgetBase; // 停靠小部件基类
class TabWidget;      // 标签页小部件类 (在此文件中稍后定义)

/**
 * @brief 一个 QTabBar 派生类（或类似功能的类），用于 KDDockWidgets::TabWidget。
 *
 * TabBar 负责显示标签页的头部，并处理与标签相关的交互，例如点击切换标签、
 * 拖拽标签以重新排序或将其拖出成为新的浮动窗口。
 * 它继承自 Draggable，表明整个标签栏或者其上的单个标签页可以被拖拽。
 */
class DOCKS_EXPORT TabBar : public Draggable
{
public:
    /**
     * @brief 构造一个新的 TabBar。
     * @param thisWidget 指向此 TabBar 关联的 QWidgetOrQuick 实例。
     * @param tabWidget 父 TabWidget 对象，默认为 nullptr。
     */
    explicit TabBar(QWidgetOrQuick *thisWidget, TabWidget *tabWidget = nullptr);

    /**
     * @brief 返回位于标签号 @p index 处的停靠小部件。
     * @param index 标签号，我们希望从中获取停靠小部件。
     * @return 位于标签号 @p index 处的停靠小部件。
     */
    [[nodiscard]] DockWidgetBase *dockWidgetAt(int index) const;

    /**
     * @brief (重载) 返回位于局部坐标 @p localPos 处的停靠小部件。
     * @param localPos 标签栏内的局部坐标点。
     * @return 如果该点位于某个标签上，则返回对应的停靠小部件；否则返回 nullptr。
     */
    [[nodiscard]] DockWidgetBase *dockWidgetAt(QPoint localPos) const;

    // Draggable 接口实现:
    /**
     * @brief 实现 Draggable 接口：将标签栏或其上的标签页包装成一个 WindowBeingDragged 对象。
     * 通常在拖拽标签页以使其浮动时调用。
     * @return 一个包含被拖拽窗口/标签页信息的 std::unique_ptr<WindowBeingDragged>。
     */
    std::unique_ptr<WindowBeingDragged> makeWindow() override;
    /**
     * @brief 实现 Draggable 接口：返回此对象是否已经是一个窗口。
     * 对于 TabBar 本身，这通常返回 false，因为它是 Frame 的一部分。
     * @return 如果是独立窗口则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isWindow() const override;

    /**
     * @brief 处理鼠标在标签栏上按下的事件。
     * @param localPos 鼠标按下的局部坐标。
     */
    void onMousePress(QPoint localPos);
    /**
     * @brief 处理鼠标在标签栏上双击的事件。
     * @param localPos 鼠标双击的局部坐标。
     */
    void onMouseDoubleClick(QPoint localPos) const;

    /**
     * @brief 返回标签栏中是否只有一个标签页（即只有一个停靠小部件）。
     * @return 如果只有一个停靠小部件，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool hasSingleDockWidget() const;

    /**
     * @brief 返回标签栏中停靠小部件的数量。
     * @return 停靠小部件的数量。
     */
    [[nodiscard]] int numDockWidgets() const;
    /**
     * @brief (纯虚函数) 返回位于局部坐标 @p localPos 处的标签的索引。
     * @param localPos 标签栏内的局部坐标点。
     * @return 如果该点位于某个标签上，则返回其索引；否则返回 -1 或其他无效索引。
     */
    [[nodiscard]] virtual int tabAt(QPoint localPos) const = 0;

    /**
     * @brief (纯虚函数) 返回指定索引处标签的文本。
     * @param index 标签的索引。
     * @return 标签的文本字符串。
     */
    [[nodiscard]] virtual QString text(int index) const = 0;

    /**
     * @brief 返回此 TabBar 类作为 QWidget (如果使用 QtWidgets) 或 QQuickItem。
     * @return 指向 QWidgetOrQuick 实例的指针。
     */
    [[nodiscard]] QWidgetOrQuick *asWidget() const;

    /**
     * @brief (纯虚函数) 返回指定索引处标签的矩形区域。
     * @param index 标签的索引。
     * @return 标签的 QRect。
     */
    [[nodiscard]] virtual QRect rectForTab(int index) const = 0;

    /**
     * @brief 实现 Draggable 接口：如果此标签栏仅包含一个停靠小部件，则返回它。
     * @return 指向单个 DockWidgetBase 的指针，如果不满足条件则返回 nullptr。
     */
    [[nodiscard]] DockWidgetBase *singleDockWidget() const override;

    /**
     * @brief (重写 Draggable 接口) 返回此对象是否为 MDI 窗口。
     * 对于 TabBar，通常返回其所属 Frame 的 isMDI() 状态。
     * @return 如果是 MDI 模式，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isMDI() const override;

    /**
     * @brief 获取此 TabBar 所属的 Frame。
     * @return 指向 Frame 对象的指针。
     */
    [[nodiscard]] Frame *frame() const;

    /**
     * @brief (纯虚函数) 移动标签页，类似于 QTabBar::moveTab(from, to)。
     * @param from 要移动的标签的起始索引。
     * @param to 目标索引。
     */
    virtual void moveTabTo(int from, int to) = 0;

private:
    TabWidget *const m_tabWidget; ///< 指向拥有此 TabBar 的 TabWidget 的常量指针。
    QPointer<DockWidgetBase> m_lastPressedDockWidget = nullptr; ///< 指向上次被按下的标签对应的停靠小部件的弱指针。
    QWidgetOrQuick *const m_thisWidget; ///< 指向此 TabBar 关联的 QWidgetOrQuick 实例的常量指针。
};

/**
 * @brief TabWidget 类是 KDDockWidgets 中对 QTabWidget 功能的抽象或封装。
 *
 * 它负责管理一组以标签页形式显示的停靠小部件 (DockWidgetBase)。
 * TabWidget 本身也可以被拖拽（通常是通过其 TabBar 或空白区域），
 * 并且是 Frame 的直接子元素。
 * 注意：这个类本身可能是一个抽象基类，具体的实现由 QtWidgets 或 QtQuick 版本提供。
 */
class DOCKS_EXPORT TabWidget : public Draggable
{
public:
    /**
     * @brief 构造一个新的 TabWidget，以 @p frame 作为父对象。
     * @param thisWidget 指向此 TabWidget 关联的 QWidgetOrQuick 实例。
     * @param frame 此 TabWidget 所属的 Frame 对象。
     */
    explicit TabWidget(QWidgetOrQuick *thisWidget, Frame *frame);

    /**
     * @brief (纯虚函数) 返回此 TabWidget 中的停靠小部件数量。
     * @return 停靠小部件的数量。
     */
    [[nodiscard]] virtual int numDockWidgets() const = 0;

    /**
     * @brief (纯虚函数) 从 TabWidget 中移除一个停靠小部件。
     * @param dw 要移除的 DockWidgetBase 对象。
     */
    virtual void removeDockWidget(DockWidgetBase *dw) = 0;

    /**
     * @brief (纯虚函数) 返回停靠小部件的索引，如果不存在则返回 -1。
     * @param dw 要查找索引的 DockWidgetBase 对象。
     * @return 索引，或者 -1（如果未找到）。
     */
    virtual int indexOfDockWidget(const DockWidgetBase *dw) const = 0;

    /**
     * @brief (纯虚函数) 设置当前的停靠小部件索引。
     * @param index 要设置为当前的标签页索引。
     */
    virtual void setCurrentDockWidget(int index) = 0;
    /**
     * @brief 设置指定的停靠小部件为当前激活的标签页。
     * @param dw 要设置为当前的 DockWidgetBase 对象。
     */
    void setCurrentDockWidget(DockWidgetBase *dw);

    /**
     * @brief 返回当前激活的停靠小部件。
     * @return 指向当前 DockWidgetBase 对象的指针。
     */
    [[nodiscard]] DockWidgetBase *currentDockWidget() const;

    /**
     * @brief (纯虚函数) 将停靠小部件插入到指定索引，并设置其图标和标题。
     * @param index 插入位置的索引。
     * @param dockWidget 要插入的 DockWidgetBase 对象。
     * @param icon 标签页的图标。
     * @param title 标签页的标题。
     * @return 如果插入成功，则返回 true；否则返回 false。
     */
    virtual bool insertDockWidget(int index, DockWidgetBase *dockWidget, const QIcon &icon, const QString &title) = 0;

    /**
     * @brief (纯虚函数) 设置标签栏是否在只有一个标签页时自动隐藏。
     * @param autoHide 如果为 true，则自动隐藏；否则不隐藏。
     */
    virtual void setTabBarAutoHide(bool autoHide) = 0;

    /**
     * @brief (纯虚函数) 重命名指定索引处标签页的文本。
     * @param index 要重命名的标签页索引。
     * @param title 新的标题文本。
     */
    virtual void renameTab(int index, const QString &title) = 0;

    /**
     * @brief (纯虚函数) 更改指定索引处标签页的图标。
     * @param index 要更改图标的标签页索引。
     * @param icon 新的图标。
     */
    virtual void changeTabIcon(int index, const QIcon &icon) = 0;

    /**
     * @brief (纯虚函数) 返回当前激活标签页的索引。
     * @return 当前索引。
     */
    [[nodiscard]] virtual int currentIndex() const = 0;

    /**
     * @brief 将一个停靠小部件追加到此 TabWidget 的末尾。
     * @param dw 要添加的 DockWidgetBase 对象。
     */
    void addDockWidget(DockWidgetBase *dw);

    /**
     * @brief (纯虚函数) 返回位于索引 @p index 处的停靠小部件。
     * @param index 标签页的索引。
     * @return 指向 DockWidgetBase 对象的指针。
     */
    [[nodiscard]] virtual DockWidgetBase *dockwidgetAt(int index) const = 0;

    /**
     * @brief 将 @p dockwidget 插入到 TabWidget 的 @p index 位置。
     * @param dockwidget 要插入的停靠小部件。
     * @param index 要插入到的索引位置。
     * @return 如果插入成功，则返回 true；否则返回 false。
     */
    bool insertDockWidget(DockWidgetBase *dockwidget, int index);

    /**
     * @brief 返回停靠小部件 @p dw 是否包含在此标签页小部件中。
     * 等同于 indexOfDockWidget(dw) != -1。
     * @param dw 要检查的 DockWidgetBase 对象。
     * @return 如果包含，则返回 true；否则返回 false。
     */
    bool contains(DockWidgetBase *dw) const;

    /**
     * @brief (纯虚函数) 返回此 TabWidget 使用的 TabBar。
     * @return 指向 TabBar 对象的指针。
     */
    [[nodiscard]] virtual TabBar *tabBar() const = 0;

    /**
     * @brief 返回此类作为 QWidget (如果使用 QtWidgets) 或 QQuickItem。
     * @return 指向 QWidgetOrQuick 实例的指针。
     */
    [[nodiscard]] QWidgetOrQuick *asWidget() const;

    /**
     * @brief 获取此 TabWidget 所属的 Frame。
     * @return 指向 Frame 对象的指针。
     */
    [[nodiscard]] Frame *frame() const;

    // Draggable 接口实现
    /**
     * @brief 实现 Draggable 接口：将此 TabWidget (通常指拖拽其 TabBar 的空白区域或整个标签组) 包装成一个 WindowBeingDragged 对象。
     * @return 一个包含被拖拽窗口/标签组信息的 std::unique_ptr<WindowBeingDragged>。
     */
    std::unique_ptr<WindowBeingDragged> makeWindow() override;
    /**
     * @brief 实现 Draggable 接口：如果此 TabWidget 仅包含一个停靠小部件，则返回它。
     * @return 指向单个 DockWidgetBase 的指针，如果不满足条件则返回 nullptr。
     */
    [[nodiscard]] DockWidgetBase *singleDockWidget() const override;
    /**
     * @brief 实现 Draggable 接口：返回此对象是否已经是一个窗口。
     * 对于 TabWidget 本身，这通常返回 false。
     * @return 如果是独立窗口则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isWindow() const override;

    /**
     * @brief (重写 Draggable 接口) 返回此对象是否为 MDI 窗口。
     * 对于 TabWidget，通常返回其所属 Frame 的 isMDI() 状态。
     * @return 如果是 MDI 模式，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isMDI() const override;

    // Q_SIGNALS: // 注意：此类可能不是 QObject 的直接派生类，或者信号在具体实现中定义。
    // 以下是期望的信号，具体的信号声明和发射机制取决于 QtWidgets 或 QtQuick 的具体实现。
    /**
     * @brief (纯虚信号) 当当前标签页改变时发射。
     * @param index 新的当前标签页索引。
     */
    virtual void currentTabChanged(int index) = 0;
    /**
     * @brief (纯虚信号) 当当前停靠小部件改变时发射。
     * @param dockWidget 指向新的当前停靠小部件。
     */
    virtual void currentDockWidgetChanged(KDDockWidgets::DockWidgetBase *dockWidget) = 0;
    /**
     * @brief (虚函数信号) 当标签页数量改变时发射。
     */
    virtual void countChanged()
    {
    }

protected:
    /**
     * @brief 当有标签页插入时的回调或处理函数。
     */
    void onTabInserted();
    /**
     * @brief 当有标签页移除时的回调或处理函数。
     */
    void onTabRemoved();
    /**
     * @brief (静态) 当当前标签页改变时的回调或处理函数。
     * @param index 新的当前标签页索引。
     */
    static void onCurrentTabChanged(int index);
    /**
     * @brief 处理鼠标双击事件。
     * @param localPos 鼠标双击的局部坐标。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    bool onMouseDoubleClick(QPoint localPos);

private:
    Frame *const m_frame; ///< 指向拥有此 TabWidget 的 Frame 的常量指针。
    QWidgetOrQuick *const m_thisWidget; ///< 指向此 TabWidget 关联的 QWidgetOrQuick 实例的常量指针。
    Q_DISABLE_COPY(TabWidget) ///< 禁止拷贝构造函数和拷贝赋值操作符。
};
} // namespace KDDockWidgets

#endif // KD_TAB_WIDGET_P_H