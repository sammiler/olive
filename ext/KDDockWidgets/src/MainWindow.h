/**
 * @file
 * @brief QMainWindow 的子类，用于启用 KDDockWidgets 支持。
 *
 * @author Sérgio Martins <sergio.martins@kdab.com>
 */

#ifndef KD_MAINWINDOW_H
#define KD_MAINWINDOW_H

#ifdef KDDOCKWIDGETS_QTWIDGETS // 条件编译：仅当为 Qt Widgets 构建时才包含此文件的内容

#include "MainWindowBase.h" // 包含主窗口的基类定义

QT_BEGIN_NAMESPACE
// 前向声明 Qt 类
class QHBoxLayout; // Qt 水平布局类
QT_END_NAMESPACE

namespace KDDockWidgets {

// 前向声明 KDDockWidgets 内部类
class SideBar; // 侧边栏类

/**
 * @brief 应用程序应该使用的 QMainWindow 子类，以便能够停靠 KDDockWidget::DockWidget 实例。
 *
 * MainWindow 继承自 MainWindowBase，提供了 QtWidgets 平台下的具体实现，
 * 包括对侧边栏 (SideBar) 的管理、中央区域的布局以及事件处理等。
 * 用户通常会创建此类或其子类的实例作为应用程序的主窗口。
 */
class DOCKS_EXPORT MainWindow : public MainWindowBase
{
    Q_OBJECT // Q_OBJECT 宏，用于启用 Qt 元对象系统特性，如信号和槽
public:
    /// @brief MainWindow 指针的 QVector 类型定义，方便使用。
    typedef QVector<MainWindow *> List;

    /**
     * @brief 构造函数。像使用 QMainWindow 一样使用它。
     * @param uniqueName 强制性的名称，在所有 MainWindow 实例之间应该是唯一的。
     * 此名称不会对用户可见，仅在内部用于保存/恢复布局。
     * @param options 可选的 MainWindowOptions，用于配置主窗口特性，默认为 MainWindowOption_None。
     * @param parent 传递给 QMainWindow 构造函数的 QWidget* 父对象，默认为 nullptr。
     * @param flags 传递给 QMainWindow 构造函数的窗口标志，默认为 Qt::WindowFlags()。
     */
    explicit MainWindow(const QString &uniqueName, MainWindowOptions options = MainWindowOption_None,
                        QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    /**
     * @brief 析构函数。
     */
    ~MainWindow() override;

    /**
     * @brief (重写) 返回指定位置的侧边栏 (SideBar)。
     * @param location 侧边栏的位置 (例如，North, East, West, South)。
     * @return 指向对应位置 SideBar 对象的指针，如果该位置没有侧边栏则为 nullptr。
     */
    [[nodiscard]] SideBar *sideBar(SideBarLocation location) const override;

    /**
     * @brief (重写) 返回中央小部件区域的内容边距。
     * @return QMargins 对象，表示中央区域与其周围元素的间距。
     */
    [[nodiscard]] QMargins centerWidgetMargins() const override;

    /**
     * @brief 设置中央小部件区域的内容边距。
     * @param margins 要设置的 QMargins 对象。
     */
    void setCenterWidgetMargins(QMargins margins);

    /**
     * @brief 返回内部布局。
     * 除非您想在主停靠小部件布局旁边布置其他小部件，否则很少需要此功能。
     * @return 指向内部 QHBoxLayout 的指针。
     */
    [[nodiscard]] QHBoxLayout *internalLayout() const;

protected:
    /**
     * @brief 处理关闭事件 (重写 QWidget::closeEvent)。
     * @param event 关闭事件对象。
     */
    void closeEvent(QCloseEvent *event) override;
    /**
     * @brief 处理大小调整事件 (重写 QWidget::resizeEvent)。
     * @param event 大小调整事件对象。
     */
    void resizeEvent(QResizeEvent *event) override;
    /**
     * @brief (重写) 获取中央区域的几何形状。
     * @return 中央区域的 QRect。
     */
    [[nodiscard]] QRect centralAreaGeometry() const override;

private:
    // 使用 using 声明将 QMainWindow 的 setCentralWidget 引入当前作用域，以便可以重写它并设为私有。
    using QMainWindow::setCentralWidget;
    /**
     * @brief 设置中央小部件 (重写 QMainWindow::setCentralWidget 并设为私有)。
     * KDDockWidgets 使用自己的布局机制管理中央区域，因此不应直接调用此函数。
     * @param widget 要设置为中央小部件的 QWidget 指针。
     */
    void setCentralWidget(QWidget *widget); // 重写只是为了将其设为私有

    class Private;   ///< PIMPL (Private Implementation) 设计模式的私有实现类前向声明。
    Private *const d; ///< 指向私有实现类的常量指针。
};

} // namespace KDDockWidgets

#endif // KDDOCKWIDGETS_QTWIDGETS

#endif // KD_MAINWINDOW_H
