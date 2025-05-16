#ifndef KD_WIDGET_RESIZE_HANDLER_P_H
#define KD_WIDGET_RESIZE_HANDLER_P_H

#include "kddockwidgets/KDDockWidgets.h" // KDDockWidgets 公共头文件，包含枚举和基本类型
#include "kddockwidgets/QWidgetAdapter.h" // QWidget 和 QQuickItem 的适配器类
#include "kddockwidgets/Qt5Qt6Compat_p.h" // Qt5/Qt6 兼容性私有头文件
#include "kddockwidgets/docks_export.h" // 导入导出宏定义
#include "kddockwidgets/private/FloatingWindow_p.h" // FloatingWindow 私有头文件 (WidgetResizeHandler 可能直接操作浮动窗口)

#include <QPoint> // Qt 点坐标类
#include <QPointer> // Qt QObject 指针的弱引用
#include <QDebug> // Qt 调试输出类
#include <QAbstractNativeEventFilter> // Qt 抽象原生事件过滤器基类

QT_BEGIN_NAMESPACE
// 前向声明 Qt 类
class QMouseEvent; // Qt 鼠标事件类
QT_END_NAMESPACE

namespace KDDockWidgets {

// 前向声明
class FloatingWindow; // 浮动窗口类


/**
 * @brief WidgetResizeHandler 类负责处理通过鼠标拖拽小部件边缘来调整其大小的逻辑。
 *
 * 它可以用于顶层浮动窗口，也可以用于嵌入式窗口（如 MDI 子窗口或覆盖层显示的侧边栏）。
 * 它通过事件过滤器拦截鼠标事件，并根据鼠标位置和状态改变目标小部件的几何形状。
 */
class DOCKS_EXPORT WidgetResizeHandler : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 定义了可以由 WidgetResizeHandler 处理或利用的原生窗口特性。
     */
    enum Feature {
        Feature_None = 0, ///< 无特性。
        Feature_NativeShadow = 1, ///< 利用原生窗口阴影（如果平台支持）。
        Feature_NativeResize = 2, ///< 利用原生窗口大小调整机制（例如，Windows Aero Snap 的边缘吸附调整）。
        Feature_NativeDrag = 4, ///< 利用原生窗口拖拽机制（例如，通过标题栏区域）。
        Feature_NativeMaximize = 8, ///< 利用原生窗口最大化机制。
        Feature_All = Feature_NativeShadow | Feature_NativeResize | Feature_NativeDrag | Feature_NativeMaximize ///< 所有原生特性。
    };
    Q_DECLARE_FLAGS(Features, Feature) ///< 将 Feature 枚举声明为可用于 QFlags 的类型。

    /**
     * @brief 结构体，用于描述和配置要使用的原生窗口特性。
     */
    struct NativeFeatures
    {
        /**
         * @brief 默认构造函数。
         */
        NativeFeatures() = default;

        /**
         * @brief 构造函数，使用一个矩形初始化 htCaptionRect。
         * @param r 用于 htCaptionRect 的矩形（全局坐标）。
         */
        explicit NativeFeatures(QRect r)
            : htCaptionRect(r)
        {
        }

        /**
         * @brief 构造函数，使用单个 Feature 初始化。
         * @param f 要启用的特性。
         */
        explicit NativeFeatures(Feature f)
            : features(f)
        {
        }

        /**
         * @brief 构造函数，使用 Features (QFlags) 初始化。
         * @param f 要启用的特性集合。
         */
        explicit NativeFeatures(Features f)
            : features(f)
        {
        }

        QRect htCaptionRect; ///< 可作为标题栏拖拽的区域（全局坐标）。
        Features features = Feature_All; ///< 启用的原生特性集合，默认为全部启用。

        /**
         * @brief 检查是否启用了任何原生特性。
         * @return 如果 features 不是 Feature_None，则返回 true。
         */
        [[nodiscard]] bool hasFeatures() const
        {
            return features != Feature_None;
        }

        /**
         * @brief 检查是否启用了原生阴影特性。
         * @return 如果启用了 Feature_NativeShadow，则返回 true。
         */
        [[nodiscard]] bool hasShadow() const
        {
            return features & Feature_NativeShadow;
        }

        /**
         * @brief 检查是否启用了原生最大化特性。
         * @return 如果启用了 Feature_NativeMaximize，则返回 true。
         */
        [[nodiscard]] bool hasMaximize() const
        {
            return features & Feature_NativeMaximize;
        }

        /**
         * @brief 检查是否启用了原生调整大小特性。
         * @return 如果启用了 Feature_NativeResize，则返回 true。
         */
        [[nodiscard]] bool hasResize() const
        {
            return features & Feature_NativeResize;
        }

        /**
         * @brief 检查是否启用了原生拖拽特性（通过 htCaptionRect 定义的区域）。
         * @return 如果启用了 Feature_NativeDrag 且 htCaptionRect 非空，则返回 true。
         */
        [[nodiscard]] bool hasDrag() const
        {
            return (features & Feature_NativeDrag) && !htCaptionRect.isNull();
        }
    };

    /**
     * @brief 定义事件过滤器的模式。
     */
    enum class EventFilterMode {
        Local = 1, ///< 事件过滤器仅安装在正在调整大小的小部件上，这是浮动窗口的默认模式。
        Global = 2 ///< 事件过滤器是应用程序范围的，例如用于嵌入式 MDI 窗口。
    };

    /**
     * @brief 定义窗口的模式，影响调整大小的行为。
     */
    enum class WindowMode {
        TopLevel = 1, ///< 用于调整顶层窗口（如 FloatingWindow）的大小。
        MDI = 2 ///< 用于调整 MDI (多文档界面) "窗口"（通常是 Frame）的大小。
    };

    /**
     * @brief 构造函数。
     * @param eventFilterMode 事件过滤器的模式（局部或全局）。
     * @param windowMode 窗口模式（顶层或 MDI）。
     * @param target 将要被调整大小的目标小部件。它也作为此 QObject 的父对象。
     */
    explicit WidgetResizeHandler(EventFilterMode eventFilterMode, WindowMode windowMode, QWidgetOrQuick *target);
    /**
     * @brief 析构函数。
     */
    ~WidgetResizeHandler() override;

    /**
     * @brief 设置用户可以通过鼠标调整大小的边。
     *
     * 默认情况下，用户可以调整所有四个边。
     * 然而，当一个停靠小部件被覆盖显示（弹出）时，可能只允许调整一个边。
     * @param sides 允许调整大小的边的组合 (CursorPositions)。
     */
    void setAllowedResizeSides(CursorPositions sides);


    /**
     * @brief 设置调整大小的间隙，默认为 10 像素。
     *
     * 这仅用于非顶层（子）小部件。
     * 当调整子小部件的大小时，它会被其父小部件裁剪，但我们会留出一点空间，
     * 以便可以再次调整它的大小。
     *
     * 意思是，如果您正在调整子小部件的“底部”，它永远不能大于 parent.geometry().bottom() - gap。
     * 这个间隙允许您将鼠标放在那里并再次调整大小。
     * @param gap 间隙大小（像素）。
     */
    void setResizeGap(int gap);

    /**
     * @brief 检查此调整处理器是否用于 MDI 模式的窗口。
     * @return 如果是 MDI 模式，则返回 true。
     */
    [[nodiscard]] bool isMDI() const;

    /**
     * @brief 检查当前是否正在进行调整大小的操作。
     * @return 如果正在调整大小，则返回 true；否则返回 false。
     */
    [[nodiscard]] bool isResizing() const;

    /**
     * @brief (静态方法) 获取小部件调整大小处理器使用的边距。
     *
     * 这个边距定义了靠近小部件边缘多大范围内可以触发调整大小操作。
     * @return 边距大小（像素）。
     */
    static int widgetResizeHandlerMargin();

    /**
     * @brief (静态方法) 为指定的 QWindow 设置一些平台相关的属性，以启用自定义边框处理。
     * @param window 指向要设置的 QWindow。
     */
    static void setupWindow(QWindow *window);
#ifdef Q_OS_WIN // 以下部分仅在 Windows 操作系统下编译
    /**
     * @brief (静态方法，仅 Windows) 检查给定的原生事件类型是否是 WidgetResizeHandler 感兴趣的。
     * @param messageType Windows 消息类型 (例如 WM_NCHITTEST)。
     * @return 如果是感兴趣的事件，则返回 true。
     */
    static bool isInterestingNativeEvent(unsigned int messageType);
    /**
     * @brief (静态方法，仅 Windows) 处理 Windows 原生事件，用于实现自定义边框行为（如 Aero Snap）。
     * @param w 指向目标 QWindow。
     * @param msg 指向 Windows MSG 结构体的指针。
     * @param result 用于存储事件处理结果的指针。
     * @param features 描述要启用的原生特性的 NativeFeatures 对象。
     * @return 如果事件被处理，则返回 true。
     */
    static bool handleWindowsNativeEvent(QWindow *w, MSG *msg, Qt5Qt6Compat::qintptr *result, const NativeFeatures &features);
    /**
     * @brief (静态方法，仅 Windows) 处理 FloatingWindow 的 Windows 原生事件。
     * 这是 handleWindowsNativeEvent 的一个特定重载，用于 FloatingWindow。
     * @param w 指向 FloatingWindow 的指针。
     * @param eventType 原生事件的类型。
     * @param message 原生事件消息。
     * @param result 事件处理结果。
     * @return 如果事件被处理，则返回 true。
     */
    static bool handleWindowsNativeEvent(FloatingWindow *w, const QByteArray &eventType,
                                         void *message, Qt5Qt6Compat::qintptr *result);
#endif // Q_OS_WIN
    /// @brief 静态布尔值，用于全局禁用所有 WidgetResizeHandler 实例。
    static bool s_disableAllHandlers;

protected:
    /**
     * @brief 事件过滤器，用于拦截和处理目标小部件或全局鼠标事件。
     * @param o 被监视的对象。
     * @param e 发生的事件。
     * @return 如果事件被处理，则返回 true；否则返回 false 以继续传递事件。
     */
    bool eventFilter(QObject *o, QEvent *e) override;

private:
    /**
     * @brief 设置要调整大小的目标小部件。
     * @param w 指向目标 QWidgetOrQuick 的指针。
     */
    void setTarget(QWidgetOrQuick *w);
    /**
     * @brief 处理鼠标移动事件。
     * @param e 鼠标事件对象。
     * @return 如果事件被处理，则返回 true。
     */
    bool mouseMoveEvent(QMouseEvent *e);
    /**
     * @brief 根据当前的鼠标位置更新光标形状。
     * @param m 描述鼠标在哪个边缘或角落的 CursorPosition。
     */
    void updateCursor(CursorPosition m);
    /**
     * @brief 设置鼠标光标的形状。
     * @param cursor 要设置的 Qt::CursorShape。
     */
    void setMouseCursor(Qt::CursorShape cursor);
    /**
     * @brief 恢复鼠标光标到其默认形状。
     */
    void restoreMouseCursor();
    /**
     * @brief 根据给定的点确定光标应处于哪个调整大小的位置（例如，左边缘、右下角等）。
     * @param localPos 相对于目标小部件的局部坐标点。
     * @return CursorPosition 枚举值。
     */
    [[nodiscard]] CursorPosition cursorPosition(QPoint localPos) const;

    QWidgetOrQuick *mTarget = nullptr; ///< 指向正在被调整大小的目标小部件。
    CursorPosition mCursorPos = CursorPosition_Undefined; ///< 当前鼠标位置对应的调整大小区域。
    QPoint mNewPosition; ///< 在调整大小过程中，用于存储新的位置或尺寸的临时点。
    bool m_resizingInProgress = false; ///< 标记当前是否正在进行调整大小操作。
    const bool m_usesGlobalEventFilter; ///< 标记此处理器是否使用全局事件过滤器。
    const bool m_isTopLevelWindowResizer; ///< 标记此处理器是否用于顶层窗口。
    int m_resizeGap = 10; ///< 调整子小部件大小时的边缘间隙。
    CursorPositions mAllowedResizeSides = CursorPosition_All; ///< 允许用户通过鼠标调整大小的边。
};

#if defined(Q_OS_WIN) && defined(KDDOCKWIDGETS_QTWIDGETS) // 以下部分仅在 Windows QWidget 环境下编译

/**
 * @brief 辅助类，用于将子小部件的 WM_NCHITTEST 消息重定向到顶层小部件。
 *
 * 为了实现 Aero-snap，顶层窗口必须响应 WM_NCHITTEST，这在 FloatingWindow::nativeEvent() 中完成。
 * 但是，如果子小部件具有原生句柄，则 WM_NCHITTEST 将发送给它们。它们必须响应 HTTRANSPARENT，
 * 以便事件被重定向。
 *
 * 这仅影响 QtWidgets，因为 QQuickItem 从不具有原生窗口 ID。
 */
class NCHITTESTEventFilter : public QAbstractNativeEventFilter
{
public:
    /**
     * @brief 构造函数。
     * @param fw 指向关联的 FloatingWindow。
     */
    explicit NCHITTESTEventFilter(FloatingWindow *fw)
        : m_floatingWindow(fw)
    {
    }
    /**
     * @brief 实现 QAbstractNativeEventFilter 的接口，过滤原生事件。
     * @param eventType 原生事件的类型。
     * @param message 原生事件消息。
     * @param result 用于存储事件处理结果的指针。
     * @return 如果事件被处理，则返回 true。
     */
    bool nativeEventFilter(const QByteArray &eventType, void *message,
                           Qt5Qt6Compat::qintptr *result) override;

    QPointer<FloatingWindow> m_floatingWindow; ///< 指向关联的 FloatingWindow 的弱指针。
};

#endif // Q_OS_WIN && KDDOCKWIDGETS_QTWIDGETS

/**
 * @brief 辅助类，用于应用自定义窗口框架，并处理相关的原生事件。
 *
 * 它允许通过一个回调函数来决定是否以及如何为一个 QWindow 应用自定义框架特性，
 * 并通过安装原生事件过滤器来处理与自定义框架相关的窗口消息（例如在 Windows 上的 WM_NCCALCSIZE）。
 */
class DOCKS_EXPORT CustomFrameHelper
    : public QObject,
      public QAbstractNativeEventFilter // 继承自 QAbstractNativeEventFilter 以处理原生窗口事件
{
    Q_OBJECT
public:
    /**
     * @brief 函数指针类型定义，用于决定是否应为给定的 QWindow 使用自定义框架。
     * @param window 指向 QWindow 的指针。
     * @return 一个 NativeFeatures 结构体，描述了应如何应用自定义框架。
     */
    typedef WidgetResizeHandler::NativeFeatures (*ShouldUseCustomFrame)(QWindow *window);

    /**
     * @brief 构造函数。
     * @param shouldUseCustomFrameFunc 一个函数指针，用于判断是否及如何应用自定义框架。
     * @param parent 父 QObject 对象，默认为 nullptr。
     */
    explicit CustomFrameHelper(ShouldUseCustomFrame shouldUseCustomFrameFunc,
                               QObject *parent = nullptr);
    /**
     * @brief 析构函数。
     */
    ~CustomFrameHelper() override;

public Q_SLOTS:
    /**
     * @brief (静态槽函数) 应用自定义框架到指定的窗口。
     *
     * 此函数通常在窗口创建或需要更新框架样式时调用。
     * @param window 指向要应用自定义框架的 QWindow。
     */
    static void applyCustomFrame(QWindow *window);

protected:
    /**
     * @brief 实现 QAbstractNativeEventFilter 的接口，过滤原生事件。
     *
     * 用于拦截和处理与自定义窗口框架相关的原生窗口消息。
     * @param eventType 原生事件的类型。
     * @param message 原生事件消息。
     * @param result 用于存储事件处理结果的指针。
     * @return 如果事件被处理，则返回 true。
     */
    bool nativeEventFilter(const QByteArray &eventType, void *message,
                           Qt5Qt6Compat::qintptr *result) override;

private:
    bool m_inDtor = false; ///< 标记是否正在析构过程中，防止重入。
    ShouldUseCustomFrame m_shouldUseCustomFrameFunc = nullptr; ///< 指向判断是否使用自定义框架的回调函数的指针。
    bool m_recursionGuard = false; ///< 用于防止在事件处理中发生无限递归的保护标志。
};
} // namespace KDDockWidgets

#endif // KD_WIDGET_RESIZE_HANDLER_P_H
