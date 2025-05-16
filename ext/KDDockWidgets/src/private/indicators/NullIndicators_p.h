#ifndef KD_NULL_INDICATORS_P_H
#define KD_NULL_INDICATORS_P_H

#include "../DropIndicatorOverlayInterface_p.h"

namespace KDDockWidgets {

/**
 * @brief A dummy DropIndicatorOverlayInterface implementation which doesn't do anything.
 *
 * Used for debugging purposes or if someone doesn't want the drop indicators.
 */
class DOCKS_EXPORT NullIndicators : public DropIndicatorOverlayInterface
{
    Q_OBJECT
public:
    explicit NullIndicators(DropArea *);
    ~NullIndicators() override;
    DropLocation hover_impl(QPoint) override
    {
        return {};
    }

    [[nodiscard]] static DropLocation dropLocationForPos(QPoint)
    {
        return {};
    }

protected:
    [[nodiscard]] QPoint posForIndicator(DropLocation) const override
    {
        return {};
    }
};

}

#endif
