

#ifndef COLORSWATCHWIDGET_H
#define COLORSWATCHWIDGET_H

#include <QOpenGLWidget>

#include "render/colorprocessor.h"

namespace olive {

class ColorSwatchWidget : public QWidget {
  Q_OBJECT
 public:
  explicit ColorSwatchWidget(QWidget* parent = nullptr);

  [[nodiscard]] const Color& GetSelectedColor() const;

  void SetColorProcessor(ColorProcessorPtr to_linear, ColorProcessorPtr to_display);

 public slots:
  void SetSelectedColor(const Color& c);

 signals:
  void SelectedColorChanged(const Color& c);

 protected:
  void mousePressEvent(QMouseEvent* e) override;

  void mouseMoveEvent(QMouseEvent* e) override;

  [[nodiscard]] virtual Color GetColorFromScreenPos(const QPoint& p) const = 0;

  virtual void SelectedColorChangedEvent(const Color& c, bool external);

  [[nodiscard]] Qt::GlobalColor GetUISelectorColor() const;

  [[nodiscard]] Color GetManagedColor(const Color& input) const;

 private:
  void SetSelectedColorInternal(const Color& c, bool external);

  Color selected_color_;

  ColorProcessorPtr to_linear_processor_;

  ColorProcessorPtr to_display_processor_;
};

}  // namespace olive

#endif  // COLORSWATCHWIDGET_H
