

#ifndef COLORPREVIEWBOX_H
#define COLORPREVIEWBOX_H

#include <QWidget>

#include "render/colorprocessor.h"

namespace olive {

class ColorPreviewBox : public QWidget {
  Q_OBJECT
 public:
  explicit ColorPreviewBox(QWidget* parent = nullptr);

  void SetColorProcessor(ColorProcessorPtr to_ref, ColorProcessorPtr to_display);

 public slots:
  void SetColor(const Color& c);

 protected:
  void paintEvent(QPaintEvent* e) override;

 private:
  Color color_;

  ColorProcessorPtr to_ref_processor_;

  ColorProcessorPtr to_display_processor_;
};

}  // namespace olive

#endif  // COLORPREVIEWBOX_H
