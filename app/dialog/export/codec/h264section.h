#ifndef H264SECTION_H
#define H264SECTION_H

#include <QComboBox>
#include <QSlider>
#include <QStackedWidget>

#include "codecsection.h"
#include "widget/slider/floatslider.h"

namespace olive {

class H264CRFSection : public QWidget {
  Q_OBJECT
 public:
  explicit H264CRFSection(int default_crf, QWidget* parent = nullptr);

  [[nodiscard]] int GetValue() const;
  void SetValue(int c);

  static const int kDefaultH264CRF = 18;
  static const int kDefaultH265CRF = 23;

 private:
  static const int kMinimumCRF = 0;
  static const int kMaximumCRF = 51;

  QSlider* crf_slider_;
};

class H264BitRateSection : public QWidget {
  Q_OBJECT
 public:
  explicit H264BitRateSection(QWidget* parent = nullptr);

  /**
   * @brief Get user-selected target bit rate (returns in BITS)
   */
  [[nodiscard]] int64_t GetTargetBitRate() const;
  void SetTargetBitRate(int64_t b);

  /**
   * @brief Get user-selected maximum bit rate (returns in BITS)
   */
  [[nodiscard]] int64_t GetMaximumBitRate() const;
  void SetMaximumBitRate(int64_t b);

 private:
  FloatSlider* target_rate_;

  FloatSlider* max_rate_;
};

class H264FileSizeSection : public QWidget {
  Q_OBJECT
 public:
  explicit H264FileSizeSection(QWidget* parent = nullptr);

  /**
   * @brief Returns file size in BITS
   */
  [[nodiscard]] int64_t GetFileSize() const;
  void SetFileSize(int64_t f);

 private:
  FloatSlider* file_size_;
};

class H264Section : public CodecSection {
  Q_OBJECT
 public:
  enum CompressionMethod { kConstantRateFactor, kTargetBitRate, kTargetFileSize };

  explicit H264Section(QWidget* parent = nullptr);
  H264Section(int default_crf, QWidget* parent);

  void AddOpts(EncodingParams* params) override;

  void SetOpts(const EncodingParams* p) override;

 private:
  QStackedWidget* compression_method_stack_;

  H264CRFSection* crf_section_;

  H264BitRateSection* bitrate_section_;

  H264FileSizeSection* filesize_section_;

  QComboBox* preset_combobox_;
};

class H265Section : public H264Section {
  Q_OBJECT
 public:
  explicit H265Section(QWidget* parent = nullptr);
};

}  // namespace olive

#endif  // H264SECTION_H
