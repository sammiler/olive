

#ifndef IMAGESECTION_H
#define IMAGESECTION_H

#include <QCheckBox>

#include "codecsection.h"
#include "widget/slider/rationalslider.h"

namespace olive {

class ImageSection : public CodecSection {
  Q_OBJECT
 public:
  explicit ImageSection(QWidget* parent = nullptr);

  [[nodiscard]] bool IsImageSequenceChecked() const { return image_sequence_checkbox_->isChecked(); }

  void SetImageSequenceChecked(bool e) { image_sequence_checkbox_->setChecked(e); }

  void SetTimebase(const rational& r) { frame_slider_->SetTimebase(r); }

  [[nodiscard]] rational GetTime() const { return frame_slider_->GetValue(); }

  void SetTime(const rational& t) { frame_slider_->SetValue(t); }

 signals:
  void TimeChanged(const rational& t);

 private:
  QCheckBox* image_sequence_checkbox_;

  RationalSlider* frame_slider_;

 private slots:
  void ImageSequenceCheckBoxToggled(bool e);
};

}  // namespace olive

#endif  // IMAGESECTION_H
