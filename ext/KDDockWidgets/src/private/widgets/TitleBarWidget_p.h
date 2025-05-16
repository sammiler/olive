#ifndef KD_TITLEBARWIDGET_P_H
#define KD_TITLEBARWIDGET_P_H

#ifdef KDDOCKWIDGETS_QTWIDGETS // 仅当定义了 KDDOCKWIDGETS_QTWIDGETS 宏时才编译以下内容 (表示在 Qt Widgets 环境下)

#include "kddockwidgets/docks_export.h" // 包含导出宏定义
#include "kddockwidgets/private/TitleBar_p.h" // 包含 TitleBar 私有头文件 (TitleBarWidget 的基类)

#include <QPainter> // 包含 QPainter 类，用于绘图
#include <QToolButton> // 包含 QToolButton 类，用作标题栏按钮的基类
#include <QStyle> // 包含 QStyle 类，用于访问样式信息
#include <QWidget> // 包含 QWidget 类，所有用户界面对象的基类
#include <QVector> // 包含 QVector 类，动态数组
#include <QStyleOptionToolButton> // 包含 QStyleOptionToolButton 类，用于自定义工具按钮的绘制

QT_BEGIN_NAMESPACE // Qt 命名空间开始
    class QHBoxLayout; // 前向声明 QHBoxLayout 类，用于水平布局
class QLabel; // 前向声明 QLabel 类，用于显示文本或图像
QT_END_NAMESPACE // Qt 命名空间结束

    namespace KDDockWidgets
{

    class DockWidget; // 前向声明 DockWidget 类
    class Frame; // 前向声明 Frame 类
    class FloatingWindow; // 前向声明 FloatingWindow 类

    /**
     * @brief TitleBarWidget 类是 TitleBar 在 Qt Widgets 环境下的具体 GUI 实现。
     *
     * 它继承自 KDDockWidgets::TitleBar (一个更抽象的基类，处理标题栏的核心逻辑)，
     * 并作为标准的 QWidget 来渲染停靠部件 (DockWidget) 或浮动窗口 (FloatingWindow) 的标题栏。
     * TitleBarWidget 负责显示标题文本、图标以及各种控制按钮（如关闭、浮动、最大化、最小化、自动隐藏）。
     */
    class DOCKS_EXPORT TitleBarWidget : public TitleBar
    {
    Q_OBJECT // 启用 Qt 元对象系统特性
        public :
        /**
         * @brief 构造一个 TitleBarWidget 对象，其父对象是一个 Frame。
         * @param parent 指向父 Frame 对象的指针。标题栏通常属于一个 Frame。
         */
        explicit TitleBarWidget(Frame *parent);

        /**
         * @brief 构造一个 TitleBarWidget 对象，其父对象是一个 FloatingWindow。
         * @param parent 指向父 FloatingWindow 对象的指针。
         */
        explicit TitleBarWidget(FloatingWindow *parent);

        /**
         * @brief 构造一个 TitleBarWidget 对象，其父对象是一个通用的 QWidget。
         *
         * 这种构造函数可能用于更灵活的场景，或者当 TitleBarWidget 不直接嵌入 Frame 或 FloatingWindow 时。
         * @param parent 父 QWidget 指针。
         */
        explicit TitleBarWidget(QWidget *parent);

        /**
         * @brief 析构函数。
         */
        ~TitleBarWidget() override;

        /**
         * @brief 获取关闭按钮的 QWidget 指针。
         * @return 指向关闭按钮 QWidget 的指针；如果不存在，则返回 nullptr。
         */
        [[nodiscard]] QWidget *closeButton() const;

    protected:
        /**
         * @brief 重写 QWidget::paintEvent()，用于自定义标题栏的绘制。
         * @param event 绘制事件对象。
         */
        void paintEvent(QPaintEvent *) override;

        /**
         * @brief 重写 QWidget::mouseDoubleClickEvent()，处理鼠标双击标题栏的事件。
         *
         * 通常用于触发停靠部件的最大化/恢复操作，或浮动窗口的相应行为。
         * @param event 鼠标双击事件对象。
         */
        void mouseDoubleClickEvent(QMouseEvent *) override;

        /**
         * @brief 更新最大化/恢复按钮的状态和外观。
         *
         * 重写基类 TitleBar 的方法。
         */
        void updateMaximizeButton() override;

        /**
         * @brief 更新最小化按钮的状态和外观。
         *
         * 重写基类 TitleBar 的方法。
         */
        void updateMinimizeButton() override;

        /**
         * @brief 更新自动隐藏按钮的状态和外观。
         *
         * 重写基类 TitleBar 的方法。
         */
        void updateAutoHideButton() override;

        /**
         * @brief 重写 QWidget::sizeHint()，提供标题栏的建议尺寸。
         * @return 标题栏的建议尺寸。
         */
        [[nodiscard]] QSize sizeHint() const override;

#ifdef DOCKS_DEVELOPER_MODE // 仅在开发者模式下编译以下内容
        // 以下方法主要用于单元测试
        /**
         * @brief (仅开发者模式) 检查关闭按钮是否可见。
         * @return 如果关闭按钮可见，则返回 true。
         */
        bool isCloseButtonVisible() const override;

        /**
         * @brief (仅开发者模式) 检查关闭按钮是否启用。
         * @return 如果关闭按钮已启用，则返回 true。
         */
        bool isCloseButtonEnabled() const override;

        /**
         * @brief (仅开发者模式) 检查浮动按钮是否可见。
         * @return 如果浮动按钮可见，则返回 true。
         */
        bool isFloatButtonVisible() const override;

        /**
         * @brief (仅开发者模式) 检查浮动按钮是否启用。
         * @return 如果浮动按钮已启用，则返回 true。
         */
        bool isFloatButtonEnabled() const override;
#endif // DOCKS_DEVELOPER_MODE

    protected:
        /**
         * @brief 初始化标题栏的组件和布局。
         *
         * 在构造函数中调用。
         */
        void init();

        /**
         * @brief 计算所有按钮区域的总宽度。
         * @return 所有可见按钮占据的总宽度。
         */
        [[nodiscard]] int buttonAreaWidth() const;

        /**
         * @brief 更新标题栏的边距。
         */
        void updateMargins();

        /**
         * @brief 计算标题栏图标的绘制区域。
         * @return 图标应绘制的 QRect。
         */
        [[nodiscard]] QRect iconRect() const;

        QHBoxLayout *const m_layout; ///< 指向标题栏内部水平布局的常量指针。
        QAbstractButton *m_closeButton = nullptr; ///< 指向关闭按钮的指针。
        QAbstractButton *m_floatButton = nullptr; ///< 指向浮动/停靠切换按钮的指针。
        QAbstractButton *m_maximizeButton = nullptr; ///< 指向最大化/恢复按钮的指针。
        QAbstractButton *m_minimizeButton = nullptr; ///< 指向最小化按钮的指针。
        QAbstractButton *m_autoHideButton = nullptr; ///< 指向自动隐藏按钮的指针。
        QLabel *m_dockWidgetIcon = nullptr; ///< 用于显示停靠部件图标的 QLabel 指针。
    };

    /**
     * @brief Button 类是一个 QToolButton 的子类，专门用作 TitleBarWidget 中的按钮。
     *
     * 这些是 KDDockWidgets 提供的默认按钮。用户可以用自己的按钮替换它们，
     * 并非强制使用这些按钮。
     * 此类主要自定义了按钮的尺寸提示和绘制行为。
     */
    class Button : public QToolButton
    {
    Q_OBJECT // 启用 Qt 元对象系统特性
        public :
        /**
         * @brief 构造一个 Button 对象。
         * @param parent 父 QWidget。
         */
        explicit Button(QWidget *parent)
        : QToolButton(parent) // 调用基类 QToolButton 的构造函数
        {
            // 设置尺寸策略为固定，确保按钮大小不会随意改变
            setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
        }

        /**
         * @brief 析构函数。
         */
        ~Button() override;

        /**
         * @brief 重写 QWidget::sizeHint()，提供按钮的建议尺寸。
         *
         * 通常基于图标大小和样式。
         * @return 按钮的建议尺寸。
         */
        [[nodiscard]] QSize sizeHint() const override;

        /**
         * @brief 重写 QWidget::paintEvent()，用于自定义按钮的绘制。
         *
         * 可能用于绘制特定样式的按钮图标或背景。
         * @param event 绘制事件对象。
         */
        void paintEvent(QPaintEvent *) override;
    };

} // namespace KDDockWidgets

#endif // KDDOCKWIDGETS_QTWIDGETS

#endif // KD_TITLEBARWIDGET_P_H
