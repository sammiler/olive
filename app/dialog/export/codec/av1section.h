#ifndef AV1SECTION_H
#define AV1SECTION_H

#include <QComboBox>     // 下拉选择框控件
#include <QSlider>       // 滑块控件
#include <QStackedWidget> // 堆叠控件，用于根据压缩方法显示不同UI
#include <QWidget>       // QWidget 基类

// 假设 codecsection.h 声明了 CodecSection 基类和 EncodingParams 类
#include "codecsection.h"
// 假设 floatslider.h 声明了 FloatSlider 类 (虽然在此头文件中未直接使用 FloatSlider，但在 AV1CRFSection 的 .cpp 中可能使用)
#include "widget/slider/floatslider.h"
// #include "common/define.h" // 如果需要 common/define.h 中的内容

namespace olive {

/**
 * @brief 用于配置 AV1 编码器固定码率因子 (CRF) 的控件区域。
 *
 * 此类封装了一个 QSlider，允许用户选择 AV1 编码的 CRF 值。
 * CRF 值越低，质量越高，文件越大；值越高，压缩率越高，质量越低。
 */
class AV1CRFSection : public QWidget {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 AV1CRFSection 对象。
   * @param default_crf CRF 滑块的默认初始值。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit AV1CRFSection(int default_crf, QWidget* parent = nullptr);

  // 默认析构函数通常足够
  // ~AV1CRFSection() override;

  /**
   * @brief 获取当前 CRF 滑块设置的值。
   * @return int 当前选中的 CRF 值。
   */
  [[nodiscard]] int GetValue() const;

  /**
   * @brief AV1 编码的默认 CRF (固定码率因子) 值。
   * 通常设置为一个在质量和文件大小之间取得较好平衡的值。
   */
  static const int kDefaultAV1CRF = 30;

 private:
  /**
   * @brief CRF 值的允许最小值。对于 AV1，通常是 0。
   */
  static const int kMinimumCRF = 0;
  /**
   * @brief CRF 值的允许最大值。对于 AV1，通常是 63。
   */
  static const int kMaximumCRF = 63;

  /**
   * @brief 指向 QSlider 控件的指针，用于让用户选择 CRF 值。
   */
  QSlider* crf_slider_;
};

/**
 * @brief 用于在导出对话框中配置 AV1 编解码器特定选项的区域。
 *
 * 此类继承自 CodecSection，提供了设置 AV1 编码参数的用户界面元素，
 * 例如压缩方法（目前仅支持 CRF）、CRF 值以及编码预设。
 */
class AV1Section : public CodecSection {
  Q_OBJECT
 public:
  /**
   * @brief AV1 编码的压缩方法枚举。
   */
  enum CompressionMethod {
    kConstantRateFactor, ///< @brief 固定码率因子 (CRF) 模式。这是 AV1 常用的高质量可变码率模式。
    // kConstantQuantizer, // 例如：恒定QP模式 (如果未来支持)
    // kAverageBitrate,    // 例如：平均码率模式 (如果未来支持)
  };

  /**
   * @brief 构造一个新的 AV1Section 对象，使用默认的 CRF 值。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit AV1Section(QWidget* parent = nullptr);

  /**
   * @brief 使用指定的默认 CRF 值构造一个新的 AV1Section 对象。
   * @param default_crf CRF 设置区域的默认初始 CRF 值。
   * @param parent 父 QWidget 对象指针。
   */
  AV1Section(int default_crf, QWidget* parent);

  // 默认析构函数通常足够
  // ~AV1Section() override;

  /**
   * @brief 将此 AV1Section 中的用户设置添加到 EncodingParams 对象中。
   * 此方法被导出流程调用，以收集所有编码相关的参数。
   * @param params 指向 EncodingParams 对象的指针，AV1 特定的编码选项将被写入此对象。
   */
  void AddOpts(EncodingParams* params) override;

 private:
  /**
   * @brief 指向 QStackedWidget 的指针，用于根据选择的压缩方法显示不同的UI控件组。
   * 例如，如果选择 CRF 模式，则显示 CRF相关的控件。
   */
  QStackedWidget* compression_method_stack_;

  /**
   * @brief 指向 AV1CRFSection 控件的指针，用于设置 CRF 值。
   */
  AV1CRFSection* crf_section_;

  /**
   * @brief 指向 QComboBox 的指针，用于让用户选择 AV1 编码预设（例如速度与压缩效率的权衡）。
   */
  QComboBox* preset_combobox_;
};

}  // namespace olive

#endif  // AV1SECTION_H