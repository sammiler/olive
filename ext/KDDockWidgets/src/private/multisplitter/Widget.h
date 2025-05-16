/**
 * @file
 * @brief 对 QWidget、QtQuickItem 或其他任何 UI 元素的抽象/包装器。
 */

#pragma once // 确保该头文件在一次编译中仅被包含一次

#include "kddockwidgets/docks_export.h" // 包含导出宏定义

#include <QRect>     // 包含 QRect 类，用于表示矩形区域
#include <QSize>     // 包含 QSize 类，用于表示尺寸
#include <QDebug>    // 包含 QDebug 类，用于调试输出
#include <QObject>   // 包含 QObject 类，Qt 对象模型的基础
#include <qglobal.h> // 包含 Qt 全局定义，例如 Q_DISABLE_COPY

#include <memory> // 包含智能指针相关的头文件

QT_BEGIN_NAMESPACE // Qt 命名空间开始
class QWidget;       // 前向声明 QWidget 类
QT_END_NAMESPACE   // Qt 命名空间结束

#ifdef QT_WIDGETS_LIB // 如果定义了 QT_WIDGETS_LIB (表示正在使用 Qt Widgets 模块)
#include <QSizePolicy> // 包含 QSizePolicy 类，用于描述部件的大小策略
#else // 如果没有定义 QT_WIDGETS_LIB (例如，在纯 Qt Quick 环境中)
/**
 * @brief QSizePolicy 的一个简化版本，用于在非 Qt Widgets 环境下提供兼容性。
 *
 * 当 KDDockWidgets 在不包含 QtWidgets 模块的环境中编译时（例如纯 Qt Quick 应用），
 * 这个类提供了 QSizePolicy 的基本功能，以便布局逻辑能够以统一的方式处理尺寸策略。
 */
class QSizePolicy
{
public:
    /**
     * @brief 定义尺寸策略的枚举。
     */
    enum Policy {
        Fixed,     ///< 固定尺寸：sizeHint() 是唯一可接受的尺寸。
        Preferred, ///< 优先尺寸：sizeHint() 是首选尺寸，但可以缩小到 minimumSizeHint() 或扩展到 maximumSize()。
        Maximum    ///< 最大尺寸：sizeHint() 是首选尺寸，可以缩小到 minimumSizeHint()，但不能比 sizeHint() 更大。
    };

    /**
     * @brief 构造函数，使用指定的水平和垂直策略。
     * @param h 水平尺寸策略。
     * @param v 垂直尺寸策略。
     */
    explicit QSizePolicy(Policy h, Policy v)
        : horizontal(h)
        , vertical(v)
    {
    }

    /**
     * @brief 获取垂直尺寸策略。
     * @return 垂直方向的 Policy。
     */
    Policy verticalPolicy() const
    {
        return vertical;
    }

    /**
     * @brief 获取水平尺寸策略。
     * @return 水平方向的 Policy。
     */
    Policy horizontalPolicy() const
    {
        return horizontal;
    }

    Policy horizontal = Fixed; ///< 水平尺寸策略，默认为 Fixed。
    Policy vertical = Fixed;   ///< 垂直尺寸策略，默认为 Fixed。
};
#endif // QT_WIDGETS_LIB

namespace Layouting { // 布局相关的命名空间

class Item; // 前向声明 Item 类

/**
 * @brief Widget 类是对 QWidget、QQuickItem 或其他任何 UI 元素的抽象/包装器。
 *
 * 这样，布局系统就可以托管 QWidget 等，而无需直接依赖它们。
 * 通过多重继承来继承它，以便在实际的 QWidget/QQuickItem 被删除时，这个包装器也被删除。
 * 这个类定义了一个通用接口，供布局系统与具体的 UI 实现（如 Qt Widgets 或 Qt Quick）进行交互。
 */
class DOCKS_EXPORT Widget
{
public:
    /**
     * @brief 构造一个 Widget 对象。
     * @param thisObj 指向实际 UI 对象 (如 QWidget 或 QQuickItem) 的 QObject 指针。
     * 这个对象用于生命周期管理和类型转换。
     */
    explicit Widget(QObject *thisObj);

    /**
     * @brief 虚析构函数。
     */
    virtual ~Widget();

    /**
     * @brief 将此 Widget 与一个布局项 (Layouting::Item) 相关联。
     *
     * 纯虚函数，必须由派生类实现。
     * @param item 指向要关联的布局项的指针。
     */
    virtual void setLayoutItem(Item *) = 0;

    /**
     * @brief 将此 Widget 转换为 QWidget 指针 (如果适用)。
     *
     * 默认实现断言失败并返回 nullptr。
     * 仅当 Widget 包装的是一个 QWidget 时，派生类 (如 Widget_qwidget) 才应重写此方法。
     * @return 如果此 Widget 包装的是 QWidget，则返回其指针；否则返回 nullptr。
     */
    [[nodiscard]] virtual QWidget *asQWidget() const
    {
        Q_ASSERT(false); // 仅 QtWidgets 需要。所有其他类型不应调用此方法。
        return nullptr;
    }

    /**
     * @brief 获取部件的建议尺寸。
     *
     * 默认返回一个空的 QSize。派生类应重写以返回其包装的 UI 元素的建议尺寸。
     * @return 部件的建议尺寸。
     */
    [[nodiscard]] virtual QSize sizeHint() const
    {
        return {};
    }

    /**
     * @brief 获取部件的最小尺寸 (纯虚函数)。
     * @return 部件的最小允许尺寸。
     */
    [[nodiscard]] virtual QSize minSize() const = 0;

    /**
     * @brief 获取部件的最大尺寸提示 (纯虚函数)。
     * @return 部件的最大建议尺寸。
     */
    [[nodiscard]] virtual QSize maxSizeHint() const = 0;

    /**
     * @brief 获取部件的当前几何区域 (位置和尺寸) (纯虚函数)。
     * @return 部件的几何 QRect。
     */
    [[nodiscard]] virtual QRect geometry() const = 0;

    /**
     * @brief 设置部件的几何区域 (纯虚函数)。
     * @param rect 新的几何 QRect。
     */
    virtual void setGeometry(QRect) = 0;

    /**
     * @brief 设置此部件的父 Widget (纯虚函数)。
     * @param parent 指向父 Layouting::Widget 的指针。
     */
    virtual void setParent(Widget *) = 0;

    /**
     * @brief 将部件的调试信息输出到 QDebug 流 (纯虚函数)。
     * @param debug QDebug 流对象。
     * @return 对 QDebug 流对象的引用。
     */
    virtual QDebug &dumpDebug(QDebug &) const = 0;

    /**
     * @brief 检查部件当前是否可见 (纯虚函数)。
     * @return 如果部件可见，则返回 true。
     */
    [[nodiscard]] virtual bool isVisible() const = 0;

    /**
     * @brief 设置部件的可见性 (纯虚函数)。
     * @param visible 如果为 true，则显示部件；否则隐藏部件。
     */
    virtual void setVisible(bool) const = 0; // 注意：参数为 const，这可能是一个笔误，通常 setVisible 不应该是 const

    /**
     * @brief 移动部件到新的位置 (纯虚函数)。
     * @param x 新的 x 坐标。
     * @param y 新的 y 坐标。
     */
    virtual void move(int x, int y) = 0;

    /**
     * @brief 设置部件的尺寸 (纯虚函数)。
     * @param width 新的宽度。
     * @param height 新的高度。
     */
    virtual void setSize(int width, int height) = 0;

    /**
     * @brief 设置部件的宽度 (纯虚函数)。
     * @param width 新的宽度。
     */
    virtual void setWidth(int width) = 0;

    /**
     * @brief 设置部件的高度 (纯虚函数)。
     * @param height 新的高度。
     */
    virtual void setHeight(int height) = 0;

    /**
     * @brief 获取此部件的父 Widget (纯虚函数)。
     * @return 指向父 Layouting::Widget 的 std::unique_ptr。
     */
    [[nodiscard]] virtual std::unique_ptr<Widget> parentWidget() const = 0;

    /**
     * @brief 显示部件 (纯虚函数)。
     */
    virtual void show() = 0;

    /**
     * @brief 隐藏部件 (纯虚函数)。
     */
    virtual void hide() = 0;

    /**
     * @brief 请求更新部件的显示 (纯虚函数)。
     */
    virtual void update() = 0;

    /**
     * @brief 获取部件的当前尺寸。
     * @return QSize 表示部件的当前尺寸。
     */
    [[nodiscard]] QSize size() const
    {
        return geometry().size();
    }

    /**
     * @brief 获取部件在其自身坐标系中的矩形 (原点为 0,0)。
     * @return QRect 表示部件的局部矩形。
     */
    [[nodiscard]] QRect rect() const
    {
        return QRect(QPoint(0, 0), size());
    }

    /**
     * @brief 获取包装的实际 UI 对象作为 QObject 指针。
     * @return 指向实际 UI 对象的 QObject 指针。
     */
    [[nodiscard]] QObject *asQObject() const
    {
        return m_thisObj;
    }

    /**
     * @brief 获取包装的实际 UI 对象的父 QObject。
     * @return 指向父 QObject 的指针。
     */
    [[nodiscard]] QObject *parent() const
    {
        return m_thisObj->parent();
    }

    /**
     * @brief 获取部件的 x 坐标。
     * @return x 坐标值。
     */
    [[nodiscard]] int x() const
    {
        return geometry().x();
    }

    /**
     * @brief 获取部件的 y 坐标。
     * @return y 坐标值。
     */
    [[nodiscard]] int y() const
    {
        return geometry().y();
    }

    /**
     * @brief 重载相等操作符，用于比较此 Widget 是否包装了给定的 QObject。
     * @param obj 要比较的 QObject 指针。
     * @return 如果此 Widget 包装的是同一个 QObject，则返回 true。
     */
    bool operator==(const QObject *obj) const
    {
        return obj == m_thisObj;
    }

    /**
     * @brief 返回一个用于关联目的的 ID，主要用于保存和恢复布局。
     * @return 唯一标识此 Widget 的 QString。
     */
    [[nodiscard]] QString id() const;

    /**
     * @brief 返回一个硬编码的最小尺寸。
     *
     * 此尺寸作为所有 Widget 的绝对最小尺寸，防止它们变得过小。
     * @return 硬编码的最小 QSize。
     */
    static QSize hardcodedMinimumSize();

    /**
     * @brief 模板函数，计算给定类型 T 的部件的最小尺寸。
     *
     * 它会考虑部件的 minimumWidth/Height 和 minimumSizeHint，并确保结果不小于 hardcodedMinimumSize。
     * @tparam T 部件的类型 (例如 QWidget, QQuickItem 的适配器)。
     * @param w 指向部件的指针。
     * @return 计算得出的最小 QSize。
     */
    template<typename T>
    static QSize widgetMinSize(const T *w)
    {
        const int minW = w->minimumWidth() > 0 ? w->minimumWidth()
                                               : w->minimumSizeHint().width();

        const int minH = w->minimumHeight() > 0 ? w->minimumHeight()
                                                : w->minimumSizeHint().height();

        return QSize(minW, minH).expandedTo(hardcodedMinimumSize());
    }

    /**
     * @brief 模板函数，计算给定类型 T 的部件的最大尺寸提示。
     *
     * 它考虑了部件的 maximumSize() 和 QSizePolicy，特别是 Fixed 和 Maximum策略。
     * 结果会经过边界检查，确保不小于最小尺寸。
     * @tparam T 部件的类型。
     * @param w 指向部件的指针。
     * @return 计算得出的最大 QSize 提示。
     */
    template<typename T>
    static QSize widgetMaxSize(const T *w)
    {
        // 最大尺寸通常是 QWidget::maximumSize()，但我们也考虑 QSizePolicy::Fixed+sizeHint() 的情况，
        // 因为部件不需要设置 QWidget::maximumSize() 也能使其最大尺寸生效。

        const QSize min = widgetMinSize(w);
        QSize max = w->maximumSize();
        max = boundedMaxSize(min, max); // 为了安全起见，防止出现奇怪的值

        const QSizePolicy policy = w->sizePolicy();

        if (policy.verticalPolicy() == QSizePolicy::Fixed || policy.verticalPolicy() == QSizePolicy::Maximum)
            max.setHeight(qMin(max.height(), w->sizeHint().height()));
        if (policy.horizontalPolicy() == QSizePolicy::Fixed || policy.horizontalPolicy() == QSizePolicy::Maximum)
            max.setWidth(qMin(max.width(), w->sizeHint().width()));

        max = boundedMaxSize(min, max); // 为了安全起见，防止出现奇怪的值
        return max;
    }

protected:
    /**
     * @brief 辅助函数，确保最大尺寸不小于最小尺寸，并对最大尺寸进行合理限制。
     * @param min 最小尺寸。
     * @param max 原始最大尺寸。
     * @return 经过边界调整后的最大尺寸。
     */
    static QSize boundedMaxSize(QSize min, QSize max);

private:
    const QString m_id;         ///< 用于保存布局时关联的唯一 ID。
    QObject *const m_thisObj;   ///< 指向实际 UI 对象 (QWidget, QQuickItem 等) 的 QObject 指针。
    Q_DISABLE_COPY(Widget)      // 禁止拷贝构造函数和赋值操作符
};

/**
 * @brief 重载相等操作符，用于比较 Widget 指针和 QObject 引用。
 * @param w 指向 Layouting::Widget 的指针。
 * @param obj 对 QObject 的引用。
 * @return 如果 w 非空且其包装的 QObject 与 obj 相同，则返回 true。
 */
inline bool operator==(const Widget *w, const QObject &obj)
{
    // if (!w && !obj) // 这段注释掉的代码似乎是想处理两者都为空的情况
    // return true;

    return w && w->operator==(&obj);
}

} // namespace Layouting
