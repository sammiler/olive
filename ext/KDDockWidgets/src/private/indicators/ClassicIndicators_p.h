#ifndef KD_INDICATORS_CLASSICINDICATORS_P_H
#define KD_INDICATORS_CLASSICINDICATORS_P_H

#include "../DropIndicatorOverlayInterface_p.h"

namespace KDDockWidgets {

class IndicatorWindow;
class Indicator;

class DOCKS_EXPORT ClassicIndicators : public DropIndicatorOverlayInterface
{
    Q_OBJECT

    // Properties for QML
    Q_PROPERTY(bool innerLeftIndicatorVisible READ innerLeftIndicatorVisible NOTIFY indicatorsVisibleChanged)
    Q_PROPERTY(bool innerRightIndicatorVisible READ innerRightIndicatorVisible NOTIFY indicatorsVisibleChanged)
    Q_PROPERTY(bool innerTopIndicatorVisible READ innerTopIndicatorVisible NOTIFY indicatorsVisibleChanged)
    Q_PROPERTY(bool innerBottomIndicatorVisible READ innerBottomIndicatorVisible NOTIFY indicatorsVisibleChanged)

    Q_PROPERTY(bool outterLeftIndicatorVisible READ outterLeftIndicatorVisible NOTIFY indicatorsVisibleChanged)
    Q_PROPERTY(bool outterRightIndicatorVisible READ outterRightIndicatorVisible NOTIFY indicatorsVisibleChanged)
    Q_PROPERTY(bool outterTopIndicatorVisible READ outterTopIndicatorVisible NOTIFY indicatorsVisibleChanged)
    Q_PROPERTY(bool outterBottomIndicatorVisible READ outterBottomIndicatorVisible NOTIFY indicatorsVisibleChanged)

    Q_PROPERTY(bool tabIndicatorVisible READ tabIndicatorVisible NOTIFY indicatorsVisibleChanged)

public:
    explicit ClassicIndicators(DropArea *dropArea);
    ~ClassicIndicators() override;
    DropLocation hover_impl(QPoint globalPos) override;
    [[nodiscard]] QPoint posForIndicator(DropLocation) const override;

    // Lots of getters needed because of QML:
    [[nodiscard]] bool innerLeftIndicatorVisible() const;
    [[nodiscard]] bool innerRightIndicatorVisible() const;
    [[nodiscard]] bool innerTopIndicatorVisible() const;
    [[nodiscard]] bool innerBottomIndicatorVisible() const;
    [[nodiscard]] bool outterLeftIndicatorVisible() const;
    [[nodiscard]] bool outterRightIndicatorVisible() const;
    [[nodiscard]] bool outterTopIndicatorVisible() const;
    [[nodiscard]] bool outterBottomIndicatorVisible() const;
    [[nodiscard]] bool tabIndicatorVisible() const;

protected:
    bool onResize(QSize newSize) override;
    void updateVisibility() override;
Q_SIGNALS:
    void indicatorsVisibleChanged();

private:
    friend class KDDockWidgets::Indicator;
    friend class KDDockWidgets::IndicatorWindow;
    [[nodiscard]] static bool rubberBandIsTopLevel();
    void raiseIndicators();
    [[nodiscard]] QRect geometryForRubberband(QRect localRect) const;
    void setDropLocation(DropLocation);
    void updateWindowPosition();

    QWidgetOrQuick *const m_rubberBand;
    IndicatorWindow *const m_indicatorWindow;
};

}

#endif
