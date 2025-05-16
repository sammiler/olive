#pragma once // 确保该头文件在一次编译中仅被包含一次

#ifdef KDDOCKWIDGETS_QTWIDGETS // 仅当定义了 KDDOCKWIDGETS_QTWIDGETS 宏时才编译以下内容 (表示在 Qt Widgets 环境下)

#include "Widget.h" // 包含 Widget.h，这是 Widget_qwidget 的基类

QT_BEGIN_NAMESPACE   // Qt 命名空间开始
class QWidget;       // 前向声明 QWidget 类
QT_END_NAMESPACE     // Qt 命名空间结束

/**
 * @file
 * @brief 一个处理 QWidget 的 Layouting::Widget 实现。
 */

namespace Layouting { // 布局相关的命名空间

/**
 * @brief Widget_qwidget 类是 Layouting::Widget 的一个具体实现，专门用于包装和管理 QWidget。
 *
 * 这个类允许 KDDockWidgets 的布局系统能够容纳和操作标准的 Qt Widgets (QWidget)，
 * 使得布局逻辑可以与 QWidget 的几何属性、可见性、父子关系等进行交互。
 * 它通过继承 Layouting::Widget 并实现其纯虚函数，桥接了抽象布局层和具体的 Qt Widgets UI元素。
 */
class DOCKS_EXPORT Widget_qwidget : public Widget
{
public:
    /**
     * @brief 构造一个 Widget_qwidget 对象。
     * @param thisWidget 指向被此包装器管理的实际 QWidget 对象的指针。
     * 该 QWidget 将被传递给基类 Widget 的构造函数 (作为 QObject*)。
     */
    explicit Widget_qwidget(QWidget *thisWidget);

    /**
     * @brief 析构函数。
     */
    ~Widget_qwidget() override;

    /**
     * @brief 将此 Layouting::Widget 转换为其包装的 QWidget 指针。
     *
     * 重写 Layouting::Widget::asQWidget()。
     * @return 指向被包装的 QWidget 对象的指针。
     */
    [[nodiscard]] QWidget *asQWidget() const override
    {
        return m_thisWidget;
    }

    /**
     * @brief 获取 QWidget 的建议尺寸 (sizeHint)。
     *
     * 重写 Layouting::Widget::sizeHint()。
     * @return QWidget 的建议尺寸。
     */
    [[nodiscard]] QSize sizeHint() const override;

    /**
     * @brief 获取 QWidget 的最小尺寸。
     *
     * 重写 Layouting::Widget::minSize()。
     * @return QWidget 的最小允许尺寸。
     */
    [[nodiscard]] QSize minSize() const override;

    /**
     * @brief 获取 QWidget 的最大尺寸提示。
     *
     * 重写 Layouting::Widget::maxSizeHint()。
     * @return QWidget 的最大建议尺寸。
     */
    [[nodiscard]] QSize maxSizeHint() const override;

    /**
     * @brief 获取 QWidget 的当前几何区域 (相对于其父Widget的位置和尺寸)。
     *
     * 重写 Layouting::Widget::geometry()。
     * @return QWidget 的几何 QRect。
     */
    [[nodiscard]] QRect geometry() const override;

    /**
     * @brief 设置 QWidget 的几何区域。
     *
     * 重写 Layouting::Widget::setGeometry()。
     * @param rect 新的几何 QRect。
     */
    void setGeometry(QRect) override;

    /**
     * @brief 设置此 QWidget 包装器的父 Layouting::Widget。
     *
     * 重写 Layouting::Widget::setParent()。
     * 这通常涉及到调用被包装 QWidget 的 setParent() 方法。
     * @param parentWidget 指向父 Layouting::Widget 的指针。
     */
    void setParent(Widget *) override;

    /**
     * @brief 将 QWidget 的调试信息输出到 QDebug 流。
     *
     * 重写 Layouting::Widget::dumpDebug()。
     * @param debug QDebug 流对象。
     * @return 对 QDebug 流对象的引用。
     */
    QDebug &dumpDebug(QDebug &) const override;

    /**
     * @brief 检查 QWidget 当前是否可见。
     *
     * 重写 Layouting::Widget::isVisible()。
     * @return 如果 QWidget 可见，则返回 true。
     */
    [[nodiscard]] bool isVisible() const override;

    /**
     * @brief 设置 QWidget 的可见性。
     *
     * 重写 Layouting::Widget::setVisible()。
     * @param visible 如果为 true，则显示 QWidget；否则隐藏 QWidget。
     */
    void setVisible(bool) const override; // 注意：参数为 const，这可能是一个笔误，通常 setVisible 不应该是 const

    /**
     * @brief 获取此 QWidget 包装器的父 Layouting::Widget。
     *
     * 重写 Layouting::Widget::parentWidget()。
     * @return 指向父 Layouting::Widget 的 std::unique_ptr。
     */
    [[nodiscard]] std::unique_ptr<Widget> parentWidget() const override;

    /**
     * @brief 将此 Widget_qwidget 与一个布局项 (Layouting::Item) 相关联。
     *
     * 重写 Layouting::Widget::setLayoutItem()。
     * 在这个实现中，它是一个空操作，可能意味着关联逻辑在其他地方处理，
     * 或者对于 QWidget 包装器，这种直接关联不是必需的。
     * @param item 指向要关联的布局项的指针 (未使用)。
     */
    void setLayoutItem(Item *) override
    {
        // 空实现
    }

    /**
     * @brief 显示 QWidget。
     *
     * 重写 Layouting::Widget::show()。
     */
    void show() override;

    /**
     * @brief 隐藏 QWidget。
     *
     * 重写 Layouting::Widget::hide()。
     */
    void hide() override;

    /**
     * @brief 移动 QWidget 到新的位置。
     *
     * 重写 Layouting::Widget::move()。
     * @param x 新的 x 坐标。
     * @param y 新的 y 坐标。
     */
    void move(int x, int y) override;

    /**
     * @brief 设置 QWidget 的尺寸。
     *
     * 重写 Layouting::Widget::setSize()。
     * @param width 新的宽度。
     * @param height 新的高度。
     */
    void setSize(int width, int height) override;

    /**
     * @brief 设置 QWidget 的宽度。
     *
     * 重写 Layouting::Widget::setWidth()。
     * @param width 新的宽度。
     */
    void setWidth(int width) override;

    /**
     * @brief 设置 QWidget 的高度。
     *
     * 重写 Layouting::Widget::setHeight()。
     * @param height 新的高度。
     */
    void setHeight(int height) override;

    /**
     * @brief 请求更新 QWidget 的显示。
     *
     * 重写 Layouting::Widget::update()。
     */
    void update() override;

private:
    QWidget *const m_thisWidget;   ///< 指向被此包装器管理的实际 QWidget 对象的常量指针。
    Q_DISABLE_COPY(Widget_qwidget) // 禁止拷贝构造函数和赋值操作符
};

} // namespace Layouting


#endif // KDDOCKWIDGETS_QTWIDGETS
