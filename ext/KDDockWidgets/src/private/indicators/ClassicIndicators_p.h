#ifndef KD_INDICATORS_CLASSICINDICATORS_P_H
#define KD_INDICATORS_CLASSICINDICATORS_P_H

#include "../DropIndicatorOverlayInterface_p.h" // 包含拖放指示器覆盖界面的私有头文件

namespace KDDockWidgets {

class IndicatorWindow; // 前向声明 IndicatorWindow 类
class Indicator;       // 前向声明 Indicator 类

/**
 * @brief ClassicIndicators 类提供了传统样式的拖放指示器。
 *
 * 该类实现了 DropIndicatorOverlayInterface 接口，用于在拖动停靠部件时显示视觉提示（指示器），
 * 以标示潜在的停靠位置。它管理一组内部和外部指示器，用于停靠到目标部件的侧边，
 * 以及一个标签页指示器，用于作为标签页停靠。
 *
 * 这些指示器的可见性作为 QML 属性公开，使其适用于 Qt Quick 应用程序。
 */
class DOCKS_EXPORT ClassicIndicators : public DropIndicatorOverlayInterface
{
    Q_OBJECT

    // QML 属性
    /**
     * @qproperty bool innerLeftIndicatorVisible
     * @brief 表示内部左侧停靠指示器当前是否可见。
     * 此属性对于 QML 绑定非常有用。
     * @see indicatorsVisibleChanged()
     */
    Q_PROPERTY(bool innerLeftIndicatorVisible READ innerLeftIndicatorVisible NOTIFY indicatorsVisibleChanged)
    /**
     * @qproperty bool innerRightIndicatorVisible
     * @brief 表示内部右侧停靠指示器当前是否可见。
     * 此属性对于 QML 绑定非常有用。
     * @see indicatorsVisibleChanged()
     */
    Q_PROPERTY(bool innerRightIndicatorVisible READ innerRightIndicatorVisible NOTIFY indicatorsVisibleChanged)
    /**
     * @qproperty bool innerTopIndicatorVisible
     * @brief 表示内部顶部停靠指示器当前是否可见。
     * 此属性对于 QML 绑定非常有用。
     * @see indicatorsVisibleChanged()
     */
    Q_PROPERTY(bool innerTopIndicatorVisible READ innerTopIndicatorVisible NOTIFY indicatorsVisibleChanged)
    /**
     * @qproperty bool innerBottomIndicatorVisible
     * @brief 表示内部底部停靠指示器当前是否可见。
     * 此属性对于 QML 绑定非常有用。
     * @see indicatorsVisibleChanged()
     */
    Q_PROPERTY(bool innerBottomIndicatorVisible READ innerBottomIndicatorVisible NOTIFY indicatorsVisibleChanged)

    /**
     * @qproperty bool outterLeftIndicatorVisible
     * @brief 表示外部左侧停靠指示器当前是否可见。
     * 此属性对于 QML 绑定非常有用。
     * @see indicatorsVisibleChanged()
     */
    Q_PROPERTY(bool outterLeftIndicatorVisible READ outterLeftIndicatorVisible NOTIFY indicatorsVisibleChanged)
    /**
     * @qproperty bool outterRightIndicatorVisible
     * @brief 表示外部右侧停靠指示器当前是否可见。
     * 此属性对于 QML 绑定非常有用。
     * @see indicatorsVisibleChanged()
     */
    Q_PROPERTY(bool outterRightIndicatorVisible READ outterRightIndicatorVisible NOTIFY indicatorsVisibleChanged)
    /**
     * @qproperty bool outterTopIndicatorVisible
     * @brief 表示外部顶部停靠指示器当前是否可见。
     * 此属性对于 QML 绑定非常有用。
     * @see indicatorsVisibleChanged()
     */
    Q_PROPERTY(bool outterTopIndicatorVisible READ outterTopIndicatorVisible NOTIFY indicatorsVisibleChanged)
    /**
     * @qproperty bool outterBottomIndicatorVisible
     * @brief 表示外部底部停靠指示器当前是否可见。
     * 此属性对于 QML 绑定非常有用。
     * @see indicatorsVisibleChanged()
     */
    Q_PROPERTY(bool outterBottomIndicatorVisible READ outterBottomIndicatorVisible NOTIFY indicatorsVisibleChanged)

    /**
     * @qproperty bool tabIndicatorVisible
     * @brief 表示标签页停靠指示器当前是否可见。
     * 此属性对于 QML 绑定非常有用。
     * @see indicatorsVisibleChanged()
     */
    Q_PROPERTY(bool tabIndicatorVisible READ tabIndicatorVisible NOTIFY indicatorsVisibleChanged)

public:
    /**
     * @brief 构造一个 ClassicIndicators 对象。
     * @param dropArea 此指示器覆盖层所属的拖放区域。
     */
    explicit ClassicIndicators(DropArea *dropArea);

    /**
     * @brief 销毁 ClassicIndicators 对象。
     */
    ~ClassicIndicators() override;

    /**
     * @brief 实现拖放指示器的悬停行为。
     *
     * 此函数在拖动操作期间鼠标悬停时调用。
     * 它根据全局鼠标位置确定适当的停靠位置，并相应地更新指示器的可见性和位置。
     *
     * @param globalPos 全局鼠标位置。
     * @return 确定的 DropLocation (停靠位置)。
     */
    DropLocation hover_impl(QPoint globalPos) override;

    /**
     * @brief 计算特定拖放指示器的位置。
     * @param location 要获取其位置的 DropLocation (停靠位置)。
     * @return 表示指示器左上角位置的 QPoint。
     */
    [[nodiscard]] QPoint posForIndicator(DropLocation) const override;

    // 由于 QML 的需要，提供了大量的 getter 方法：
    /**
     * @brief 返回内部左侧指示器是否可见。
     * @return 如果内部左侧指示器可见，则返回 true，否则返回 false。
     */
    [[nodiscard]] bool innerLeftIndicatorVisible() const;
    /**
     * @brief 返回内部右侧指示器是否可见。
     * @return 如果内部右侧指示器可见，则返回 true，否则返回 false。
     */
    [[nodiscard]] bool innerRightIndicatorVisible() const;
    /**
     * @brief 返回内部顶部指示器是否可见。
     * @return 如果内部顶部指示器可见，则返回 true，否则返回 false。
     */
    [[nodiscard]] bool innerTopIndicatorVisible() const;
    /**
     * @brief 返回内部底部指示器是否可见。
     * @return 如果内部底部指示器可见，则返回 true，否则返回 false。
     */
    [[nodiscard]] bool innerBottomIndicatorVisible() const;
    /**
     * @brief 返回外部左侧指示器是否可见。
     * @return 如果外部左侧指示器可见，则返回 true，否则返回 false。
     */
    [[nodiscard]] bool outterLeftIndicatorVisible() const;
    /**
     * @brief 返回外部右侧指示器是否可见。
     * @return 如果外部右侧指示器可见，则返回 true，否则返回 false。
     */
    [[nodiscard]] bool outterRightIndicatorVisible() const;
    /**
     * @brief 返回外部顶部指示器是否可见。
     * @return 如果外部顶部指示器可见，则返回 true，否则返回 false。
     */
    [[nodiscard]] bool outterTopIndicatorVisible() const;
    /**
     * @brief 返回外部底部指示器是否可见。
     * @return 如果外部底部指示器可见，则返回 true，否则返回 false。
     */
    [[nodiscard]] bool outterBottomIndicatorVisible() const;
    /**
     * @brief 返回标签页指示器是否可见。
     * @return 如果标签页指示器可见，则返回 true，否则返回 false。
     */
    [[nodiscard]] bool tabIndicatorVisible() const;

protected:
    /**
     * @brief 处理覆盖层的大小调整事件。
     *
     * 当关联的拖放区域的大小发生变化时调用此函数。
     * 它会更新指示器和橡皮筋的几何形状及位置。
     *
     * @param newSize 新的尺寸。
     * @return 如果处理成功则返回 true，否则返回 false。
     */
    bool onResize(QSize newSize) override;

    /**
     * @brief 更新指示器的可见性。
     *
     * 根据当前的拖放状态和配置，更新所有指示器的可见性。
     */
    void updateVisibility() override;

Q_SIGNALS:
    /**
     * @brief 当任何指示器的可见性状态发生改变时，发射此信号。
     * 主要用于 QML 属性的通知。
     */
    void indicatorsVisibleChanged();

private:
    friend class KDDockWidgets::Indicator;       // 友元类，允许 Indicator 访问私有成员
    friend class KDDockWidgets::IndicatorWindow; // 友元类，允许 IndicatorWindow 访问私有成员

    /**
     * @brief 检查橡皮筋是否为顶层窗口。
     * @return 如果橡皮筋是顶层窗口则返回 true，否则返回 false。
     */
    [[nodiscard]] static bool rubberBandIsTopLevel();

    /**
     * @brief 提升指示器窗口的层级，确保它们在其他窗口之上。
     */
    void raiseIndicators();

    /**
     * @brief 根据局部矩形计算橡皮筋的几何形状。
     * @param localRect 局部坐标系下的矩形。
     * @return 橡皮筋的几何形状。
     */
    [[nodiscard]] QRect geometryForRubberband(QRect localRect) const;

    /**
     * @brief 设置当前的拖放位置。
     * @param location 新的拖放位置。
     */
    void setDropLocation(DropLocation);

    /**
     * @brief 更新指示器窗口的位置。
     *
     * 确保指示器窗口相对于其父窗口或屏幕正确定位。
     */
    void updateWindowPosition();

    QWidgetOrQuick *const m_rubberBand;       ///< 指向橡皮筋预览部件（可以是 QWidget 或 QQuickItem）的指针。
    IndicatorWindow *const m_indicatorWindow; ///< 指向管理所有指示器显示的窗口的指针。
};

} // namespace KDDockWidgets

#endif // KD_INDICATORS_CLASSICINDICATORS_P_H