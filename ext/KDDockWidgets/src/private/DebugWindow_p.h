/**
 * @file
 * @brief 用于显示调试信息的窗口。仅用于调试目的，适用于不支持 GammaRay 的应用程序。
 *
 * @author Sérgio Martins <sergio.martins@kdab.com>
 */
#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#ifdef KDDOCKWIDGETS_QTWIDGETS // 仅当定义了 KDDOCKWIDGETS_QTWIDGETS 宏时才编译以下内容 (表示在 Qt Widgets 环境下)
#include "ObjectViewer_p.h" // 包含 ObjectViewer 私有头文件，DebugWindow 使用它来显示对象信息
#include <QWidget> // 包含 QWidget 类，所有用户界面对象的基类

QT_BEGIN_NAMESPACE // Qt 命名空间开始
    class QEventLoop; // 前向声明 QEventLoop 类，用于事件循环处理
QT_END_NAMESPACE // Qt 命名空间结束


    namespace KDDockWidgets::Debug
{ // KDDockWidgets 库的 Debug 命名空间

    /**
     * @brief DebugWindow 类提供了一个用于显示 KDDockWidgets 内部状态和调试信息的窗口。
     *
     * 这个窗口主要用于开发和调试目的，特别是在无法使用如 GammaRay 这类更高级的 Qt 内省工具时。
     * 它可以显示对象树、部件几何信息，并可能包含一些平台相关的调试功能。
     */
    class DebugWindow : public QWidget
    {
    Q_OBJECT // 启用 Qt 元对象系统特性
        public :
        /**
         * @brief 构造一个 DebugWindow 对象。
         * @param parent 父 QWidget，默认为 nullptr。
         */
        explicit DebugWindow(QWidget *parent = nullptr);

    private:
#ifdef Q_OS_WIN // 仅在 Windows 操作系统下编译以下方法
        /**
         * @brief (仅 Windows) 转储指定 QWidget 的窗口信息。
         *
         * 此方法可能用于获取和显示特定于 Windows 平台的窗口句柄、样式等调试信息。
         * @param widget 指向要转储信息的 QWidget 的指针。
         */
        void dumpWindow(QWidget *);

        /**
         * @brief (仅 Windows) 转储当前应用程序中所有顶层窗口的信息。
         */
        void dumpWindows();
#endif // Q_OS_WIN

        /**
         * @brief 递归地重绘指定的 QWidget 及其所有子部件。
         *
         * 可能用于强制刷新UI以进行调试或可视化检查。
         * @param widget 指向要递归重绘的 QWidget 的指针。
         */
        void repaintWidgetRecursive(QWidget *);

        /**
         * @brief 静态方法，转储 KDDockWidgets 中所有 DockWidget 的信息。
         *
         * 此方法可以输出当前活动的停靠部件的列表、它们的状态、几何信息等。
         */
        static void dumpDockWidgetInfo();

        ObjectViewer m_objectViewer; ///< ObjectViewer 实例，用于在调试窗口中显示 QObject 的属性和层级结构。
        QEventLoop *m_isPickingWidget = nullptr; ///< 事件循环指针，可能用于实现“拾取部件”功能，允许用户点击界面上的某个部件以在 ObjectViewer 中查看其详细信息。当非 nullptr 时，可能表示正处于拾取模式。

    protected:
        /**
         * @brief 重写 QWidget::mousePressEvent()，处理鼠标按下事件。
         *
         * 可能用于实现“拾取部件”功能：当用户点击时，捕获被点击的部件并在 m_objectViewer 中显示其信息。
         * @param event 鼠标按下事件对象。
         */
        void mousePressEvent(QMouseEvent *event) override;
    };
} // namespace KDDockWidgets::Debug


#endif // KDDOCKWIDGETS_QTWIDGETS

#endif // DEBUGWINDOW_H
