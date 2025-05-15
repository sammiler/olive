#ifndef CINEFORMSECTION_H
#define CINEFORMSECTION_H

#include <QComboBox>

#include "codecsection.h"

namespace olive {

class CineformSection : public CodecSection {
  Q_OBJECT
 public:
  explicit CineformSection(QWidget *parent = nullptr);

  void AddOpts(EncodingParams *params) override;

  void SetOpts(const EncodingParams *p) override;

 private:
  QComboBox *quality_combobox_;
};

}  // namespace olive

#endif  // CINEFORMSECTION_H
