#ifndef KD_SIDEBARWIDGET_P_H
#define KD_SIDEBARWIDGET_P_H

#ifdef KDDOCKWIDGETS_QTWIDGETS // 仅当定义了 KDDOCKWIDGETS_QTWIDGETS 宏时才编译以下内容 (表示在 Qt Widgets 环境下)

#include "kddockwidgets/docks_export.h" // 包含导出宏定义
#include "../SideBar_p.h"             // 包含 SideBar 私有头文件 (SideBarWidget 的基类)

#include <QToolButton> // 包含 QToolButton 类，用作侧边栏按钮的基类
#include <QPointer>    // 包含 QPointer 类，用于安全地跟踪 QObject 对象

QT_BEGIN_NAMESPACE     // Qt 命名空间开始
class QBoxLayout;      // 前向声明 QBoxLayout 类，用于线性布局
class QAbstractButton; // 前向声明 QAbstractButton 类 (虽然 SideBarButton 直接继承 QToolButton，但有时会包含以备不时之需)
QT_END_NAMESPACE       // Qt 命名空间结束

namespace KDDockWidgets {

class DockWidget;        // 前向声明 DockWidget 类
class DockWidgetBase;    // 前向声明 DockWidgetBase 类
class Frame;             // 前向声明 Frame 类
class SideBarWidget;     // 前向声明 SideBarWidget 类自身 (用于 SideBarButton 的父类型)
class MainWindowBase;    // 前向声明 MainWindowBase 类

/**
 * @brief SideBarButton 类代表 KDDockWidgets 侧边栏中的一个按钮。
 *
 * 每个按钮通常对应一个已停靠到侧边栏的 DockWidgetBase。
 * 当侧边栏是垂直的时，按钮文本会垂直绘制。
 * 此类继承自 QToolButton，提供了工具按钮的基本功能和外观。
 */
class DOCKS_EXPORT SideBarButton : public QToolButton
{
    Q_OBJECT // 启用 Qt 元对象系统特性
public:
    /**
     * @brief 构造一个 SideBarButton 对象。
     * @param dw 与此按钮关联的 DockWidgetBase 对象。
     * @param parent 指向父 SideBarWidget 的指针。
     */
    explicit SideBarButton(DockWidgetBase *dw, SideBarWidget *parent);

    /**
     * @brief 检查按钮是否应垂直显示其文本。
     *
     * 这通常取决于其所在的 SideBarWidget 的方向。
     * @return 如果按钮是垂直的（通常意味着侧边栏是垂直的），则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isVertical() const;

    /**
     * @brief 重写 QWidget::paintEvent()，用于自定义按钮的绘制。
     *
     * 特别是用于在按钮垂直时绘制旋转的文本。
     * @param event 绘制事件对象。
     */
    void paintEvent(QPaintEvent *) override;

    /**
     * @brief 重写 QWidget::sizeHint()，提供按钮的建议尺寸。
     *
     * 考虑了文本方向和图标。
     * @return 按钮的建议尺寸。
     */
    [[nodiscard]] QSize sizeHint() const override;

private:
    SideBarWidget *const m_sideBar;                   ///< 指向包含此按钮的 SideBarWidget 的常量指针。
    const QPointer<DockWidgetBase> m_dockWidget;      ///< 指向与此按钮关联的 DockWidgetBase 的 QPointer。使用 QPointer 是为了安全地处理 DockWidgetBase 可能被删除的情况。
};

/**
 * @brief SideBarWidget 类是 SideBar 在 Qt Widgets 环境下的具体实现。
 *
 * 它是一个 QWidget，用于在主窗口的边缘显示一组 SideBarButton，
 * 代表那些被“最小化”或“固定”到侧边栏的停靠部件。
 * 用户可以点击这些按钮来显示或隐藏相应的停靠部件。
 */
class DOCKS_EXPORT SideBarWidget : public SideBar
{
    Q_OBJECT // 启用 Qt 元对象系统特性
public:
    /**
     * @brief 构造一个 SideBarWidget 对象。
     * @param location 侧边栏在主窗口中的位置 (例如，左侧、右侧等)。
     * @param parent 指向父 KDDockWidgets::MainWindowBase 对象的指针。
     */
    explicit SideBarWidget(SideBarLocation location, KDDockWidgets::MainWindowBase *parent);

protected:
    /**
     * @brief 实现基类 SideBar 的方法，将一个停靠部件添加到侧边栏。
     *
     * 这通常涉及到创建一个新的 SideBarButton 并将其添加到侧边栏的布局中。
     * @param dw 要添加的 DockWidgetBase 对象。
     */
    void addDockWidget_Impl(DockWidgetBase *dw) override;

    /**
     * @brief 实现基类 SideBar 的方法，从侧边栏中移除一个停靠部件。
     *
     * 这通常涉及到查找并移除与给定 DockWidgetBase 关联的 SideBarButton。
     * @param dock 要移除的 DockWidgetBase 对象。
     */
    void removeDockWidget_Impl(DockWidgetBase *dock) override;

    /**
     * @brief (虚函数) 创建一个新的 SideBarButton 实例。
     *
     * 允许派生类提供自定义的按钮实现。
     * @param dw 与新按钮关联的 DockWidgetBase 对象。
     * @param parent 新按钮的父 SideBarWidget。
     * @return 指向新创建的 SideBarButton 的指针。
     */
    virtual SideBarButton *createButton(DockWidgetBase *dw, SideBarWidget *parent) const;

private:
    QBoxLayout *const m_layout; ///< 指向侧边栏内部布局的常量指针 (通常是 QVBoxLayout 或 QHBoxLayout，取决于侧边栏方向)。
};

} // namespace KDDockWidgets

#endif // KDDOCKWIDGETS_QTWIDGETS


#endif // KD_SIDEBARWIDGET_P_H
