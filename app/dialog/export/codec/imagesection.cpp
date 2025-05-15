

#include "imagesection.h"

#include <QGridLayout>
#include <QLabel>

namespace olive {

ImageSection::ImageSection(QWidget* parent) : CodecSection(parent) {
  auto* layout = new QGridLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);

  int row = 0;

  layout->addWidget(new QLabel(tr("Image Sequence:")), row, 0);

  image_sequence_checkbox_ = new QCheckBox();
  connect(image_sequence_checkbox_, &QCheckBox::toggled, this, &ImageSection::ImageSequenceCheckBoxToggled);
  layout->addWidget(image_sequence_checkbox_, row, 1);

  row++;

  layout->addWidget(new QLabel(tr("Frame to Export:")), row, 0);

  frame_slider_ = new RationalSlider();
  frame_slider_->SetMinimum(rational(0));
  frame_slider_->SetValue(rational(0));
  frame_slider_->SetDisplayType(RationalSlider::kTime);
  connect(frame_slider_, &RationalSlider::ValueChanged, this, &ImageSection::TimeChanged);
  layout->addWidget(frame_slider_, row, 1);
}

void ImageSection::ImageSequenceCheckBoxToggled(bool e) { frame_slider_->setEnabled(!e); }

}  // namespace olive
