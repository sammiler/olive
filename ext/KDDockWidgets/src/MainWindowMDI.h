/**
 * @file
 * @brief MainWindow (主窗口) 的子类，使用 MDI (多文档界面) 作为其布局方式。
 *
 * @author Sérgio Martins <sergio.martins@kdab.com>
 */

#ifndef KD_MAINWINDOW_MDI_H
#define KD_MAINWINDOW_MDI_H

#ifdef KDDOCKWIDGETS_QTWIDGETS // 条件编译：如果 KDDockWidgets 是为 Qt Widgets 构建的
#include "MainWindow.h" // 包含标准的 MainWindow 类 (QtWidgets 版本)
#else // 否则 (即为 QtQuick 构建)
#include "private/quick/MainWindowQuick_p.h" // 包含 QtQuick 版本的 MainWindow 私有头文件 (或其基类)
// 注意：MDIMainWindowBase 的定义可能依赖于此选择
#endif

namespace KDDockWidgets {

/**
 * @brief MainWindow 的一个子类，它使用 MDI (多文档界面) 作为其主要布局方式。
 *
 * 在 MDI 布局中，停靠小部件 (DockWidgetBase) 通常表现为可以自由浮动、层叠或平铺的独立子窗口，
 * 而不是像传统停靠系统中那样严格地分割主窗口区域。
 * 此类继承自 KDDockWidgets::MDIMainWindowBase (MDIMainWindowBase 的定义未在此文件中，
 * 但根据上下文，它应该是 MainWindowBase 的一个专门用于 MDI 的变体，或者是一个根据平台
 * (QtWidgets/QtQuick) 决定的类型别名)。
 */
class DOCKS_EXPORT MainWindowMDI : public KDDockWidgets::MDIMainWindowBase // MDIMainWindowBase 是实际的基类
{
Q_OBJECT // Q_OBJECT 宏，用于启用 Qt 元对象系统特性，如信号和槽
    public :
    /**
     * @brief 构造函数。详细信息请参见基类的文档。
     * @param uniqueName 强制性的名称，在所有 MainWindow 实例之间应该是唯一的。
     * 此名称不会对用户可见，仅在内部用于保存/恢复布局。
     * @param parent 父 WidgetType (QWidget* 或 QQuickItem*) 对象，默认为 nullptr。
     * @param flags 窗口标志，默认为 Qt::WindowFlags()。
     */
    explicit MainWindowMDI(const QString &uniqueName, WidgetType *parent = nullptr,
                           Qt::WindowFlags flags = Qt::WindowFlags());

    /**
     * @brief 析构函数。
     */
    ~MainWindowMDI() override;

    /**
     * @brief 将指定的停靠小部件 @p dockWidget 添加（停靠）到此 MDI 区域中。
     * 小部件将被放置在指定的局部坐标 @p localPos 处。
     * @param dockWidget 要添加的 DockWidgetBase 对象。
     * @param localPos 停靠小部件在 MDI 区域内的目标左上角局部坐标。
     * @param addingOption 添加选项，例如初始可见性，默认为空 (通常表示立即显示)。
     */
    void addDockWidget(DockWidgetBase *dockWidget, QPoint localPos, InitialOption addingOption = {});

    /**
     * @brief (便利性重载) 将指定的停靠小部件 @p dockWidget 添加到此 MDI 区域中。
     * 小部件将被放置在指定的局部坐标 @p localPos 处。此版本接受 QPointF。
     * @param dockWidget 要添加的 DockWidgetBase 对象。
     * @param localPos 停靠小部件在 MDI 区域内的目标左上角局部坐标 (使用 QPointF)。
     * @param addingOption 添加选项，默认为空。
     */
    void addDockWidget(DockWidgetBase *dockWidget, QPointF localPos, InitialOption addingOption = {});
};

} // namespace KDDockWidgets

#endif // KD_MAINWINDOW_MDI_H
