/**
 * @file
 * @brief 代表一个停靠小部件。
 *
 * @author Sérgio Martins <sergio.martins@kdab.com>
 */

#ifndef KD_DOCKWIDGET_H
#define KD_DOCKWIDGET_H

#ifdef KDDOCKWIDGETS_QTWIDGETS // 条件编译：仅当为 Qt Widgets 构建时才包含此文件的内容

#include "DockWidgetBase.h" // 包含停靠小部件的基类定义

QT_BEGIN_NAMESPACE
// 前向声明 Qt 类
class QCloseEvent; // Qt 关闭事件类
QT_END_NAMESPACE

// clazy:excludeall=ctor-missing-parent-argument // clazy 静态分析器指令：排除所有关于构造函数缺少父参数的警告

namespace KDDockWidgets {

/**
 * @brief 代表一个可停靠的小部件。
 *
 * 大部分接口功能定义在 DockWidgetBase 中，以便于与 QtQuick 共享代码。
 *此类是用户在 QtWidgets 应用程序中创建和管理可停靠内容的主要入口点。
 *它封装了停靠、浮动、标签化以及与 KDDockWidgets 框架交互的复杂逻辑。
 */
class DOCKS_EXPORT DockWidget : public DockWidgetBase
{
Q_OBJECT // Q_OBJECT 宏，用于启用 Qt 元对象系统特性，如信号和槽
    public :
    /**
     * @brief 构造一个新的 DockWidget。
     * @param uniqueName 强制性的名称，在所有 DockWidget 实例之间应该是唯一的。
     * 此名称不会对用户可见，仅在内部用于保存/恢复布局。
     * 使用 setTitle() 设置用户可见的文本。
     * @param options 可选的选项，用于控制行为，默认为 KDDockWidgets::DockWidgetBase::Options()。
     * @param layoutSaverOptions 可选的布局保存器选项，默认为 KDDockWidgets::DockWidgetBase::LayoutSaverOptions()。
     *
     * 此构造函数没有 parent 参数。DockWidget 在可见时会作为 FloatingWindow 或 MainWindow 的子控件，
     * 在隐藏时则没有父控件。这允许支持停靠到不同的主窗口。
     */
    explicit DockWidget(const QString &uniqueName, Options options = KDDockWidgets::DockWidgetBase::Options(),
                        LayoutSaverOptions layoutSaverOptions = KDDockWidgets::DockWidgetBase::LayoutSaverOptions());

    /**
     * @brief 析构函数。
     */
    ~DockWidget() override;

protected:
    /**
     * @brief Qt 事件处理函数。
     * @param event 事件对象。
     * @return 如果事件被处理，则返回 true；否则返回 false 以继续传递事件。
     */
    bool event(QEvent *event) override;
    /**
     * @brief 处理关闭事件。
     * 当用户尝试关闭此停靠小部件时（例如通过点击标题栏的关闭按钮），此事件会被触发。
     * @param event 关闭事件对象。
     */
    void closeEvent(QCloseEvent *event) override;

private:
    class Private; ///< PIMPL (Private Implementation) 设计模式的私有实现类前向声明。
    Private *const d; ///< 指向私有实现类的常量指针。
};

} // namespace KDDockWidgets


#endif // KDDOCKWIDGETS_QTWIDGETS

#endif // KD_DOCKWIDGET_H
