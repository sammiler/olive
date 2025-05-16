/**
 * @file
 * @brief 定义一个类，该类在为 QtWidgets 构建时是 QWidget，在为 QtQuick 构建时是 QObject。
 *
 * 允许使用相同的代码库支持两种不同的 UI 技术栈。
 *
 * @author Sérgio Martins <sergio.martins@kdab.com>
 */

#ifndef KDDOCKWIDGETS_QWIDGETADAPTERWIDGETS_P_H
#define KDDOCKWIDGETS_QWIDGETADAPTERWIDGETS_P_H

#ifdef KDDOCKWIDGETS_QTWIDGETS // 仅当定义了 KDDOCKWIDGETS_QTWIDGETS 宏时才编译以下内容 (表示在 Qt Widgets 环境下)

#include "kddockwidgets/docks_export.h" // 包含导出宏定义

#include <QWindow> // 包含 QWindow 类，用于窗口系统集成
#include <QWidget> // 包含 QWidget 类，所有用户界面对象的基类

namespace KDDockWidgets {
namespace Private { // KDDockWidgets 内部私有辅助功能命名空间

/**
 * @brief 辅助函数，获取给定 QWidget 的父 QWidget。
 *
 * 由于 QQuickItem::parentItem() 与 QWidget::parentWidget() 名称不同，此函数提供了一致的访问方式。
 * @param widget 指向目标 QWidget 的指针。
 * @return 如果 widget 非空且有父 widget，则返回其父 QWidget 的指针；否则返回 nullptr。
 */
inline QWidget *parentWidget(QWidget *widget)
{
    return widget ? widget->parentWidget() : nullptr;
}

/**
 * @brief 检查给定的 QWindow 是否处于最小化状态。
 * @param window 指向要检查的 QWindow 的指针。
 * @return 如果窗口已最小化，则返回 true；否则返回 false。
 */
bool isMinimized(QWindow *window);

/**
 * @brief 检查给定的 QWidget 是否处于最小化状态。
 *
 * 它通过获取 QWidget 关联的 QWindow 来进行判断。
 * @param widget 指向要检查的 QWidget 的指针。
 * @return 如果部件已最小化，则返回 true；否则返回 false。
 */
inline bool isMinimized(const QWidget *widget)
{
    QWindow *window = widget ? widget->window()->windowHandle() : nullptr;
    return KDDockWidgets::Private::isMinimized(window);
}

/**
 * @brief 安全地获取 QWidget 的几何区域。
 * @param w 指向目标 QWidget 的指针。
 * @return 如果 w 非空，则返回其几何区域；否则返回一个空的 QRect。
 */
inline QRect geometry(const QWidget *w)
{
    return w ? w->geometry() : QRect();
}

/**
 * @brief 安全地获取 QWidget 父部件的几何区域。
 * @param w 指向目标 QWidget 的指针。
 * @return 如果 w 及其父部件都有效，则返回父部件的几何区域；否则返回一个空的 QRect。
 */
inline QRect parentGeometry(const QWidget *w)
{
    if (!w || !w->parentWidget())
        return {}; // 返回空 QRect

    return w->parentWidget()->geometry();
}

/**
 * @brief 获取 QWidget 关联的 QWindow 句柄。
 * @param w 指向目标 QWidget 的指针。
 * @return 如果 w 非空且其窗口有效，则返回 QWindow 指针；否则返回 nullptr。
 */
inline QWindow *windowForWidget(const QWidget *w)
{
    return w ? w->window()->windowHandle() : nullptr;
}

/**
 * @brief 根据 QWindow 句柄查找关联的 QWidget。
 * @param window 指向 QWindow 的指针。
 * @return 如果找到关联的 QWidget，则返回其指针；否则返回 nullptr。
 */
DOCKS_EXPORT QWidget *widgetForWindow(QWindow *window);

/**
 * @brief 设置包含指定 QWidget 的顶层窗口的几何区域。
 * @param geometry 要设置的新几何区域。
 * @param widget 指向目标 QWidget 的指针。如果为 nullptr，则函数不执行任何操作。
 */
inline void setTopLevelGeometry(QRect geometry, const QWidget *widget)
{
    if (!widget)
        return;

    if (QWidget *topLevel = widget->window()) // 获取顶层窗口
        topLevel->setGeometry(geometry); // 设置其几何区域
}

} // namespace Private

class FloatingWindow; // 前向声明 FloatingWindow 类

/**
 * @brief QWidgetAdapter 类是一个 QWidget 子类，作为 KDDockWidgets 框架中与平台无关的部件表示的 Qt Widgets 实现。
 *
 * 它提供了一组通用的接口和事件处理机制，使得 KDDockWidgets 的核心逻辑可以统一处理
 * 不同 UI 技术栈（如 Qt Widgets 和 Qt Quick）中的部件。
 * 这个类主要用于封装和扩展 QWidget 的行为，以适应停靠框架的需求。
 */
class DOCKS_EXPORT QWidgetAdapter : public QWidget
{
Q_OBJECT // 启用 Qt 元对象系统特性
    public :
    /**
     * @brief 构造一个 QWidgetAdapter 对象。
     * @param parent 父 QWidget，默认为 nullptr。
     * @param f 窗口标志，默认为空 (Qt::WindowFlags())。
     */
    explicit QWidgetAdapter(QWidget *parent = nullptr, Qt::WindowFlags f = {});

    /**
     * @brief 析构函数。
     */
    ~QWidgetAdapter() override;

    /**
     * @brief 返回此部件所在的 FloatingWindow (浮动窗口)。
     * @return 如果此部件位于一个 FloatingWindow 中，则返回其指针；否则返回 nullptr。
     */
    [[nodiscard]] FloatingWindow *floatingWindow() const;

    /**
     * @brief 设置或清除指定的窗口类型标志。
     * @param type 要设置或清除的 Qt::WindowType 标志。
     * @param on 如果为 true，则设置标志；如果为 false，则清除标志。默认为 true。
     */
    void setFlag(Qt::WindowType, bool on = true);

    /**
     * @brief 设置此适配器部件的大小。
     * @param size 新的尺寸。
     */
    void setSize(QSize);

    /**
     * @brief 检查此部件是否为顶层窗口。
     * @return 如果此部件是一个窗口 (isWindow() 返回 true)，则返回 true。
     */
    [[nodiscard]] bool isTopLevel() const
    {
        return isWindow(); // 调用 QWidget::isWindow()
    }

Q_SIGNALS:
    /**
     * @brief 当部件的几何区域发生改变时发射此信号。
     */
    void widgetGeometryChanged();

protected:
    /**
     * @brief 提升窗口层级并激活窗口。
     */
    void raiseAndActivate();

    /**
     * @brief 重写 QObject::event()，用于通用事件处理。
     * @param e 事件对象。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    bool event(QEvent *e) override;

    /**
     * @brief 重写 QWidget::resizeEvent()，处理尺寸调整事件。
     * @param event 尺寸调整事件对象。
     */
    void resizeEvent(QResizeEvent *) override;

    /**
     * @brief 重写 QWidget::moveEvent()，处理移动事件。
     * @param event 移动事件对象。
     */
    void moveEvent(QMoveEvent *) override;

    /**
     * @brief 重写 QWidget::mousePressEvent()，处理鼠标按下事件。
     * @param event 鼠标按下事件对象。
     */
    void mousePressEvent(QMouseEvent *) override;

    /**
     * @brief 重写 QWidget::mouseMoveEvent()，处理鼠标移动事件。
     * @param event 鼠标移动事件对象。
     */
    void mouseMoveEvent(QMouseEvent *) override;

    /**
     * @brief 重写 QWidget::mouseReleaseEvent()，处理鼠标释放事件。
     * @param event 鼠标释放事件对象。
     */
    void mouseReleaseEvent(QMouseEvent *) override;

    /**
     * @brief 重写 QWidget::closeEvent()，处理关闭事件。
     * @param event 关闭事件对象。
     */
    void closeEvent(QCloseEvent *) override;

    /**
     * @brief 虚函数，设置部件的“正常”几何区域。
     *
     * "正常"几何区域通常指部件在非最大化或最小化状态下的几何区域。
     * @param rect 要设置的正常几何区域。
     */
    virtual void setNormalGeometry(QRect);

    /**
     * @brief 虚函数，当部件大小调整时由 resizeEvent 调用。
     *
     * 派生类可以重写此方法以执行自定义的尺寸调整逻辑。
     * @param newSize 新的尺寸。
     * @return 如果事件被处理，则返回 true；否则返回 false。
     */
    virtual bool onResize(QSize newSize);

    /**
     * @brief 虚函数，当需要重新布局时调用。
     */
    virtual void onLayoutRequest();

    /**
     * @brief 虚函数，当发生鼠标按下事件时由 mousePressEvent 调用。
     */
    virtual void onMousePress();

    /**
     * @brief 虚函数，当发生鼠标移动事件时由 mouseMoveEvent 调用。
     * @param globalPos 全局鼠标坐标。
     */
    virtual void onMouseMove(QPoint globalPos);

    /**
     * @brief 虚函数，当发生鼠标释放事件时由 mouseReleaseEvent 调用。
     */
    virtual void onMouseRelease();

    /**
     * @brief 虚函数，当发生关闭事件时由 closeEvent 调用。
     * @param event 关闭事件对象。
     */
    virtual void onCloseEvent(QCloseEvent *);
};

/**
 * @brief 获取给定 QWidget 的逻辑 DPI 缩放因子。
 *
 * 在 macOS 上，此函数总是返回 1，因为 macOS 通常自己处理 DPI 缩放。
 * 在其他平台上，它基于 widget->logicalDpiX() / 96.0 计算。
 * @param w 指向目标 QWidget 的指针。
 * @return 逻辑 DPI 缩放因子。
 */
inline qreal logicalDpiFactor(const QWidget *w)
{
#ifdef Q_OS_MACOS
    // 在 mac 上总是 72 DPI
    Q_UNUSED(w); // 标记 w 为未使用，避免编译器警告
    return 1;
#else
    return w->logicalDpiX() / 96.0; // 计算逻辑 DPI 因子
#endif
}
} // namespace KDDockWidgets

#endif // KDDOCKWIDGETS_QTWIDGETS


#endif // KDDOCKWIDGETS_QWIDGETADAPTERWIDGETS_P_H
