#ifndef KD_DOCKWIDGETS_MDI_LAYOUT_WIDGET_P_H
#define KD_DOCKWIDGETS_MDI_LAYOUT_WIDGET_P_H

#include "LayoutWidget_p.h" // 包含父类 LayoutWidget 的私有头文件
#include "kddockwidgets/KDDockWidgets.h" // KDDockWidgets 公共头文件，包含枚举和基本类型
#include "kddockwidgets/docks_export.h" // 导入导出宏定义

// 前向声明内部布局引擎相关的类
namespace Layouting {
class ItemFreeContainer; // MDI 布局使用的自由容器项
}

namespace KDDockWidgets {

/**
 * @brief MDILayoutWidget 类实现了一种适用于 MDI (多文档界面) 风格停靠的布局。
 *
 * 在这种布局中，停靠小部件可以自由地放置在任意位置，不受传统分割布局的限制。
 * 它继承自 LayoutWidget，并使用 ItemFreeContainer 作为其根布局项，
 * 允许其子项（通常是 Frame）在二维空间内自由定位和调整大小。
 */
class DOCKS_EXPORT MDILayoutWidget : public LayoutWidget
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数。
     * @param parent 父 QWidgetOrQuick 对象，默认为 nullptr。
     */
    explicit MDILayoutWidget(QWidgetOrQuick *parent = nullptr);
    /**
     * @brief 析构函数。
     */
    ~MDILayoutWidget() override;

    /**
     * @brief 将停靠小部件停靠到此 MDI 区域的指定位置。
     * @param dw 要添加的 DockWidgetBase 对象。
     * @param localPt 在 MDI 区域内的目标局部坐标点。
     * @param addingOption 添加选项，例如初始可见性，默认为空。
     */
    void addDockWidget(DockWidgetBase *dw, QPoint localPt, InitialOption addingOption = {});

    /**
     * @brief 将停靠小部件 @p dw 移动到点 @p pos。
     * @param dw 要移动的 DockWidgetBase 对象。
     * @param pos 目标位置（在 MDI 区域的局部坐标系中）。
     */
    void moveDockWidget(DockWidgetBase *dw, QPoint pos);

    /**
     * @brief 将框架 @p f 移动到点 @p pos。
     * 这是 moveDockWidget(DockWidgetBase*, QPoint) 的便利性重载。
     * @param f 要移动的 Frame 对象。
     * @param pos 目标位置（在 MDI 区域的局部坐标系中）。
     */
    void moveDockWidget(Frame *f, QPoint pos);

    /**
     * @brief 将停靠小部件 @p dw 的大小调整为 @p size。
     * @param dw 要调整大小的 DockWidgetBase 对象。
     * @param size 新的尺寸。
     */
    void resizeDockWidget(DockWidgetBase *dw, QSize size);

    /**
     * @brief 将框架 @p f 的大小调整为 @p size。
     * 这是 resizeDockWidget(DockWidgetBase*, QSize) 的便利性重载。
     * @param f 要调整大小的 Frame 对象。
     * @param size 新的尺寸。
     */
    void resizeDockWidget(Frame *f, QSize size);

    /**
     * @brief 设置停靠小部件框架 @p f 的几何形状（位置和大小）。
     * @param f 要设置几何形状的 Frame 对象。
     * @param geometry 新的几何形状（QRect）。
     */
    void setDockWidgetGeometry(Frame *f, QRect geometry);

private:
    /// @brief 指向根布局项的常量指针。对于 MDILayoutWidget，这通常是一个 ItemFreeContainer，
    /// 允许其子项自由定位。
    Layouting::ItemFreeContainer *const m_rootItem;
};

} // namespace KDDockWidgets

#endif // KD_DOCKWIDGETS_MDI_LAYOUT_WIDGET_P_H