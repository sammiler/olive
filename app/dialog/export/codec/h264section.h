#ifndef H264SECTION_H
#define H264SECTION_H

#include <QComboBox>     // 下拉选择框控件
#include <QSlider>       // 滑块控件
#include <QStackedWidget> // 堆叠控件，用于根据压缩方法显示不同UI
#include <QWidget>       // QWidget 基类
#include <cstdint>       // 为了 int64_t

// 假设 codecsection.h 声明了 CodecSection 基类和 EncodingParams 类
#include "codecsection.h"
// 假设 floatslider.h 声明了 FloatSlider 类
#include "widget/slider/floatslider.h"
// #include "common/define.h" // 如果需要 common/define.h 中的内容

namespace olive {

/**
 * @brief 用于配置 H.264 (及类似编解码器如 H.265) 固定码率因子 (CRF) 的控件区域。
 *
 * 此类封装了一个 QSlider，允许用户选择编码的 CRF 值。
 * CRF 值越低，通常意味着质量越高，文件越大；值越高，压缩率越高，质量越低。
 */
class H264CRFSection : public QWidget {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 H264CRFSection 对象。
   * @param default_crf CRF 滑块的默认初始值。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit H264CRFSection(int default_crf, QWidget* parent = nullptr);

  // ~H264CRFSection() override; // 默认析构通常足够

  /**
   * @brief 获取当前 CRF 滑块设置的值。
   * @return int 当前选中的 CRF 值。
   */
  [[nodiscard]] int GetValue() const;

  /**
   * @brief 设置 CRF 滑块的值。
   * @param c 要设置的 CRF 值。
   */
  void SetValue(int c);

  /**
   * @brief H.264 编码的推荐默认 CRF (固定码率因子) 值。
   */
  static const int kDefaultH264CRF = 18;
  /**
   * @brief H.265 (HEVC) 编码的推荐默认 CRF (固定码率因子) 值。
   */
  static const int kDefaultH265CRF = 23;

 private:
  /**
   * @brief CRF 值的允许最小值。对于 H.264/H.265，通常是 0 (无损或接近无损)。
   */
  static const int kMinimumCRF = 0;
  /**
   * @brief CRF 值的允许最大值。对于 H.264/H.265，通常是 51。
   */
  static const int kMaximumCRF = 51;

  /**
   * @brief 指向 QSlider 控件的指针，用于让用户选择 CRF 值。
   */
  QSlider* crf_slider_;
};

/**
 * @brief 用于配置 H.264 (及类似编解码器) 目标码率和最大码率的控件区域。
 *
 * 此类包含两个 FloatSlider 控件，分别用于设置目标平均码率和允许的最大码率。
 * 单位通常是 kbps (千比特每秒) 或 mbps (兆比特每秒)，返回时转换为比特每秒 (BITS)。
 */
class H264BitRateSection : public QWidget {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 H264BitRateSection 对象。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit H264BitRateSection(QWidget* parent = nullptr);

  // ~H264BitRateSection() override; // 默认析构通常足够

  /**
   * @brief 获取用户选定的目标码率。
   * @return int64_t 目标码率，单位为比特每秒 (bits per second)。
   */
  [[nodiscard]] int64_t GetTargetBitRate() const;

  /**
   * @brief 设置目标码率UI控件的值。
   * @param b 目标码率，单位为比特每秒。
   */
  void SetTargetBitRate(int64_t b);

  /**
   * @brief 获取用户选定的最大码率。
   * @return int64_t 最大码率，单位为比特每秒 (bits per second)。
   */
  [[nodiscard]] int64_t GetMaximumBitRate() const;

  /**
   * @brief 设置最大码率UI控件的值。
   * @param b 最大码率，单位为比特每秒。
   */
  void SetMaximumBitRate(int64_t b);

 private:
  /**
   * @brief 指向 FloatSlider 控件的指针，用于设置目标码率。
   */
  FloatSlider* target_rate_;

  /**
   * @brief 指向 FloatSlider 控件的指针，用于设置最大码率。
   */
  FloatSlider* max_rate_;
};

/**
 * @brief 用于配置 H.264 (及类似编解码器) 目标文件大小的控件区域。
 *
 * 此类包含一个 FloatSlider 控件，允许用户指定期望的输出文件大小。
 * 编码器将尝试调整码率以达到此目标文件大小。单位返回为比特 (BITS)。
 */
class H264FileSizeSection : public QWidget {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 H264FileSizeSection 对象。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit H264FileSizeSection(QWidget* parent = nullptr);

  // ~H264FileSizeSection() override; // 默认析构通常足够

  /**
   * @brief 获取用户选定的目标文件大小。
   * @return int64_t 目标文件大小，单位为比特 (bits)。
   */
  [[nodiscard]] int64_t GetFileSize() const;

  /**
   * @brief 设置目标文件大小UI控件的值。
   * @param f 目标文件大小，单位为比特。
   */
  void SetFileSize(int64_t f);

 private:
  /**
   * @brief 指向 FloatSlider 控件的指针，用于设置目标文件大小。
   */
  FloatSlider* file_size_;
};

/**
 * @brief 用于在导出对话框中配置 H.264 编解码器特定选项的区域。
 *
 * 此类继承自 CodecSection，提供了设置 H.264 编码参数的用户界面元素，
 * 包括不同的压缩方法（如CRF、目标码率、目标文件大小）及其对应的设置控件，
 * 以及编码预设的选择。
 */
class H264Section : public CodecSection {
  Q_OBJECT
 public:
  /**
   * @brief H.264 编码的压缩方法枚举。
   */
  enum CompressionMethod {
    kConstantRateFactor, ///< @brief 固定码率因子 (CRF) 模式：追求特定质量水平，码率可变。
    kTargetBitRate,      ///< @brief 目标码率模式：尝试达到指定的目标平均码率，可能还需设置最大码率。
    kTargetFileSize      ///< @brief 目标文件大小模式：尝试使输出文件达到指定的大小。
  };

  /**
   * @brief 构造一个新的 H264Section 对象，使用默认的 CRF 值。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit H264Section(QWidget* parent = nullptr);

  /**
   * @brief 使用指定的默认 CRF 值构造一个新的 H264Section 对象。
   * @param default_crf CRF 设置区域的默认初始 CRF 值 (通常是 H264CRFSection::kDefaultH264CRF)。
   * @param parent 父 QWidget 对象指针。
   */
  H264Section(int default_crf, QWidget* parent);

  // ~H264Section() override; // 默认析构通常足够

  /**
   * @brief 将此 H264Section 中的用户设置添加到 EncodingParams 对象中。
   * 此方法被导出流程调用，以收集所有编码相关的参数。
   * @param params 指向 EncodingParams 对象的指针，H.264 特定的编码选项将被写入此对象。
   */
  void AddOpts(EncodingParams* params) override;

  /**
   * @brief 根据提供的 EncodingParams 对象中的设置来配置此 H264Section 的UI控件。
   * 例如，如果加载了包含 H.264 设置的预设，则会调用此方法来更新界面显示。
   * @param p 指向包含要应用的编码参数的 EncodingParams 对象的常量指针。
   */
  void SetOpts(const EncodingParams* p) override;

 private:
  /**
   * @brief 指向 QStackedWidget 的指针，用于根据选择的压缩方法显示不同的UI控件组。
   */
  QStackedWidget* compression_method_stack_;

  /**
   * @brief 指向 H264CRFSection 控件的指针，用于 CRF 模式下的设置。
   */
  H264CRFSection* crf_section_;

  /**
   * @brief 指向 H264BitRateSection 控件的指针，用于目标码率模式下的设置。
   */
  H264BitRateSection* bitrate_section_;

  /**
   * @brief 指向 H264FileSizeSection 控件的指针，用于目标文件大小模式下的设置。
   */
  H264FileSizeSection* filesize_section_;

  /**
   * @brief 指向 QComboBox 的指针，用于让用户选择 H.264 编码预设
   * (例如 ultrafast, medium, slow，影响编码速度和压缩效率)。
   */
  QComboBox* preset_combobox_;
};

/**
 * @brief 用于在导出对话框中配置 H.265 (HEVC) 编解码器特定选项的区域。
 *
 * 此类继承自 H264Section，表明 H.265 的许多编码选项和UI结构与 H.264 类似，
 * 可能复用了大部分 H264Section 的实现，并针对 H.265 的特性（如不同的默认CRF值）进行调整。
 */
class H265Section : public H264Section {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 H265Section 对象。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit H265Section(QWidget* parent = nullptr);
  // ~H265Section() override; // 默认析构通常足够
};

}  // namespace olive

#endif  // H264SECTION_H