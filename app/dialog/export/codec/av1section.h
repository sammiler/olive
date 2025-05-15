

#ifndef AV1SECTION_H
#define AV1SECTION_H

#include <QComboBox>
#include <QSlider>
#include <QStackedWidget>

#include "codecsection.h"
#include "widget/slider/floatslider.h"

namespace olive {

class AV1CRFSection : public QWidget {
  Q_OBJECT
 public:
  explicit AV1CRFSection(int default_crf, QWidget* parent = nullptr);

  [[nodiscard]] int GetValue() const;

  static const int kDefaultAV1CRF = 30;

 private:
  static const int kMinimumCRF = 0;
  static const int kMaximumCRF = 63;

  QSlider* crf_slider_;
};

class AV1Section : public CodecSection {
  Q_OBJECT
 public:
  enum CompressionMethod {
    kConstantRateFactor,
  };

  explicit AV1Section(QWidget* parent = nullptr);
  AV1Section(int default_crf, QWidget* parent);

  void AddOpts(EncodingParams* params) override;

 private:
  QStackedWidget* compression_method_stack_;

  AV1CRFSection* crf_section_;

  QComboBox* preset_combobox_;
};

}  // namespace olive

#endif  // AV1SECTION_H
