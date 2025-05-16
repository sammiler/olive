#ifndef KD_INDICATORS_CLASSICINDICATORS_WINDOW_P_H
#define KD_INDICATORS_CLASSICINDICATORS_WINDOW_P_H

#include "../DropIndicatorOverlayInterface_p.h" // 包含拖放指示器覆盖界面的私有头文件
#include "ClassicIndicators_p.h" // 包含 ClassicIndicators 的私有头文件

#ifdef KDDOCKWIDGETS_QTWIDGETS // 仅当使用 Qt Widgets 时编译以下部分

#include <QImage> // 包含 QImage 类，用于图像处理
#include <QWidget> // 包含 QWidget 类，作为窗口和小部件的基类
#include <QResizeEvent> // 包含 QResizeEvent 类，用于处理大小调整事件

namespace KDDockWidgets {

class Indicator; // 前向声明 Indicator 类
class ClassicIndicators; // 前向声明 ClassicIndicators 类

/**
 * @brief IndicatorWindow 类是一个 QWidget，用于显示一组经典样式的停靠指示器。
 *
 * 这个窗口管理着多个 Indicator 对象（例如中心、左、右、上、下等指示器），
 * 并根据拖放操作过程中的鼠标位置来更新它们的可见性和位置。
 * 它与 ClassicIndicators 类紧密协作，响应其状态变化。
 */
class IndicatorWindow : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief 构造一个 IndicatorWindow 对象。
     * @param classicIndicators 指向关联的 ClassicIndicators 对象的指针，用于获取指示器状态。
     */
    explicit IndicatorWindow(ClassicIndicators *classicIndicators);

    /**
     * @brief 处理鼠标悬停事件，确定潜在的停靠位置。
     * @param globalPos 全局鼠标坐标。
     * @return 返回根据当前鼠标位置计算出的 DropLocation (停靠位置)。
     */
    DropLocation hover(QPoint globalPos);

    /**
     * @brief 更新所有指示器的位置。
     *
     * 通常在窗口大小改变或需要重新布局指示器时调用。
     */
    void updatePositions();

    /**
     * @brief 获取指定停靠位置的指示器的理想位置。
     * @param location 目标停靠位置。
     * @return 指示器应该放置的 QPoint 坐标。
     */
    [[nodiscard]] QPoint posForIndicator(DropLocation) const;

private:
    /**
     * @brief 更新各个指示器的可见性状态。
     *
     * 根据 ClassicIndicators 的状态和当前的悬停位置来显示或隐藏相应的指示器。
     */
    void updateIndicatorVisibility();

    /**
     * @brief 重写 QWidget::resizeEvent()，处理窗口大小调整事件。
     * @param ev 包含新尺寸信息的大小调整事件对象。
     */
    void resizeEvent(QResizeEvent *ev) override;

    /**
     * @brief 更新窗口的遮罩 (mask)。
     *
     * 当窗口合成器不支持半透明时（通常在某些 Linux 系统上），
     * 使用遮罩来实现不规则形状窗口的透明效果。
     */
    void updateMask();

    /**
     * @brief 根据停靠位置获取对应的 Indicator 对象。
     * @param loc 目标停靠位置。
     * @return 返回指向对应 Indicator 对象的指针，如果找不到则可能返回 nullptr。
     */
    [[nodiscard]] Indicator *indicatorForLocation(DropLocation loc) const;

    ClassicIndicators *const classicIndicators; ///< 指向关联的 ClassicIndicators 对象的指针。
    Indicator *const m_center; ///< 中心停靠指示器。
    Indicator *const m_left; ///< 左侧停靠指示器。
    Indicator *const m_right; ///< 右侧停靠指示器。
    Indicator *const m_bottom; ///< 底部停靠指示器。
    Indicator *const m_top; ///< 顶部停靠指示器。
    Indicator *const m_outterLeft; ///< 外部左侧停靠指示器。
    Indicator *const m_outterRight; ///< 外部右侧停靠指示器。
    Indicator *const m_outterBottom; ///< 外部底部停靠指示器。
    Indicator *const m_outterTop; ///< 外部顶部停靠指示器。
    QVector<Indicator *> m_indicators; ///< 存储所有指示器对象的向量。
};

/**
 * @brief Indicator 类是一个 QWidget，代表一个单独的停靠指示器图标。
 *
 * 它负责绘制指示器图标（活动和非活动状态），并处理悬停状态。
 */
class Indicator : public QWidget
{
    Q_OBJECT
public:
    typedef QList<Indicator *> List; ///< Indicator 指针列表的类型定义。

    /**
     * @brief 构造一个 Indicator 对象。
     * @param classicIndicators 指向关联的 ClassicIndicators 对象的指针。
     * @param parent 父 IndicatorWindow 对象。
     * @param location 此指示器代表的 DropLocation (停靠位置)。
     */
    explicit Indicator(ClassicIndicators *classicIndicators, IndicatorWindow *parent,
                       DropLocation location);

    /**
     * @brief 重写 QWidget::paintEvent()，绘制指示器。
     * @param event 绘制事件对象。
     */
    void paintEvent(QPaintEvent *) override;

    /**
     * @brief 设置指示器的悬停状态。
     * @param hovered 如果鼠标悬停在此指示器上，则为 true，否则为 false。
     */
    void setHovered(bool hovered);

    /**
     * @brief 获取指示器图标的名称（不含文件扩展名）。
     * @param active 指示器是否处于活动状态。
     * @return 图标名称字符串。
     */
    [[nodiscard]] QString iconName(bool active) const;

    /**
     * @brief 获取指示器图标的文件名（包含文件扩展名）。
     * @param active 指示器是否处于活动状态。
     * @return 图标文件名字符串。
     */
    [[nodiscard]] QString iconFileName(bool active) const;

    QImage m_image; ///< 非活动状态下的指示器图像。
    QImage m_imageActive; ///< 活动状态下的指示器图像。
    ClassicIndicators *const q; ///< 指向关联的 ClassicIndicators 对象的指针（通常用于访问共享数据或配置）。
    bool m_hovered = false; ///< 指示器当前是否处于悬停状态。
    const DropLocation m_dropLocation; ///< 此指示器代表的停靠位置。
};
} // namespace KDDockWidgets

#else // KDDOCKWIDGETS_QTWIDGETS， 如果不是 Qt Widgets 环境 (即 Qt Quick 环境)

#include <QQuickView> // 包含 QQuickView 类，用于在 Qt Quick 中显示内容

namespace KDDockWidgets {
class ClassicIndicators; // 前向声明 ClassicIndicators 类

/**
 * @brief IndicatorWindow 类在 Qt Quick 环境下是一个 QQuickView，用于显示经典样式的停靠指示器。
 *
 * 它通过加载 QML 文件来展示指示器，并管理这些 QML 项与 C++ 后端的交互。
 * 功能上与 QWidget 版本的 IndicatorWindow 类似，但适配 Qt Quick 架构。
 */
class IndicatorWindow : public QQuickView
{
    Q_OBJECT
    /**
     * @qproperty KDDockWidgets::ClassicIndicators* classicIndicators
     * @brief 提供对关联的 ClassicIndicators 对象的只读访问。
     * 主要用于 QML 中绑定数据。
     */
    Q_PROPERTY(KDDockWidgets::ClassicIndicators *classicIndicators READ classicIndicators CONSTANT)
public:
    /**
     * @brief 构造一个 IndicatorWindow 对象 (QQuickView 版本)。
     * @param classicIndicators 指向关联的 ClassicIndicators 对象的指针。
     */
    explicit IndicatorWindow(ClassicIndicators *);

    /**
     * @brief 处理鼠标悬停事件。
     * @param globalPos 全局鼠标坐标。
     * @return 根据鼠标位置计算出的 DropLocation (停靠位置)。
     */
    DropLocation hover(QPoint);

    /**
     * @brief 更新 QML 中指示器的位置。
     */
    void updatePositions();

    /**
     * @brief 获取指定停靠位置的指示器的理想位置。
     * @param location 目标停靠位置。
     * @return 指示器应该放置的 QPoint 坐标。
     */
    QPoint posForIndicator(DropLocation) const;

    /**
     * @brief Q_INVOKABLE 方法，用于从 QML 获取指定位置和状态的图标名称。
     * @param loc 代表 DropLocation 的整数值。
     * @param active 指示器是否处于活动状态。
     * @return 图标名称字符串。
     */
    Q_INVOKABLE QString iconName(int loc, bool active) const;

    /**
     * @brief 返回关联的 ClassicIndicators 对象。
     * @return 指向 ClassicIndicators 对象的常量指针。
     */
    KDDockWidgets::ClassicIndicators *classicIndicators() const;

    /**
     * @brief 根据停靠位置获取对应的 QML 指示器项。
     * @param loc 目标停靠位置。
     * @return 指向对应 QQuickItem 的指针，如果找不到则可能返回 nullptr。
     */
    QQuickItem *indicatorForLocation(DropLocation loc) const;

private:
    /**
     * @brief 根据 QML 指示器项获取其代表的停靠位置。
     * @param item 指向 QQuickItem 的指针。
     * @return 对应的 DropLocation。
     */
    DropLocation locationForIndicator(const QQuickItem *) const;

    /**
     * @brief 根据屏幕位置获取其下方的 QML 指示器项。
     * @param pos 屏幕坐标点。
     * @return 指向对应 QQuickItem 的指针，如果找不到则可能返回 nullptr。
     */
    QQuickItem *indicatorForPos(QPoint) const;

    /**
     * @brief 获取所有 QML 指示器项的列表。
     * @return 包含所有 QQuickItem 指示器的向量。
     */
    QVector<QQuickItem *> indicatorItems() const;

    ClassicIndicators *const m_classicIndicators; ///< 指向关联的 ClassicIndicators 对象的指针。
};
} // namespace KDDockWidgets

#endif // KDDOCKWIDGETS_QTWIDGETS

#endif // KD_INDICATORS_CLASSICINDICATORS_WINDOW_P_H