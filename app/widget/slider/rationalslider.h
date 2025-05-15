#ifndef RATIONALSLIDER_H
#define RATIONALSLIDER_H

#include <olive/core/core.h>
#include <QMouseEvent>

#include "base/decimalsliderbase.h"

namespace olive {

using namespace core;

/**
 * @brief A olive::rational based slider
 *
 * A slider that can display rationals as either timecode (drop or non-drop), a timestamp (frames),
 * or a float (seconds).
 */
class RationalSlider : public DecimalSliderBase {
  Q_OBJECT
 public:
  /**
   * @brief enum containing the possibly display types
   */
  enum DisplayType { kTime, kFloat, kRational };

  explicit RationalSlider(QWidget* parent = nullptr);

  /**
   * @brief Returns the sliders value as a rational
   */
  rational GetValue();

  /**
   * @brief Sets the sliders default value
   */
  void SetDefaultValue(const rational& r);

  /**
   * @brief Sets the sliders minimum value
   */
  void SetMinimum(const rational& d);

  /**
   * @brief Sets the sliders maximum value
   */
  void SetMaximum(const rational& d);

  /**
   * @brief Sets the display type of the slider
   */
  void SetDisplayType(const DisplayType& type);

  /**
   * @brief Set whether the user can change the display type or not
   */
  void SetLockDisplayType(bool e);

  /**
   * @brief Get whether the user can change the display type or not
   */
  [[nodiscard]] bool GetLockDisplayType() const;

  /**
   * @brief Hide display type in menu
   */
  void DisableDisplayType(DisplayType type);

 public slots:
  /**
   * @brief Sets the sliders timebase which is also the minimum increment of the slider
   */
  void SetTimebase(const rational& timebase);

  /**
   * @brief Sets the sliders value
   */
  void SetValue(const rational& d);

 protected:
  [[nodiscard]] QString ValueToString(const QVariant& v) const override;

  QVariant StringToValue(const QString& s, bool* ok) const override;

  [[nodiscard]] QVariant AdjustDragDistanceInternal(const QVariant& start, const double& drag) const override;

  void ValueSignalEvent(const QVariant& v) override;

  [[nodiscard]] bool ValueGreaterThan(const QVariant& lhs, const QVariant& rhs) const override;

  [[nodiscard]] bool ValueLessThan(const QVariant& lhs, const QVariant& rhs) const override;

 signals:
  void ValueChanged(rational);

 private slots:
  void ShowDisplayTypeMenu();

  void SetDisplayTypeFromMenu();

 private:
  DisplayType display_type_;

  rational timebase_;

  bool lock_display_type_;

  QVector<DisplayType> disabled_;
};

}  // namespace olive

#endif  // RATIONALSLIDER_H
