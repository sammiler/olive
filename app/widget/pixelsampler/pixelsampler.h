

#ifndef PIXELSAMPLERWIDGET_H
#define PIXELSAMPLERWIDGET_H

#include <QGroupBox>
#include <QLabel>
#include <QWidget>

#include "widget/colorwheel/colorpreviewbox.h"

namespace olive {

class PixelSamplerWidget : public QGroupBox {
  Q_OBJECT
 public:
  explicit PixelSamplerWidget(QWidget* parent = nullptr);

 public slots:
  void SetValues(const Color& color);

 private:
  void UpdateLabelInternal();

  Color color_;

  ColorPreviewBox* box_;

  QLabel* label_;
};

class ManagedPixelSamplerWidget : public QWidget {
  Q_OBJECT
 public:
  explicit ManagedPixelSamplerWidget(QWidget* parent = nullptr);

 public slots:
  void SetValues(const Color& reference, const Color& display);

 private:
  PixelSamplerWidget* reference_view_;

  PixelSamplerWidget* display_view_;
};

}  // namespace olive

#endif  // PIXELSAMPLERWIDGET_H
