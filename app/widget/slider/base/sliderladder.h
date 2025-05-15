#ifndef SLIDERLADDER_H
#define SLIDERLADDER_H

#include <QLabel>
#include <QTimer>
#include <QWidget>

#include "common/define.h"

namespace olive {

class SliderLadderElement : public QWidget {
  Q_OBJECT
 public:
  SliderLadderElement(const double& multiplier, const QString& width_hint, QWidget* parent = nullptr);

  void SetHighlighted(bool e);

  void SetValue(const QString& value);

  void SetMultiplierVisible(bool e);

  [[nodiscard]] double GetMultiplier() const { return multiplier_; }

 private:
  void UpdateLabel();

  QLabel* label_;

  double multiplier_;
  QString value_;

  bool highlighted_;

  bool multiplier_visible_;
};

class SliderLadder : public QFrame {
  Q_OBJECT
 public:
  SliderLadder(double drag_multiplier, int nb_outer_values, const QString& width_hint, QWidget* parent = nullptr);

  ~SliderLadder() override;

  void SetValue(const QString& s);

  void StartListeningToMouseInput();

 protected:
  void mouseReleaseEvent(QMouseEvent* event) override;

  void closeEvent(QCloseEvent* event) override;

 signals:
  void DraggedByValue(int value, double multiplier);

  void Released();

 private:
  [[nodiscard]] bool UsingLadders() const;

  int drag_start_x_;
  int drag_start_y_;
  int wrap_count_;

  QList<SliderLadderElement*> elements_;

  int active_element_;

  QTimer drag_timer_;

  QScreen* screen_;

 private slots:
  void TimerUpdate();
};

}  // namespace olive

#endif  // SLIDERLADDER_H
