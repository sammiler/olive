#ifndef KD_FRAME_WIDGET_P_H
#define KD_FRAME_WIDGET_P_H

#ifdef KDDOCKWIDGETS_QTWIDGETS // 仅当定义了 KDDOCKWIDGETS_QTWIDGETS 宏时才编译以下内容 (表示在 Qt Widgets 环境下)

#include "../Frame_p.h" // 包含 Frame 私有头文件 (FrameWidget 的基类)

QT_BEGIN_NAMESPACE   // Qt 命名空间开始
class QTabBar;       // 前向声明 QTabBar 类，用于显示标签页
QT_END_NAMESPACE     // Qt 命名空间结束

class TestDocks;     // 前向声明 TestDocks 类，可能用于单元测试

namespace KDDockWidgets {

class TabWidget;    // 前向声明 TabWidget 类 (可能是 KDDockWidgets 内部的标签页部件)

/**
 * @brief FrameWidget 类是 Frame 类的图形用户界面 (GUI) 对应实现。
 *
 * 它继承自 Frame 类，并实现了具体的绘制逻辑 (paintEvent) 以及其他与 Qt Widgets 相关的用户界面功能。
 * FrameWidget 通常用于容纳一个或多个 DockWidgetBase 对象，并通过一个 QTabBar (如果多于一个子部件)
 * 来管理这些子部件的切换。
 * 它是 KDDockWidgets 框架中停靠区域的主要可视化组件。
 */
class DOCKS_EXPORT FrameWidget : public Frame
{
    Q_OBJECT // 启用 Qt 元对象系统特性，如信号和槽
public:
    /**
     * @brief 构造一个 FrameWidget 对象。
     * @param parent 父 QWidget，默认为 nullptr。
     * @param options 控制 Frame 行为的选项，默认为 FrameOption_None。
     * @param userType 用户定义的类型标识符，默认为 0。
     */
    explicit FrameWidget(QWidget *parent = nullptr, FrameOptions options = FrameOption_None,
                         int userType = 0);

    /**
     * @brief 析构函数。
     */
    ~FrameWidget() override;

    /**
     * @brief 返回此 FrameWidget 内部使用的 QTabBar 实例。
     *
     * 如果 FrameWidget 当前不显示标签页 (例如，只有一个停靠部件且配置为不显示单个标签)，
     * 则可能返回 nullptr。
     * @return 指向 QTabBar 的指针，如果不存在则为 nullptr。
     */
    [[nodiscard]] QTabBar *tabBar() const;

protected:
    /**
     * @brief 重写 QWidget::paintEvent()，用于自定义 FrameWidget 的绘制。
     *
     * 可能用于绘制边框、背景或当 Frame 为空时的提示信息。
     * @param event 绘制事件对象。
     */
    void paintEvent(QPaintEvent *) override;

    /**
     * @brief 重写基类 Frame 的方法，获取此 FrameWidget 的最大尺寸提示。
     * @return 最大建议尺寸。
     */
    [[nodiscard]] QSize maxSizeHint() const override;

    /**
     * @brief 实现基类 Frame 的方法，返回指定停靠部件在此 Frame 中的索引。
     * @param dockWidget 要查找索引的 DockWidgetBase 对象。
     * @return DockWidgetBase 在标签栏中的索引；如果未找到，则返回 -1。
     */
    int indexOfDockWidget_impl(const DockWidgetBase *dockWidget) override;

    /**
     * @brief 实现基类 Frame 的方法，设置当前显示的停靠部件。
     * @param dockWidget 要设置为当前显示的 DockWidgetBase 对象。
     */
    void setCurrentDockWidget_impl(DockWidgetBase *dockWidget) override;

    /**
     * @brief 实现基类 Frame 的方法，获取当前活动标签页的索引。
     * @return 当前活动标签页的索引；如果没有活动标签页，则返回 -1。
     */
    [[nodiscard]] int currentIndex_impl() const override;

    /**
     * @brief 实现基类 Frame 的方法，将一个停靠部件插入到指定的索引位置。
     * @param dockWidget 要插入的 DockWidgetBase 对象。
     * @param index 插入的目标索引。
     */
    void insertDockWidget_impl(DockWidgetBase *dockWidget, int index) override;

    /**
     * @brief 实现基类 Frame 的方法，从 Frame 中移除一个停靠部件。
     * @param dockWidget 要移除的 DockWidgetBase 对象。
     */
    void removeWidget_impl(DockWidgetBase *dockWidget) override;

    /**
     * @brief 实现基类 Frame 的方法，根据索引设置当前活动的标签页。
     * @param index 要设置为活动状态的标签页的索引。
     */
    void setCurrentTabIndex_impl(int index) override;

    /**
     * @brief 实现基类 Frame 的方法，获取当前显示的停靠部件。
     * @return 指向当前显示的 DockWidgetBase 对象的指针；如果没有，则返回 nullptr。
     */
    [[nodiscard]] DockWidgetBase *currentDockWidget_impl() const override;

    /**
     * @brief 实现基类 Frame 的方法，获取指定索引处的停靠部件。
     * @param index 目标停靠部件的索引。
     * @return 指向指定索引处 DockWidgetBase 对象的指针；如果索引无效，则返回 nullptr。
     */
    [[nodiscard]] DockWidgetBase *dockWidgetAt_impl(int index) const override;

    /**
     * @brief 重写基类 Frame 的方法，获取用于拖动的矩形区域。
     *
     * 这通常是标题栏或标签栏的区域，用户可以通过拖动此区域来移动整个 Frame。
     * @return 可用于拖动的 QRect。
     */
    [[nodiscard]] QRect dragRect() const override;

    /**
     * @brief 重写基类 Frame 的方法，重命名指定索引处的标签页。
     * @param index 要重命名的标签页的索引。
     * @param newName 新的标签页名称。
     */
    void renameTab(int index, const QString &newName) override;

    /**
     * @brief 重写基类 Frame 的方法，更改指定索引处标签页的图标。
     * @param index 要更改图标的标签页的索引。
     * @param icon 新的图标。
     */
    void changeTabIcon(int index, const QIcon &icon) override;

    /**
     * @brief 重写基类 Frame 的方法，获取 Frame 中非内容区域的高度。
     *
     * 这通常包括标题栏和/或标签栏的高度。
     * @return 非内容区域的高度（像素）。
     */
    [[nodiscard]] int nonContentsHeight() const override;

    /**
     * @brief 重写 QWidget::minimumSizeHint()，提供 FrameWidget 的最小尺寸提示。
     *
     * 这个尺寸会考虑到标题栏、标签栏以及所包含的停靠部件的最小尺寸。
     * @return FrameWidget 的最小建议尺寸。
     */
    [[nodiscard]] QSize minimumSizeHint() const override;

private:
    friend class ::TestDocks; // TestDocks 类是友元类，允许访问私有成员以进行单元测试。
};


} // namespace KDDockWidgets

#endif // KDDOCKWIDGETS_QTWIDGETS


#endif // KD_FRAME_WIDGET_P_H
