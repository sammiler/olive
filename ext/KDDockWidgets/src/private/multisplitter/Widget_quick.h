#pragma once // 确保该头文件在一次编译中仅被包含一次

#ifdef KDDOCKWIDGETS_QTQUICK // 仅当定义了 KDDOCKWIDGETS_QTQUICK 宏时才编译以下内容 (表示在 Qt Quick 环境下)

#include "Widget.h" // 包含 Widget.h，这是 Widget_quick 的基类

#include <QQuickItem> // 包含 QQuickItem 类，这是 Qt Quick 中所有可视化项的基类

QT_BEGIN_NAMESPACE // Qt 命名空间开始
    class QQuickItem; // 前向声明 QQuickItem 类 (虽然已经 #include，但有时仍会保留以示明确)
QT_END_NAMESPACE // Qt 命名空间结束

    /**
     * @file
     * @brief 一个处理 QQuickItem 的 Layouting::Widget 实现。
     */

    namespace Layouting
{ // 布局相关的命名空间

    /**
     * @brief Widget_quick 类是 Layouting::Widget 的一个具体实现，专门用于包装和管理 QQuickItem。
     *
     * 这个类允许 KDDockWidgets 的布局系统能够容纳和操作 Qt Quick 项目 (QQuickItem)，
     * 使得布局逻辑可以与 QQuickItem 的几何属性、可见性等进行交互。
     * 它通过继承 Layouting::Widget 并实现其纯虚函数，桥接了抽象布局层和具体的 Qt Quick UI元素。
     */
    class DOCKS_EXPORT Widget_quick : public Widget
    {
    public:
        /**
         * @brief 构造一个 Widget_quick 对象。
         * @param thisWidget 指向被此包装器管理的实际 QQuickItem 对象的指针。
         * 该 QQuickItem 将被传递给基类 Widget 的构造函数。
         */
        explicit Widget_quick(QQuickItem *thisWidget)
            : Widget(thisWidget) // 调用基类 Widget 的构造函数，传入 QQuickItem 作为 QObject
            , m_thisWidget(thisWidget) // 初始化成员变量，保存对 QQuickItem 的直接指针
        {
        }

        /**
         * @brief 析构函数。
         */
        ~Widget_quick() override;

        /**
         * @brief 获取 QQuickItem 的最小尺寸。
         *
         * 重写 Layouting::Widget::minSize()。
         * @return QQuickItem 的最小允许尺寸。
         */
        QSize minSize() const override;

        /**
         * @brief 获取 QQuickItem 的最大尺寸提示。
         *
         * 重写 Layouting::Widget::maxSizeHint()。
         * @return QQuickItem 的最大建议尺寸。
         */
        QSize maxSizeHint() const override;

        /**
         * @brief 获取 QQuickItem 的当前几何区域 (位置和尺寸)。
         *
         * 重写 Layouting::Widget::geometry()。
         * @return QQuickItem 的几何 QRect。
         */
        QRect geometry() const override;

        /**
         * @brief 设置 QQuickItem 的几何区域。
         *
         * 重写 Layouting::Widget::setGeometry()。
         * @param rect 新的几何 QRect。
         */
        void setGeometry(QRect) override;

        /**
         * @brief 设置此 QQuickItem 包装器的父 Layouting::Widget。
         *
         * 重写 Layouting::Widget::setParent()。
         * 这通常涉及到设置实际 QQuickItem 的父 QQuickItem (如果适用)。
         * @param parentWidget 指向父 Layouting::Widget 的指针。
         */
        void setParent(Widget *parentWidget) override;

        /**
         * @brief 将 QQuickItem 的调试信息输出到 QDebug 流。
         *
         * 重写 Layouting::Widget::dumpDebug()。
         * @param debug QDebug 流对象。
         * @return 对 QDebug 流对象的引用。
         */
        QDebug &dumpDebug(QDebug &) const override;

        /**
         * @brief 检查 QQuickItem 当前是否可见。
         *
         * 重写 Layouting::Widget::isVisible()。
         * @return 如果 QQuickItem 可见，则返回 true。
         */
        bool isVisible() const override;

        /**
         * @brief 设置 QQuickItem 的可见性。
         *
         * 重写 Layouting::Widget::setVisible()。
         * @param visible 如果为 true，则显示 QQuickItem；否则隐藏 QQuickItem。
         */
        void setVisible(bool) const override;

        /**
         * @brief 获取此 QQuickItem 包装器的父 Layouting::Widget。
         *
         * 重写 Layouting::Widget::parentWidget()。
         * @return 指向父 Layouting::Widget 的 std::unique_ptr。
         */
        std::unique_ptr<Widget> parentWidget() const override;

        /**
         * @brief 将此 Widget_quick 与一个布局项 (Layouting::Item) 相关联。
         *
         * 重写 Layouting::Widget::setLayoutItem()。
         * 在这个实现中，它是一个空操作，可能意味着关联逻辑在其他地方处理，
         * 或者对于 QQuickItem 包装器，这种直接关联不是必需的。
         * @param item 指向要关联的布局项的指针 (未使用)。
         */
        void setLayoutItem(Item *) override
        {
            // 空实现
        }

        /**
         * @brief 显示 QQuickItem。
         *
         * 重写 Layouting::Widget::show()。
         */
        void show() override;

        /**
         * @brief 隐藏 QQuickItem。
         *
         * 重写 Layouting::Widget::hide()。
         */
        void hide() override;

        /**
         * @brief 移动 QQuickItem 到新的位置。
         *
         * 重写 Layouting::Widget::move()。
         * @param x 新的 x 坐标。
         * @param y 新的 y 坐标。
         */
        void move(int x, int y) override;

        /**
         * @brief 设置 QQuickItem 的尺寸。
         *
         * 重写 Layouting::Widget::setSize()。
         * @param width 新的宽度。
         * @param height 新的高度。
         */
        void setSize(int width, int height) override;

        /**
         * @brief 设置 QQuickItem 的宽度。
         *
         * 重写 Layouting::Widget::setWidth()。
         * @param width 新的宽度。
         */
        void setWidth(int width) override;

        /**
         * @brief 设置 QQuickItem 的高度。
         *
         * 重写 Layouting::Widget::setHeight()。
         * @param height 新的高度。
         */
        void setHeight(int height) override;

        /**
         * @brief 请求更新 QQuickItem 的显示。
         *
         * 重写 Layouting::Widget::update()。
         */
        void update() override;

        /**
         * @brief 静态辅助函数，用于计算 QWidget 的最小尺寸。
         *
         * 注意：这个静态函数参数类型是 QWidget*，但在 Widget_quick 类中。
         * 这可能是从 Widget_qwidget.h 中遗留的或者是用于某种特定转换或比较的辅助函数，
         * 尽管此类主要处理 QQuickItem。
         *
         * @param w 指向 QWidget 的指针。
         * @return 计算得出的最小 QSize。
         */
        static QSize widgetMinSize(const QWidget *w);

    protected:
        /**
         * @brief 创建并返回一个新的 QQuickItem 实例，从指定的 QML 文件加载。
         * @param filename QML 文件的路径或 URL。
         * @param parent 新创建的 QQuickItem 的父项。
         * @return 指向新创建的 QQuickItem 的指针。
         */
        QQuickItem *createQQuickItem(const QString &filename, QQuickItem *parent) const;

    private:
        QQuickItem *const m_thisWidget; ///< 指向被此包装器管理的实际 QQuickItem 对象的常量指针。
        Q_DISABLE_COPY(Widget_quick) // 禁止拷贝构造函数和赋值操作符
    };

} // namespace Layouting

#endif // KDDOCKWIDGETS_QTQUICK
