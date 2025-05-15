#ifndef EXPORTVIDEOTAB_H
#define EXPORTVIDEOTAB_H

#include <QCheckBox>     // 复选框控件
#include <QComboBox>     // 下拉选择框控件
#include <QWidget>       // QWidget 基类
#include <QString>       // Qt 字符串类
#include <QStringList>   // Qt 字符串列表 (可能在 .cpp 中使用)
#include <QStackedWidget> // 为了 codec_stack_ (虽然 CodecStack 已经继承了它)
#include <QVariant>      // 为了 currentData().toInt()

// Olive 内部头文件
#include "common/define.h"        // 包含 olive::rational, ColorManager, ExportCodec::Codec, VideoParams::ColorRange 等
#include "common/qtutils.h"       // 包含 QtUtils::SetComboBoxData
#include "codec/exportformat.h"   // 包含 ExportFormat::Format
#include "codec/encoder.h"        // 包含 EncodingParams (虽然此文件在导出对话框主类中也可能被包含)
#include "dialog/export/codec/av1section.h"       // AV1 编码器设置区域
#include "dialog/export/codec/cineformsection.h"  // CineForm 编码器设置区域
#include "dialog/export/codec/codecstack.h"       // 用于管理不同编解码器设置区域的堆叠控件
#include "dialog/export/codec/h264section.h"      // H.264/H.265 编码器设置区域
#include "dialog/export/codec/imagesection.h"     // 图像(序列)导出设置区域
#include "dialog/export/codec/codecsection.h"     // 编解码器设置区域的基类
#include "node/color/colormanager/colormanager.h" // 颜色管理器
#include "widget/colorwheel/colorspacechooser.h"  // 颜色空间选择器控件
#include "widget/slider/integerslider.h"        // 整数滑块控件
#include "widget/standardcombos/standardcombos.h" // 包含标准化的下拉框，如 FrameRateComboBox 等

// 前向声明 (如果需要)
// namespace olive {
// class ColorManager;
// class FrameRateComboBox;
// class ImageSection;
// class H264Section;
// class AV1Section;
// class CineformSection;
// class CodecStack;
// class ColorSpaceChooser;
// class IntegerSlider;
// class InterlacedComboBox;
// class PixelAspectRatioComboBox;
// class PixelFormatComboBox;
// }

namespace olive {

/**
 * @brief 导出对话框中用于配置视频导出选项的选项卡页面。
 *
 * 此类继承自 QWidget (通常会作为 CodecSection 或类似基类的派生类，但这里直接是 QWidget)，
 * 提供了设置视频编码参数的综合用户界面，包括：
 * - 分辨率 (宽度、高度、保持宽高比、缩放方法)
 * - 帧率
 * - 颜色空间 (通过 OCIO)
 * - 视频编解码器选择
 * - 特定于选定编解码器的详细参数 (通过 CodecStack 和各种 CodecSection 实现)
 * - 隔行扫描方式
 * - 像素宽高比
 * - 像素格式
 * - 编码线程数
 * - YUV颜色范围
 * - 是否导出为图像序列及其相关设置
 */
class ExportVideoTab : public QWidget {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 ExportVideoTab 对象。
   * @param color_manager 指向应用程序颜色管理器的指针，用于处理颜色空间相关的设置。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit ExportVideoTab(ColorManager* color_manager, QWidget* parent = nullptr);

  // ~ExportVideoTab() override; // 默认析构函数通常足够

  /**
   * @brief 根据指定的导出文件封装格式 (Format) 来更新此视频选项卡中可用控件（如编解码器列表和编解码器特定设置区域）。
   *
   * @param format 当前选定的导出文件封装格式。
   * @return int 通常返回编解码器下拉框中新选中的索引，或者一个状态码。
   * （返回类型为 int，其具体含义需参照实现）。
   */
  int SetFormat(ExportFormat::Format format);

  /**
   * @brief 检查“图像序列”选项 (通常在 ImageSection 中) 当前是否被选中。
   * @return bool 如果设置为导出图像序列，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool IsImageSequenceSet() const;

  /**
   * @brief 设置“图像序列”选项的状态。
   * @param e 如果为 true，则设置为导出图像序列；否则取消。
   */
  void SetImageSequence(bool e) const; // 通常 setter 不会是 const

  /**
   * @brief 获取当导出为单帧静态图像时，选定的图像的时间点。
   * @return rational 选定的静态图像的时间点。
   */
  [[nodiscard]] rational GetStillImageTime() const { return image_section_->GetTime(); }

  /**
   * @brief 获取用户在编解码器下拉框中当前选中的视频编解码器。
   * @return ExportCodec::Codec 选中的视频编解码器枚举值。
   */
  [[nodiscard]] ExportCodec::Codec GetSelectedCodec() const {
    return static_cast<ExportCodec::Codec>(codec_combobox()->currentData().toInt());
  }

  /**
   * @brief 设置视频编解码器下拉框的当前选中项。
   * @param c 要选中的 ExportCodec::Codec 枚举值。
   */
  void SetSelectedCodec(ExportCodec::Codec c) const { QtUtils::SetComboBoxData(codec_combobox(), static_cast<int>(c)); } // 确保传入int

  /**
   * @brief 获取指向视频编解码器选择下拉框的指针。
   * @return QComboBox* 指向编解码器下拉框的指针。
   */
  [[nodiscard]] QComboBox* codec_combobox() const { return codec_combobox_; }

  /**
   * @brief 获取指向视频宽度设置滑块的指针。
   * @return IntegerSlider* 指向宽度滑块的指针。
   */
  [[nodiscard]] IntegerSlider* width_slider() const { return width_slider_; }

  /**
   * @brief 获取指向视频高度设置滑块的指针。
   * @return IntegerSlider* 指向高度滑块的指针。
   */
  [[nodiscard]] IntegerSlider* height_slider() const { return height_slider_; }

  /**
   * @brief 获取指向“保持宽高比”复选框的指针。
   * @return QCheckBox* 指向复选框的指针。
   */
  [[nodiscard]] QCheckBox* maintain_aspect_checkbox() const { return maintain_aspect_checkbox_; }

  /**
   * @brief 获取指向视频缩放方法选择下拉框的指针。
   * @return QComboBox* 指向缩放方法下拉框的指针。
   */
  [[nodiscard]] QComboBox* scaling_method_combobox() const { return scaling_method_combobox_; }

  /**
   * @brief 获取当前选定的视频帧率。
   * @return rational 选定的帧率。
   */
  [[nodiscard]] rational GetSelectedFrameRate() const { return frame_rate_combobox_->GetFrameRate(); }

  /**
   * @brief 设置视频帧率下拉框的选中项，并更新相关的UI或内部状态。
   * @param fr 要设置的帧率。
   */
  void SetSelectedFrameRate(const rational& fr) {
    frame_rate_combobox_->SetFrameRate(fr);
    UpdateFrameRate(fr); // 内部更新逻辑
  }

  /**
   * @brief 获取当前在颜色空间选择器中选择的输入OCIO颜色空间名称。
   * @return QString OCIO输入颜色空间的名称。
   */
  QString CurrentOCIOColorSpace() { return color_space_chooser_->input(); }

  /**
   * @brief 设置颜色空间选择器中的输入OCIO颜色空间。
   * @param s 要设置的OCIO输入颜色空间的名称。
   */
  void SetOCIOColorSpace(const QString& s) { color_space_chooser_->set_input(s); }

  /**
   * @brief 获取当前在 `codec_stack_` 中显示的编解码器特定设置区域 (CodecSection)。
   * @return CodecSection* 指向当前可见的 CodecSection 派生类对象的指针；如果没有可见的或者类型不匹配，则返回 nullptr。
   */
  [[nodiscard]] CodecSection* GetCodecSection() const {
    return dynamic_cast<CodecSection*>(codec_stack_->currentWidget());
  }

  /**
   * @brief 设置 `codec_stack_` 中当前要显示的编解码器特定设置区域。
   * @param section 指向要显示的 CodecSection 派生类对象的指针。如果为 nullptr，则隐藏 `codec_stack_`。
   */
  void SetCodecSection(CodecSection* section) {
    if (section) {
      codec_stack_->setVisible(true);
      codec_stack_->setCurrentWidget(section);
    } else {
      codec_stack_->setVisible(false);
    }
  }

  /**
   * @brief 获取指向隔行扫描方式选择下拉框的指针。
   * @return InterlacedComboBox* 指向隔行扫描下拉框的指针。
   */
  [[nodiscard]] InterlacedComboBox* interlaced_combobox() const { return interlaced_combobox_; }

  /**
   * @brief 获取指向像素宽高比选择下拉框的指针。
   * @return PixelAspectRatioComboBox* 指向像素宽高比下拉框的指针。
   */
  [[nodiscard]] PixelAspectRatioComboBox* pixel_aspect_combobox() const { return pixel_aspect_combobox_; }

  /**
   * @brief 获取指向像素格式选择下拉框的指针。
   * @return PixelFormatComboBox* 指向像素格式下拉框的指针。
   */
  [[nodiscard]] PixelFormatComboBox* pixel_format_field() const { return pixel_format_field_; }

  /**
   * @brief 获取（高级选项中）用户设置的编码线程数。
   * @return const int& 对线程数的常量引用。
   */
  [[nodiscard]] const int& threads() const { return threads_; }

  /**
   * @brief 设置编码线程数（通常从高级对话框获取）。
   * @param t 线程数量。
   */
  void SetThreads(int t) { threads_ = t; }

  /**
   * @brief 获取（高级选项中）用户选择的输出像素格式名称。
   * @return const QString& 对像素格式名称字符串的常量引用。
   */
  [[nodiscard]] const QString& pix_fmt() const { return pix_fmt_; }
  /**
   * @brief 设置输出像素格式名称（通常从高级对话框获取）。
   * @param s 像素格式的名称字符串。
   */
  void SetPixFmt(const QString& s) { pix_fmt_ = s; }

  /**
   * @brief 获取（高级选项中）用户选择的YUV颜色范围。
   * @return VideoParams::ColorRange YUV颜色范围的枚举值。
   */
  [[nodiscard]] VideoParams::ColorRange color_range() const { return color_range_; }
  /**
   * @brief 设置YUV颜色范围（通常从高级对话框获取）。
   * @param c YUV颜色范围的枚举值。
   */
  void SetColorRange(VideoParams::ColorRange c) { color_range_ = c; }

 public slots:
  /**
   * @brief 当视频编解码器下拉框的选项发生变化时调用的槽函数。
   * 此函数会根据新选中的编解码器，在 `codec_stack_` 中显示对应的设置区域。
   */
  void VideoCodecChanged();

  /**
   * @brief 当导出单帧图像时，用户在 `ImageSection` 中选择的时间点发生变化时调用的槽函数。
   * @param time 新选中的时间点。
   */
  void SetTime(const rational& time); // 应该连接到 ImageSection::TimeChanged 信号

 signals:
  /**
   * @brief 当OCIO颜色空间选择发生变化时发出此信号。
   * @param colorspace 新选择的OCIO颜色空间的名称。
   */
  void ColorSpaceChanged(const QString& colorspace);

  /**
   * @brief 当“图像序列”复选框的状态发生变化时发出此信号。
   * @param e 复选框新的选中状态。
   */
  void ImageSequenceCheckBoxChanged(bool e); // 通常连接到 ImageSection 中的信号或复选框的 toggled() 信号

  /**
   * @brief 当导出单帧图像的时间点选择发生变化时发出此信号 (通常由 ImageSection 发出并中继)。
   * @param time 新的选中时间点。
   */
  void TimeChanged(const rational& time);

 private:
  /**
   * @brief 创建并设置分辨率相关的UI控件区域。
   * @return QWidget* 指向包含分辨率设置控件的父 QWidget 的指针。
   */
  QWidget* SetupResolutionSection();
  /**
   * @brief 创建并设置颜色相关的UI控件区域 (例如OCIO颜色空间选择)。
   * @return QWidget* 指向包含颜色设置控件的父 QWidget 的指针。
   */
  QWidget* SetupColorSection();
  /**
   * @brief 创建并设置编解码器选择及编解码器特定选项相关的UI控件区域。
   * @return QWidget* 指向包含编解码器设置控件的父 QWidget 的指针。
   */
  QWidget* SetupCodecSection();

  /** @brief 指向视频编解码器选择下拉框的指针。 */
  QComboBox* codec_combobox_{};
  /** @brief 指向帧率选择下拉框 (自定义控件) 的指针。 */
  FrameRateComboBox* frame_rate_combobox_{};
  /** @brief 指向“保持宽高比”复选框的指针。 */
  QCheckBox* maintain_aspect_checkbox_{};
  /** @brief 指向视频缩放方法选择下拉框的指针。 */
  QComboBox* scaling_method_combobox_{};

  /** @brief 指向 CodecStack 控件的指针，用于显示特定于编解码器的设置区域。 */
  CodecStack* codec_stack_{};
  /** @brief 指向图像(序列)导出设置区域的指针。 */
  ImageSection* image_section_{};
  /** @brief 指向 H.264 编码设置区域的指针。 */
  H264Section* h264_section_{};
  /** @brief 指向 H.265 (HEVC) 编码设置区域的指针 (继承自 H264Section)。 */
  H264Section* h265_section_{}; // 类型是 H264Section，但逻辑上用于 H.265
  /** @brief 指向 AV1 编码设置区域的指针。 */
  AV1Section* av1_section_{};
  /** @brief 指向 CineForm 编码设置区域的指针。 */
  CineformSection* cineform_section_{};

  /** @brief 指向 OCIO 颜色空间选择器 (自定义控件) 的指针。 */
  ColorSpaceChooser* color_space_chooser_{};

  /** @brief 指向视频宽度设置滑块 (自定义控件) 的指针。 */
  IntegerSlider* width_slider_{};
  /** @brief 指向视频高度设置滑块 (自定义控件) 的指针。 */
  IntegerSlider* height_slider_{};

  /** @brief 指向应用程序颜色管理器的指针。 */
  ColorManager* color_manager_;

  /** @brief 指向隔行扫描方式选择下拉框 (自定义控件) 的指针。 */
  InterlacedComboBox* interlaced_combobox_{};
  /** @brief 指向像素宽高比选择下拉框 (自定义控件) 的指针。 */
  PixelAspectRatioComboBox* pixel_aspect_combobox_{};
  /** @brief 指向像素格式选择下拉框 (自定义控件) 的指针。 */
  PixelFormatComboBox* pixel_format_field_{};

  /** @brief 存储从高级设置对话框中获取的编码线程数。 */
  int threads_;

  /** @brief 存储从高级设置对话框中获取的像素格式名称字符串。 */
  QString pix_fmt_;
  /** @brief 存储从高级设置对话框中获取的YUV颜色范围。 */
  VideoParams::ColorRange color_range_;

  /** @brief 存储当前主导出对话框选择的输出文件封装格式。 */
  ExportFormat::Format format_;

 private slots:
  /**
   * @brief 当“保持宽高比”复选框的状态改变时调用的槽函数。
   * 用于在用户调整宽度或高度时，自动调整另一项以保持宽高比。
   * @param val 复选框新的选中状态。
   */
  void MaintainAspectRatioChanged(bool val);

  /**
   * @brief 打开高级视频设置对话框 (ExportAdvancedVideoDialog) 的槽函数。
   */
  void OpenAdvancedDialog();

  /**
   * @brief 当帧率发生变化时调用的内部更新槽函数。
   * @param r 新的帧率。
   */
  void UpdateFrameRate(rational r);
};

}  // namespace olive

#endif  // EXPORTVIDEOTAB_H