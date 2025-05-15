#include "cineformsection.h"

#include <QGridLayout>
#include <QLabel>

namespace olive {

CineformSection::CineformSection(QWidget *parent) : CodecSection(parent) {
  auto *layout = new QGridLayout(this);

  layout->setContentsMargins(0, 0, 0, 0);

  int row = 0;

  layout->addWidget(new QLabel(tr("Quality:")), row, 0);

  quality_combobox_ = new QComboBox();

  /* Correspond to the following indexes for FFmpeg
   *
   *   -quality           <int>        E..V....... set quality (from 0 to 12) (default film3+)
   *     film3+          0            E..V.......
   *     film3           1            E..V.......
   *     film2+          2            E..V.......
   *     film2           3            E..V.......
   *     film1.5         4            E..V.......
   *     film1+          5            E..V.......
   *     film1           6            E..V.......
   *     high+           7            E..V.......
   *     high            8            E..V.......
   *     medium+         9            E..V.......
   *     medium          10           E..V.......
   *     low+            11           E..V.......
   *     low             12           E..V.......
   *
   */

  quality_combobox_->addItem(tr("Film Scan 3+"));
  quality_combobox_->addItem(tr("Film Scan 3"));
  quality_combobox_->addItem(tr("Film Scan 2+"));
  quality_combobox_->addItem(tr("Film Scan 2"));
  quality_combobox_->addItem(tr("Film Scan 1.5"));
  quality_combobox_->addItem(tr("Film Scan 1+"));
  quality_combobox_->addItem(tr("Film Scan 1"));
  quality_combobox_->addItem(tr("High+"));
  quality_combobox_->addItem(tr("High"));
  quality_combobox_->addItem(tr("Medium+"));
  quality_combobox_->addItem(tr("Medium"));
  quality_combobox_->addItem(tr("Low+"));
  quality_combobox_->addItem(tr("Low"));

  // Default to "medium"
  quality_combobox_->setCurrentIndex(10);

  layout->addWidget(quality_combobox_, row, 1);
}

void CineformSection::AddOpts(EncodingParams *params) {
  params->set_video_option(QStringLiteral("quality"), QString::number(quality_combobox_->currentIndex()));
}

void CineformSection::SetOpts(const EncodingParams *p) {
  quality_combobox_->setCurrentIndex(p->video_option(QStringLiteral("quality")).toInt());
}

}  // namespace olive
