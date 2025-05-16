#ifndef KD_MULTISPLITTER_SEPARATOR_QUICK_H
#define KD_MULTISPLITTER_SEPARATOR_QUICK_H

#ifdef KDDOCKWIDGETS_QTQUICK // 仅当定义了 KDDOCKWIDGETS_QTQUICK 宏时才编译以下内容 (表示在 Qt Quick 环境下)

#include "kddockwidgets/docks_export.h" // 包含导出宏定义，用于库的导出和导入
#include "Separator_p.h" // 包含 Separator 私有头文件 (SeparatorQuick 的基类之一)
#include "Widget_quick.h" // 包含 Widget_quick.h (SeparatorQuick 的基类之一，用于 Qt Quick 的 Widget 适配)

#include <QQuickItem> // 包含 QQuickItem 类，这是 Qt Quick 中所有可视化项的基类

namespace Layouting { // 布局相关的命名空间

/**
 * @brief SeparatorQuick 类是 Layouting::Separator 在 Qt Quick 环境下的具体实现。
 *
 * 此类继承自 QQuickItem，使其能够作为 Qt Quick 场景中的一个可视化元素，
 * 同时继承自 Layouting::Separator 来实现分隔条的核心逻辑，
 * 并继承自 Layouting::Widget_quick 以便与 KDDockWidgets 的 Qt Quick Widget 框架集成。
 *
 * 它允许用户在 QML 中创建和交互分隔条，用于调整相邻停靠部件的大小。
 * 通过 Q_INVOKABLE 方法，QML 中的鼠标事件可以被转发到 C++ 基类的处理函数中。
 */
class DOCKS_EXPORT SeparatorQuick
    : public QQuickItem, // 继承自 QQuickItem，使其成为一个 Qt Quick 可视化项
      public Layouting::Separator, // 继承自 Layouting::Separator，获取分隔条的核心逻辑
      public Layouting::Widget_quick // 继承自 Layouting::Widget_quick，作为 Qt Quick 的 Widget 适配器
{
Q_OBJECT // 启用 Qt 元对象系统特性，如信号和槽

/**
 * @qproperty bool isVertical
 * @brief 表示分隔条是否为垂直方向。
 * 此属性绑定到 Layouting::Separator::isVertical() 方法。
 * @see isVerticalChanged()
 */
Q_PROPERTY(bool isVertical READ isVertical NOTIFY isVerticalChanged) public :
    /**
     * @brief 构造一个 SeparatorQuick 对象。
     * @param parent 指向父 Layouting::Widget 对象的指针，默认为 nullptr。
     * 注意：这里的 parent 是逻辑上的父 Widget，而不是 QQuickItem 的可视化父项。
     */
    explicit SeparatorQuick(Layouting::Widget *parent = nullptr);

    /**
     * @brief 返回分隔条是否为垂直方向。
     *
     * 此方法是 Q_PROPERTY isVertical 的读取函数。
     * @return 如果分隔条是垂直的，则返回 true；否则返回 false (表示水平)。
     */
    bool isVertical() const;

protected:
    /**
     * @brief 创建一个橡皮筋 (RubberBand) 部件，用于在拖动分隔条时提供视觉反馈。
     *
     * 重写基类 Layouting::Separator::createRubberBand 方法。
     * @param parent 橡皮筋的父 Widget。
     * @return 指向新创建的橡皮筋 Widget 的指针，如果不支持则返回 nullptr。
     */
    Widget *createRubberBand(Widget *parent) override;

    /**
     * @brief 返回代表此 SeparatorQuick 实例的 Layouting::Widget 接口。
     *
     * 重写基类 Layouting::Separator::asWidget 方法。
     * @return 指向自身的 Layouting::Widget 指针 (通过 Widget_quick 基类实现)。
     */
    Widget *asWidget() override;

public:
    // QML 接口方法:
    /**
     * @brief Q_INVOKABLE 方法，用于从 QML 中调用，处理鼠标按下事件。
     *
     * 此方法会调用基类 Layouting::Separator::onMousePress()。
     */
    Q_INVOKABLE void onMousePressed();

    /**
     * @brief Q_INVOKABLE 方法，用于从 QML 中调用，处理鼠标移动事件。
     * @param localPos 当前鼠标相对于此 QQuickItem 的局部坐标。
     * 注意：基类 Layouting::Separator::onMouseMove() 可能期望的是相对于父容器的坐标，
     * 因此这里可能需要进行坐标转换。
     */
    Q_INVOKABLE void onMouseMoved(QPointF localPos);

    /**
     * @brief Q_INVOKABLE 方法，用于从 QML 中调用，处理鼠标释放事件。
     *
     * 此方法会调用基类 Layouting::Separator::onMouseReleased()。
     */
    Q_INVOKABLE void onMouseReleased();

    /**
     * @brief Q_INVOKABLE 方法，用于从 QML 中调用，处理鼠标双击事件。
     *
     * 此方法会调用基类 Layouting::Separator::onMouseDoubleClick()。
     */
    Q_INVOKABLE void onMouseDoubleClicked();

Q_SIGNALS:
    /**
     * @brief 当分隔条的方向 (isVertical 属性) 确定或发生改变时发射此信号。
     *
     * 注意：注释中提到 "constant but it's only set after Separator::init"，
     * 这意味着该属性在 Separator::init() 调用后通常是固定的，但仍通过信号通知其初始状态。
     */
    void isVerticalChanged();
};

} // namespace Layouting
#endif // KDDOCKWIDGETS_QTQUICK

#endif // KD_MULTISPLITTER_SEPARATOR_QUICK_H
