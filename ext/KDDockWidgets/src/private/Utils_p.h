#ifndef KD_UTILS_P_H
#define KD_UTILS_P_H

#include "Config.h" // KDDockWidgets 配置类
#include "Frame_p.h" // Frame 私有头文件 (停靠小部件的容器)
#include "kddockwidgets/QWidgetAdapter.h" // QWidget 和 QQuickItem 的适配器类

#include <QScreen> // Qt 屏幕信息类
#include <QWindow> // Qt 窗口基类
#include <QMouseEvent> // Qt 鼠标事件类

#ifdef KDDOCKWIDGETS_QTQUICK // 条件编译：如果 KDDockWidgets 是为 QtQuick 构建的
#include "private/quick/TitleBarQuick_p.h" // QtQuick 版本的 TitleBar 私有头文件

#include <QQuickItem> // QtQuick 项目基类
#include <QQuickView> // QtQuick 视图类 (尽管这里可能未使用，但常与 QQuickWindow 一起出现)
#include <QGuiApplication> // Qt GUI 应用程序类 (用于 QtQuick 和通用 GUI 功能)
#else // 否则 (即为 QtWidgets 构建)
#include <QApplication> // Qt Widgets 应用程序类
#include <QAbstractButton> // Qt 抽象按钮基类
#include <QLineEdit> // Qt 单行文本编辑器控件
#endif

#ifdef QT_X11EXTRAS_LIB // 条件编译：如果链接了 QtX11Extras 模块 (通常用于 Linux/X11)
#include <QtX11Extras/QX11Info> // Qt X11 平台特定信息类
#endif

QT_BEGIN_NAMESPACE
// 前向声明 Qt 类
class QWidget; // Qt Widget 基类
class QWindow; // Qt 窗口基类
QT_END_NAMESPACE

namespace KDDockWidgets {

#ifdef KDDOCKWIDGETS_QTQUICK
/**
 * @brief (仅 QtQuick) 根据全局位置递归查找包含该位置的最深层 QQuickMouseArea。
 * @param item 要开始搜索的父 QQuickItem。
 * @param globalPos 全局鼠标坐标。
 * @return 指向找到的 QQuickMouseArea 的指针，如果未找到则返回 nullptr。
 */
inline QQuickItem *mouseAreaForPos(QQuickItem *item, QPointF globalPos);
#endif

/**
 * @brief 检查当前平台是否为 Wayland。
 * @return 如果是 Wayland，则返回 true；否则返回 false。
 */
inline bool isWayland()
{
    return qApp->platformName() == QLatin1String("wayland");
}

/**
 * @brief 检查当前平台是否为 Offscreen (离屏渲染)。
 * @return 如果是 Offscreen，则返回 true；否则返回 false。
 */
inline bool isOffscreen()
{
    return qApp->platformName() == QLatin1String("offscreen");
}

/**
 * @brief 检查当前平台是否为 XCB (X11)。
 * @return 如果是 XCB，则返回 true；否则返回 false。
 */
inline bool isXCB()
{
    return qApp->platformName() == QLatin1String("xcb");
}

/**
 * @brief 检查当前平台是否为 EGLFS (嵌入式 Linux 平台的全屏渲染)。
 * @return 如果是 EGLFS，则返回 true；否则返回 false。
 */
inline bool isEGLFS()
{
    return qApp->platformName() == QLatin1String("eglfs");
}

/**
 * @brief 返回 KDDockWidgets 是为 QtWidgets 还是 QtQuick 构建的。
 * @return 如果为 QtWidgets 构建，则返回 true；否则 (为 QtQuick 构建) 返回 false。
 */
inline bool kddwUsesQtWidgets()
{
#ifdef KDDOCKWIDGETS_QTWIDGETS
    return true;
#else
    return false;
#endif
}

/**
 * @brief 检查鼠标左键当前是否被按下。
 * @return 如果左键被按下，则返回 true；否则返回 false。
 */
inline bool isLeftButtonPressed()
{
    return qApp->mouseButtons() & Qt::LeftButton;
}

/**
 * @brief 检查是否使用原生操作系统标题栏。
 * @return 如果配置为使用原生标题栏，则返回 true。
 */
inline bool usesNativeTitleBar()
{
    return Config::self().flags() & Config::Flag_NativeTitleBar;
}

/**
 * @brief 检查是否使用客户端（即 Qt 绘制的）标题栏。
 * @return 如果使用客户端标题栏，则返回 true。
 */
inline bool usesClientTitleBar()
{
    if (isWayland()) {
        // Wayland 由于其限制，可能同时具有客户端和原生标题栏的概念，
        // 但 KDDW 在 Wayland 上通常会绘制自己的标题栏以获得更好的控制。
        return true;
    }

    // 其他所有平台要么使用操作系统原生标题栏，要么使用 Qt 绘制的标题栏（即客户端标题栏）。
    return !usesNativeTitleBar();
}

/**
 * @brief 检查是否启用了在 Windows 上使用自定义装饰（客户端标题栏）时的 Aero Snap 功能。
 * @return 如果启用了此特性，则返回 true。
 */
inline bool usesAeroSnapWithCustomDecos()
{
    return Config::self().flags() & Config::Flag_AeroSnapWithClientDecos;
}

/**
 * @brief 检查是否使用原生的拖拽和大小调整机制。
 *
 * 原生标题栏意味着原生的调整大小和拖拽。
 * Windows Aero-Snap 也意味着原生拖拽，并且意味着没有原生标题栏。
 * @return 如果使用原生拖拽和大小调整，则返回 true。
 */
inline bool usesNativeDraggingAndResizing()
{
    // 断言：不能同时使用原生标题栏和自定义装饰的 Aero Snap
    Q_ASSERT(!(usesNativeTitleBar() && usesAeroSnapWithCustomDecos()));
    return usesNativeTitleBar() || usesAeroSnapWithCustomDecos();
}

/**
 * @brief 检查 KDDockWidgets 是否链接了 XLib (通常在 Linux/X11 平台上)。
 * @return 如果链接了 XLib，则返回 true。
 */
inline bool linksToXLib()
{
#ifdef KDDockWidgets_XLIB
    return true;
#else
    return false;
#endif
}

/**
 * @brief 检查给定的窗口状态是否为“正常”（非最大化也非全屏）。
 * @param states 要检查的 Qt::WindowStates。
 * @return 如果是正常状态，则返回 true。
 */
inline bool isNormalWindowState(Qt::WindowStates states)
{
    return !(states & Qt::WindowMaximized) && !(states & Qt::WindowFullScreen);
}

/**
 * @brief 检查是否应使用备用的鼠标捕获机制。
 * @return 对于 QtQuick 返回 true，对于 QtWidgets 返回 false。
 */
inline bool usesFallbackMouseGrabber()
{
#ifdef KDDOCKWIDGETS_QTWIDGETS
    // QtWidgets 将使用 QWidget::grabMouse()
    return false;
#else
    // 对于 QtQuick，我们使用全局事件过滤器，因为鼠标传递不稳定。
    // 例如，接收到按下的同一个 QQuickItem 在顶层窗口移动时可能接收不到鼠标移动事件。
    return true;
#endif
}

/**
 * @brief 激活指定的窗口。
 * @param window 指向要激活的 QWindow 的指针。
 */
inline void activateWindow(QWindow *window)
{
    window->requestActivate(); // 请求窗口管理器激活此窗口
}

/**
 * @brief 检查窗口管理器是否支持半透明效果。
 * @return 如果支持半透明且未被禁用，则返回 true。
 */
inline bool windowManagerHasTranslucency()
{
    // 如果通过环境变量或内部配置禁用了半透明，则直接返回 false
    if (qEnvironmentVariableIsSet("KDDW_NO_TRANSLUCENCY") || (Config::self().internalFlags() & Config::InternalFlag_DisableTranslucency))
        return false;

#ifdef QT_X11EXTRAS_LIB // 如果链接了 X11 Extras 模块
    if (isXCB()) // 如果当前平台是 XCB (X11)
        return QX11Info::isCompositingManagerRunning(); // 检查 X11 合成管理器是否正在运行
#endif

    // macOS 和 Windows 通常支持半透明
    return true;
}

/**
 * @brief 获取指定窗口所在屏幕的尺寸。
 * @param window 指向 QWindow 的指针。
 * @return 屏幕的 QSize，如果窗口或屏幕无效则返回空 QSize。
 */
inline QSize screenSizeForWindow(const QWindow *window)
{
    if (window) {
        if (QScreen *screen = window->screen()) { // 获取窗口所在的屏幕
            return screen->size(); // 返回屏幕尺寸
        }
    }
    return {}; // 返回空尺寸
}

/**
 * @brief 获取指定窗口所在屏幕的索引号。
 * @param window 指向 QWindow 的指针。
 * @return 屏幕的索引号（在 qApp->screens() 列表中），如果窗口或屏幕无效则返回 -1。
 */
inline int screenNumberForWindow(const QWindow *window)
{
    if (window) {
        if (QScreen *screen = window->screen()) {
            return qApp->screens().indexOf(screen); // 返回屏幕在应用程序屏幕列表中的索引
        }
    }
    return -1; // 无效则返回 -1
}

/**
 * @brief 将 QEvent 转换为 QMouseEvent（如果类型匹配）。
 * @param e 指向 QEvent 的指针。
 * @return 如果事件是鼠标事件类型，则返回转换后的 QMouseEvent 指针；否则返回 nullptr。
 */
inline QMouseEvent *mouseEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
    case QEvent::NonClientAreaMouseButtonPress: // 非客户区鼠标按下
    case QEvent::NonClientAreaMouseButtonRelease: // 非客户区鼠标释放
    case QEvent::NonClientAreaMouseMove: // 非客户区鼠标移动
    case QEvent::NonClientAreaMouseButtonDblClick: // 非客户区鼠标双击
        return dynamic_cast<QMouseEvent *>(e); // 尝试动态转换为 QMouseEvent
    default:
        break;
    }
    return nullptr; // 非鼠标事件类型
}

/**
 * @brief 检查事件是否为非客户区鼠标事件。
 * @param e 指向 QEvent 的指针。
 * @return 如果是非客户区鼠标事件，则返回 true；否则返回 false。
 */
inline bool isNonClientMouseEvent(const QEvent *e)
{
    switch (e->type()) {
    case QEvent::NonClientAreaMouseButtonPress:
    case QEvent::NonClientAreaMouseButtonRelease:
    case QEvent::NonClientAreaMouseMove:
        return true;
    default:
        break;
    }
    return false;
}

/**
 * @brief 检查给定的 QWindow 指针是否有效（非空）。
 * @param w 指向 QWindow 的指针。
 * @return 如果 w 不为 nullptr，则返回 true。
 */
inline bool isWindow(const QWindow *w)
{
    return w != nullptr;
}

/**
 * @brief 获取开始拖拽操作所需的最小鼠标移动距离。
 * @return 拖拽启动距离（像素）。
 */
inline int startDragDistance()
{
#ifdef KDDOCKWIDGETS_QTWIDGETS
    return QApplication::startDragDistance(); // QtWidgets 使用 QApplication 的设置
#else
    return 4; // QtQuick 可能使用一个固定的较小值
#endif
}

/**
 * @brief 返回指定全局位置处的 QWidget 或 QQuickItem。
 * 基本上是 QApplication::widgetAt()，但增加了对 QtQuick 的支持。
 * @param globalPos 全局屏幕坐标。
 * @return 指向位于该位置的 WidgetType (QWidget* 或 QQuickItem*) 的指针，如果未找到则为 nullptr。
 */
inline WidgetType *mouseReceiverAt(QPoint globalPos)
{
#ifdef KDDOCKWIDGETS_QTWIDGETS
    return qApp->widgetAt(globalPos); // QtWidgets 直接使用 QApplication::widgetAt
#else
    // QtQuick 的实现：先找到顶层 QQuickWindow，然后在其内容项中查找
    auto window = qobject_cast<QQuickWindow *>(QGuiApplication::topLevelAt(globalPos));
    if (!window)
        return nullptr;

    return mouseAreaForPos(window->contentItem(), globalPos); // 调用辅助函数查找 MouseArea
#endif
}

/**
 * @brief 检查全局位置是否位于不允许开始拖拽的控件上（例如按钮）。
 * 并非整个标题栏都可拖拽。例如，关闭按钮不允许从那里开始拖拽。
 * @param globalPos 全局鼠标坐标。
 * @return 如果鼠标位于不应启动拖拽的控件上，则返回 true。
 */
inline bool inDisallowDragWidget(QPoint globalPos)
{
    WidgetType *widget = mouseReceiverAt(globalPos); // 获取鼠标下的控件
    if (!widget)
        return false;

#ifdef KDDOCKWIDGETS_QTWIDGETS
    // 用户可能在工具栏上有一个行编辑器。TODO: 修复不够优雅，应该让用户的标签栏实现某个虚方法...
    // 对于 QtWidgets，检查是否为按钮或行编辑器
    return qobject_cast<QAbstractButton *>(widget) || qobject_cast<QLineEdit *>(widget);
#else
    // 对于 QtQuick，检查对象名称是否不是 "draggableMouseArea" (假设可拖拽区域有特定名称)
    return widget->objectName() != QLatin1String("draggableMouseArea");
#endif
}

#ifdef KDDOCKWIDGETS_QTWIDGETS // 以下函数仅在 QtWidgets 环境下定义
/**
 * @brief (仅 QtWidgets) 获取指定 QWidget 所在屏幕的索引号。
 * @param widget 指向 QWidget 的指针。
 * @return 屏幕索引号，如果无效则返回 -1。
 */
inline int screenNumberForWidget(const QWidget *widget)
{
    return screenNumberForWindow(widget->window()->windowHandle());
}

/**
 * @brief (仅 QtWidgets) 获取指定 QWidget 所在屏幕的尺寸。
 * @param widget 指向 QWidget 的指针。
 * @return 屏幕的 QSize，如果无效则返回空 QSize。
 */
inline QSize screenSizeForWidget(const QWidget *widget)
{
    return screenSizeForWindow(widget->window()->windowHandle());
}

/**
 * @brief (仅 QtWidgets) 将点 @p p 从小部件 @p w 的局部坐标映射到全局坐标。
 * @param w 指向 QWidget 的指针。
 * @param p 局部坐标点。
 * @return 全局坐标点。
 */
inline QPoint mapToGlobal(QWidget *w, QPoint p)
{
    return w->mapToGlobal(p);
}

/**
 * @brief (仅 QtWidgets) 激活指定的小部件所在的窗口。
 * @param widget 指向 QWidget 的指针。
 */
inline void activateWindow(QWidget *widget)
{
    widget->activateWindow();
}

/**
 * @brief (仅 QtWidgets) 检查给定的 QWidget 是否为一个独立的窗口。
 * @param w 指向 QWidget 的指针。
 * @return 如果 w 有效且是一个窗口，则返回 true。
 */
inline bool isWindow(const QWidget *w)
{
    return w && w->isWindow();
}

#else // 以下函数在 QtQuick 环境下定义

/**
 * @brief (仅 QtQuick) 获取指定 QQuickItem 所在屏幕的索引号。
 * @param w 指向 QQuickItem 的指针。
 * @return 屏幕索引号，如果无效则返回 -1。
 */
inline int screenNumberForWidget(const QQuickItem *w)
{
    return screenNumberForWindow(w->window());
}

/**
 * @brief (仅 QtQuick) 获取指定 QQuickItem 所在屏幕的尺寸。
 * @param w 指向 QQuickItem 的指针。
 * @return 屏幕的 QSize，如果无效则返回空 QSize。
 */
inline QSize screenSizeForWidget(const QQuickItem *w)
{
    return screenSizeForWindow(w->window());
}

/**
 * @brief (仅 QtQuick) 将点 @p p 从 QQuickItem @p item 的局部坐标映射到全局坐标。
 * @param item 指向 QQuickItem 的指针。
 * @param p 局部坐标点。
 * @return 全局坐标点。
 */
inline QPoint mapToGlobal(QQuickItem *item, QPoint p)
{
    Q_ASSERT(item); // 断言 item 不为空
    return item->mapToGlobal(p).toPoint(); // QPointF 转 QPoint
}

/**
 * @brief (仅 QtQuick) 递归查找在 QQuickItem @p item 的子项中，包含全局点 @p globalPos 的最深层 QQuickMouseArea。
 * @param item 要开始搜索的父 QQuickItem。
 * @param globalPos 全局坐标点。
 * @return 指向找到的 QQuickMouseArea 的指针，如果未找到则返回 nullptr。
 */
inline QQuickItem *mouseAreaForPos(QQuickItem *item, QPointF globalPos)
{
    QRectF rect = item->boundingRect(); // 获取项的边界矩形
    rect.moveTopLeft(item->mapToGlobal(QPointF(0, 0))); // 将边界矩形移动到全局坐标

    // 假设子项都在其父项内部。这对于 KDDW 的目的来说是可行的。
    if (!rect.contains(globalPos)) { // 如果全局点不在当前项的全局边界内，则直接返回
        return nullptr;
    }

    const QList<QQuickItem *> children = item->childItems(); // 获取所有子项

    // 从后向前遍历子项（通常后添加的在顶层）
    for (auto it = children.rbegin(), end = children.rend(); it != end; ++it) {
        if (QQuickItem *receiver = mouseAreaForPos(*it, globalPos)) // 递归查找
            return receiver; // 如果在子项中找到，则返回
    }

    // 如果在子项中未找到，检查当前项本身是否为 QQuickMouseArea
    if (QLatin1String(item->metaObject()->className()) == QLatin1String("QQuickMouseArea"))
        return item;

    return nullptr; // 未找到
}

/**
 * @brief (仅 QtQuick) 获取 QQuickItem 的全局几何形状。
 * @param item 指向 QQuickItem 的指针。
 * @return 全局坐标系下的 QRect。
 */
inline QRect globalGeometry(QQuickItem *item)
{
    QRect geo(QPoint(0, 0), item->size().toSize()); // 获取局部几何形状
    geo.moveTopLeft(item->mapToGlobal(QPointF(0, 0)).toPoint()); // 映射到全局坐标
    return geo;
}

#endif // KDDOCKWIDGETS_QTWIDGETS


/**
 * @brief 返回小部件的几何形状，但始终以全局坐标系表示。
 * @param w 指向 QWidgetOrQuick 的指针。
 * @return 全局坐标系下的 QRect。
 */
inline QRect globalGeometry(QWidgetOrQuick *w)
{
    QRect geo = w->geometry(); // 获取小部件的几何形状
    if (!w->isWindow()) // 如果不是顶层窗口
        geo.moveTopLeft(w->mapToGlobal(QPoint(0, 0))); // 将其左上角映射到全局坐标
    return geo;
}

/**
 * @brief 返回是否支持指定的缩放因子。
 * 这是针对旧版本 Qt (QTBUG-86170) 中一个 BUG 的解决方法。
 * 主要影响 Linux。除非您使用 Qt::HighDpiScaleFactorRoundingPolicy::PassThrough，
 * 否则它也会影响其他操作系统。
 * @param factor 要检查的缩放因子。
 * @return 如果支持该缩放因子，则返回 true。
 */
inline bool scalingFactorIsSupported(qreal factor)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 2) // 如果 Qt 版本低于 5.15.2
    // 在旧版 Qt 中我们不支持小数缩放因子。
    const bool isInteger = int(factor) == factor; // 检查因子是否为整数
    return isInteger;
#else
    Q_UNUSED(factor); // 标记参数未使用
    return true; // 新版本 Qt 假定支持
#endif
}

/**
 * @brief 模板函数，查找指定 QObject 的第一个类型为 T 的父对象。
 * @tparam T 要查找的父对象类型。
 * @param child 指向子 QObject 的指针。
 * @return 指向找到的类型为 T 的父对象的指针，如果未找到或跨越窗口边界则返回 nullptr。
 */
template<typename T>
inline T *firstParentOfType(const QObject *child)
{
    auto p = const_cast<QObject *>(child); // 移除 const 限定，以便修改 p
    while (p) {
        if (auto candidate = qobject_cast<T *>(p)) // 尝试将当前父对象转换为类型 T
            return candidate; // 如果成功，则返回

        if (qobject_cast<const QWindow *>(p)) {
            // 忽略跨越多个窗口的 QObject 层级结构
            return nullptr;
        }

#ifdef KDDOCKWIDGETS_QTWIDGETS
        // 忽略跨越多个窗口的 QObject 层级结构 (QtWidgets 特定检查)
        if (auto w = qobject_cast<QWidget *>(p))
            if (w->isWindow())
                return nullptr;
#endif

        p = p->parent(); // 继续向上查找父对象
    }

    return nullptr; // 未找到
}

/**
 * @brief 模板函数，查找指定 QObject 的最后一个（最顶层的）类型为 T 的父对象。
 * @tparam T 要查找的父对象类型。
 * @param child 指向子 QObject 的指针。
 * @return 指向找到的类型为 T 的父对象的指针，如果未找到则返回 nullptr。
 */
template<typename T>
inline T *lastParentOfType(const QObject *child)
{
    auto p = const_cast<QObject *>(child);
    T *result = nullptr; // 用于存储最后找到的匹配项
    while (p) {
        if (auto candidate = qobject_cast<T *>(p))
            result = candidate; // 更新最后找到的匹配项

        if (qobject_cast<const QWindow *>(p)) {
            // 忽略跨越多个窗口的 QObject 层级结构
            return result; // 返回当前找到的最后一个匹配项
        }

#ifdef KDDOCKWIDGETS_QTWIDGETS
        // 忽略跨越多个窗口的 QObject 层级结构 (QtWidgets 特定检查)
        if (auto w = qobject_cast<QWidget *>(p))
            if (w->isWindow())
                return result; // 返回当前找到的最后一个匹配项
#endif

        p = p->parent(); // 继续向上查找父对象
    }

    return result; // 返回最后找到的匹配项（如果存在）
}

} // namespace KDDockWidgets

#endif // KD_UTILS_P_H
