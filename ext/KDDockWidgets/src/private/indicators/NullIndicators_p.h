#ifndef KD_NULL_INDICATORS_P_H
#define KD_NULL_INDICATORS_P_H

#include "../DropIndicatorOverlayInterface_p.h" // 包含拖放指示器覆盖界面的私有头文件

namespace KDDockWidgets {

/**
 * @brief 一个虚拟的 DropIndicatorOverlayInterface 实现，它不执行任何操作。
 *
 * 主要用于调试目的，或者在用户不希望显示拖放指示器时使用。
 * 当使用此类时，在拖放操作过程中不会有任何视觉指示器出现。
 */
class DOCKS_EXPORT NullIndicators : public DropIndicatorOverlayInterface
{
    Q_OBJECT
public:
    /**
     * @brief 构造一个 NullIndicators 对象。
     * @param dropArea 此指示器覆盖层所属的拖放区域（尽管在此类中未使用）。
     */
    explicit NullIndicators(DropArea *);

    /**
     * @brief 销毁 NullIndicators 对象。
     */
    ~NullIndicators() override;

    /**
     * @brief 实现悬停行为，但不执行任何操作。
     *
     * 无论鼠标位置如何，此函数总是返回一个空的 DropLocation。
     *
     * @param QPoint 未使用的全局鼠标位置。
     * @return 返回一个空的 DropLocation (KDDockWidgets::NoDrop)。
     */
    DropLocation hover_impl(QPoint) override
    {
        return {}; // 返回空的 DropLocation，表示没有有效的停靠位置
    }

    /**
     * @brief 静态函数，用于根据位置确定停靠位置，但不执行任何操作。
     *
     * 无论输入位置如何，此函数总是返回一个空的 DropLocation。
     *
     * @param QPoint 未使用的全局鼠标位置。
     * @return 返回一个空的 DropLocation (KDDockWidgets::NoDrop)。
     */
    [[nodiscard]] static DropLocation dropLocationForPos(QPoint)
    {
        return {}; // 返回空的 DropLocation
    }

protected:
    /**
     * @brief 获取指示器的位置，但不执行任何操作。
     *
     * 无论指定的停靠位置如何，此函数总是返回一个空的 QPoint。
     *
     * @param DropLocation 未使用的停靠位置。
     * @return 返回一个空的 QPoint。
     */
    [[nodiscard]] QPoint posForIndicator(DropLocation) const override
    {
        return {}; // 返回空的 QPoint
    }
};

} // namespace KDDockWidgets

#endif // KD_NULL_INDICATORS_P_H