#ifndef KD_FLOATING_WINDOWWIDGET_P_H
#define KD_FLOATING_WINDOWWIDGET_P_H

#ifdef KDDOCKWIDGETS_QTWIDGETS // 仅当定义了 KDDOCKWIDGETS_QTWIDGETS 宏时才编译以下内容 (表示在 Qt Widgets 环境下)

#include "../FloatingWindow_p.h" // 包含 FloatingWindow 私有头文件 (FloatingWindowWidget 的基类)

QT_BEGIN_NAMESPACE // Qt 命名空间开始
    class QVBoxLayout; // 前向声明 QVBoxLayout 类，用于垂直布局子部件
QT_END_NAMESPACE // Qt 命名空间结束

    namespace KDDockWidgets
{

    /**
     * @brief FloatingWindowWidget 类是 FloatingWindow 在 Qt Widgets 环境下的具体实现。
     *
     * 它代表一个可以容纳其他停靠部件 (DockWidget) 并独立于主窗口浮动的窗口。
     * 此类继承自 KDDockWidgets::FloatingWindow，并使用标准的 QWidget 技术来构建其用户界面。
     * 它通常包含一个 Frame，该 Frame 又包含一个或多个 DockWidget。
     */
    class DOCKS_EXPORT FloatingWindowWidget : public FloatingWindow
    {
    Q_OBJECT // 启用 Qt 元对象系统特性，如信号和槽
        public :
        /**
         * @brief 构造一个 FloatingWindowWidget 对象。
         *
         * 创建一个新的浮动窗口，其初始几何形状由 suggestedGeometry 建议。
         *
         * @param suggestedGeometry 建议的初始几何区域 (位置和尺寸)。
         * @param parent 指向父 MainWindowBase 对象的指针，默认为 nullptr。浮动窗口通常是顶层窗口，但可能与主窗口逻辑关联。
         * @param flags 控制浮动窗口行为的标志，默认为从全局配置中获取。
         */
        explicit FloatingWindowWidget(QRect suggestedGeometry, MainWindowBase *parent = nullptr,
                                      FloatingWindowFlags flags = FloatingWindowFlag::FromGlobalConfig);

        /**
         * @brief 构造一个 FloatingWindowWidget 对象，并立即承载一个 Frame。
         *
         * 创建一个新的浮动窗口，用于显示指定的 Frame 及其包含的停靠部件。
         *
         * @param frame 要在此浮动窗口中显示的 Frame 对象。
         * @param suggestedGeometry 建议的初始几何区域 (位置和尺寸)。
         * @param parent 指向父 MainWindowBase 对象的指针，默认为 nullptr。
         */
        explicit FloatingWindowWidget(Frame *frame, QRect suggestedGeometry, MainWindowBase *parent = nullptr);

    protected:
        /**
         * @brief 重写 QWidget::paintEvent()，用于自定义浮动窗口的绘制。
         *
         * 可能用于绘制自定义边框、背景或其他视觉元素。
         * @param event 绘制事件对象。
         */
        void paintEvent(QPaintEvent *) override;

        /**
         * @brief 重写 QObject::event()，用于处理更广泛的事件类型。
         * @param ev 事件对象。
         * @return 如果事件被处理，则返回 true；否则返回 false。
         */
        bool event(QEvent *ev) override;

        QVBoxLayout *const m_vlayout; ///< 指向浮动窗口内部垂直布局的常量指针。用于组织窗口内的子部件 (如 Frame)。
        QMetaObject::Connection m_screenChangedConnection; ///< 用于连接屏幕变化信号的连接对象，以便在屏幕属性（如DPI、多显示器配置）改变时做出响应。

    private:
        /**
         * @brief 初始化浮动窗口。
         *
         * 在构造函数中调用，用于设置窗口的基本属性、布局和事件过滤器等。
         */
        void init();

        /**
         * @brief 更新窗口的边距。
         *
         * 根据窗口样式或内容动态调整边距。
         */
        void updateMargins();

        /**
         * @brief 重写 QObject::eventFilter()，用于拦截和处理其他对象的事件。
         *
         * 可能用于监听子部件或相关对象的特定事件，以协调浮动窗口的行为。
         * @param o 被观察的对象。
         * @param ev 发生的事件。
         * @return 如果事件被过滤（即停止进一步处理），则返回 true；否则返回 false。
         */
        bool eventFilter(QObject *o, QEvent *ev) override;

        Q_DISABLE_COPY(FloatingWindowWidget) // 禁止拷贝构造函数和赋值操作符，因为 QWidget 通常不应被拷贝
    };

} // namespace KDDockWidgets

#endif // KDDOCKWIDGETS_QTWIDGETS


#endif // KD_FLOATING_WINDOWWIDGET_P_H
