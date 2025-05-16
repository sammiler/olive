/**
 * @file
 * @brief MDIArea (多文档界面区域) 类，允许在 MDI 模式下托管停靠小部件。
 *
 * @author Sérgio Martins <sergio.martins@kdab.com>
 */

#ifndef KDDOCKWIDGETS_MDI_AREA_H
#define KDDOCKWIDGETS_MDI_AREA_H

#include "kddockwidgets/docks_export.h" // 导入导出宏定义，用于库的符号可见性
#include "KDDockWidgets.h" // KDDockWidgets 公共头文件，包含核心枚举和类型定义
#include "QWidgetAdapter.h" // QWidget 和 QQuickItem 的适配器类

#include <QWidget> // 包含 QWidget 基类 (MDIArea 本身可以是一个 QWidget)

namespace KDDockWidgets {

// 前向声明 KDDockWidgets 内部或公共类
class MDILayoutWidget; // MDI 布局小部件类，MDIArea 内部会使用它
class DockWidgetBase; // 停靠小部件基类
class Frame; // 框架类 (容纳 DockWidgetBase)

/**
 * @brief MDIArea 类允许以 MDI (多文档界面) 模式托管停靠小部件。
 *
 * 这是使用完整 MainWindowMDI 的一种替代方案。
 * 使用场景是：当您已经有一个主窗口（进行常规停靠）并且希望添加一个区域，
 * 在该区域中使用 MDI 风格的停靠小部件时。您只需使用 MDIArea，
 * 而不是停靠一个 MainWindowMDI，从而避免嵌套主窗口。
 *
 * 请参见示例 examples/mdi_with_docking/。
 * MDIArea 继承自 QWidgetAdapter，因此它可以是 QWidget 或 QQuickItem 的包装。
 */
class DOCKS_EXPORT MDIArea : public QWidgetAdapter
{
Q_OBJECT // Q_OBJECT 宏，用于启用 Qt 元对象系统特性，如信号和槽
    public :
    /**
     * @brief 构造函数。
     * @param parent 父 QWidgetOrQuick 对象，默认为 nullptr。MDIArea 将作为此父对象的子控件。
     */
    explicit MDIArea(QWidgetOrQuick *parent = nullptr);
    /**
     * @brief 析构函数。
     */
    ~MDIArea() override;

    /**
     * @brief 将指定的停靠小部件 @p dw 添加（停靠）到此 MDI 区域中。
     * 小部件将被放置在指定的局部坐标 @p localPt 处。
     * @param dw 要添加的 DockWidgetBase 对象。
     * @param localPt 停靠小部件在 MDI 区域内的目标左上角局部坐标。
     * @param addingOption 添加选项，例如初始可见性，默认为空 (通常表示立即显示)。
     */
    void addDockWidget(DockWidgetBase *dw, QPoint localPt, InitialOption addingOption = {});

    /**
     * @brief 将停靠小部件 @p dw 移动到点 @p pos。
     * @param dw 要移动的 DockWidgetBase 对象。
     * @param pos 目标位置（在 MDI 区域的局部坐标系中）。
     */
    void moveDockWidget(DockWidgetBase *dw, QPoint pos);

    /**
     * @brief 将停靠小部件 @p dw 的大小调整为 @p size。
     * @param dw 要调整大小的 DockWidgetBase 对象。
     * @param size 新的尺寸。
     */
    void resizeDockWidget(DockWidgetBase *dw, QSize size);

    /**
     * @brief 返回此 MDI 区域中包含的框架 (Frame) 列表。
     * 每个 Frame 对象代表嵌入在 MDI 区域中的一个“窗口”。
     * @return Frame 指针的 QList。
     */
    [[nodiscard]] QList<Frame *> frames() const;

    /**
     * @brief (重写 QWidgetAdapter 中的虚函数) 处理关闭事件。
     * 将关闭事件转发给 MDI 区域内的所有停靠小部件，如果至少有一个停靠小部件不允许关闭，则中止关闭操作。
     * @param event 关闭事件对象。
     */
    void onCloseEvent(QCloseEvent *event) override;

private:
    class Private; ///< PIMPL (Private Implementation) 设计模式的私有实现类前向声明。
    Private *const d; ///< 指向私有实现类的常量指针。
};

} // namespace KDDockWidgets

#endif // KDDOCKWIDGETS_MDI_AREA_H
