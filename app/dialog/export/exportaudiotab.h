#ifndef EXPORTAUDIOTAB_H
#define EXPORTAUDIOTAB_H

#include <QComboBox>  // 下拉选择框控件
#include <QVariant>   // 为了 currentData().toInt()
#include <QWidget>    // QWidget 基类

// Olive 内部头文件
#include "codec/exportformat.h"  // 包含 ExportFormat::Format 和 ExportCodec::Codec (假设 ExportCodec 也在此或其包含文件)
#include "common/define.h"  // 可能包含 SampleRateComboBox, SampleFormatComboBox, ChannelLayoutComboBox, IntegerSlider 的基类或相关定义
#include "widget/slider/integerslider.h"           // 整数滑块控件，用于码率
#include "widget/standardcombos/standardcombos.h"  // 包含标准化的下拉框，如 SampleRateComboBox 等

// 前向声明 (如果需要)
// namespace olive {
// class SampleRateComboBox;
// class SampleFormatComboBox;
// class ChannelLayoutComboBox;
// class IntegerSlider;
// }

namespace olive {

/**
 * @brief 导出对话框中用于配置音频导出选项的选项卡页面。
 *
 * 此类继承自 QWidget (通常会作为 CodecSection 或类似基类的派生类，但这里直接是 QWidget)，
 * 提供了设置音频编码参数的用户界面元素，例如：
 * - 音频编解码器 (Codec)
 * - 采样率 (Sample Rate)
 * - 采样格式 (Sample Format)
 * - 通道布局 (Channel Layout)
 * - 音频码率 (Bit Rate)
 */
class ExportAudioTab : public QWidget {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 ExportAudioTab 对象。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit ExportAudioTab(QWidget* parent = nullptr);

  // ~ExportAudioTab() override; // 默认析构函数通常足够

  /**
   * @brief 获取用户在编解码器下拉框中当前选中的音频编解码器。
   * @return ExportCodec::Codec 选中的音频编解码器枚举值。
   */
  [[nodiscard]] ExportCodec::Codec GetCodec() const {
    // 假设 QComboBox 的 itemData 存储的是 ExportCodec::Codec 枚举值对应的整数
    return static_cast<ExportCodec::Codec>(codec_combobox_->currentData().toInt());
  }

  /**
   * @brief 设置编解码器下拉框的当前选中项。
   * @param c 要选中的 ExportCodec::Codec 枚举值。
   */
  void SetCodec(ExportCodec::Codec c) {
    for (int i = 0; i < codec_combobox_->count(); i++) {
      if (static_cast<ExportCodec::Codec>(codec_combobox_->itemData(i).toInt()) == c) {  // 确保比较时也进行类型转换
        codec_combobox_->setCurrentIndex(i);
        break;
      }
    }
  }

  /**
   * @brief 获取指向采样率选择下拉框的指针。
   * @return SampleRateComboBox* 指向采样率下拉框的指针。
   */
  [[nodiscard]] SampleRateComboBox* sample_rate_combobox() const { return sample_rate_combobox_; }

  /**
   * @brief 获取指向采样格式选择下拉框的指针。
   * @return SampleFormatComboBox* 指向采样格式下拉框的指针。
   */
  [[nodiscard]] SampleFormatComboBox* sample_format_combobox() const { return sample_format_combobox_; }

  /**
   * @brief 获取指向通道布局选择下拉框的指针。
   * @return ChannelLayoutComboBox* 指向通道布局下拉框的指针。
   */
  [[nodiscard]] ChannelLayoutComboBox* channel_layout_combobox() const { return channel_layout_combobox_; }

  /**
   * @brief 获取指向音频码率设置滑块的指针。
   * @return IntegerSlider* 指向码率滑块的指针。
   */
  [[nodiscard]] IntegerSlider* bit_rate_slider() const { return bit_rate_slider_; }

 public slots:
  /**
   * @brief 根据指定的导出文件封装格式 (Format) 来更新此音频选项卡中可用控件（如编解码器列表）。
   *
   * 不同的封装格式支持不同的音频编解码器。此函数会根据传入的 `format`
   * 筛选并更新 `codec_combobox_` 中可用的音频编解码器选项。
   * @param format 当前选定的导出文件封装格式。
   * @return int 通常返回编解码器下拉框中新选中的索引，或者一个状态码。
   * （返回类型为 int，其具体含义需参照实现，可能表示选中项的索引或某种状态）。
   */
  int SetFormat(ExportFormat::Format format);

 private:
  /**
   * @brief 存储当前设置的导出文件封装格式。
   * 用于在格式变化时更新可用的编解码器等。
   */
  ExportFormat::Format fmt_;
  /**
   * @brief 指向 QComboBox 控件的指针，用于选择音频编解码器。
   */
  QComboBox* codec_combobox_;
  /**
   * @brief 指向 SampleRateComboBox (自定义下拉框) 的指针，用于选择音频采样率。
   */
  SampleRateComboBox* sample_rate_combobox_;
  /**
   * @brief 指向 ChannelLayoutComboBox (自定义下拉框) 的指针，用于选择音频通道布局。
   */
  ChannelLayoutComboBox* channel_layout_combobox_;
  /**
   * @brief 指向 SampleFormatComboBox (自定义下拉框) 的指针，用于选择音频采样格式。
   */
  SampleFormatComboBox* sample_format_combobox_;
  /**
   * @brief 指向 IntegerSlider 控件的指针，用于设置音频码率。
   */
  IntegerSlider* bit_rate_slider_;

  /**
   * @brief 音频编码的默认码率值 (单位可能需要参考 .cpp 实现，例如 kbps)。
   */
  static const int kDefaultBitRate;  // 具体值在 .cpp 文件中定义

 private slots:
  /**
   * @brief 当选定的音频编解码器或封装格式发生变化时，更新采样格式下拉框中可用的选项。
   * 不同的编解码器和格式组合可能支持不同的采样格式。
   */
  void UpdateSampleFormats();

  /**
   * @brief 当选定的音频编解码器发生变化时，更新码率滑块的启用/禁用状态。
   * 某些音频编解码器（如PCM、FLAC这类无损格式）可能不需要或不支持设置码率，
   * 此时码率滑块应被禁用。
   */
  void UpdateBitRateEnabled();
};

}  // namespace olive

#endif  // EXPORTAUDIOTAB_H