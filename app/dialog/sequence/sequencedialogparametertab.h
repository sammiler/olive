#ifndef SEQUENCEDIALOGPARAMETERTAB_H
#define SEQUENCEDIALOGPARAMETERTAB_H

#include <QCheckBox>  // 引入 QCheckBox 类，用于创建复选框控件
#include <QComboBox>  // 引入 QComboBox 类，用于创建下拉列表框控件
#include <QList>      // 引入 QList 类，Qt的泛型容器类
#include <QSpinBox>   // 引入 QSpinBox 类，用于创建整数输入框控件

// 前向声明，避免不必要的头文件包含
QT_BEGIN_NAMESPACE
class QLabel;  // Qt的标签控件，用于显示文本或图片
QT_END_NAMESPACE

#include "node/project/sequence/sequence.h"        // 引入序列类定义
#include "sequencepreset.h"                        // 引入序列预设类定义
#include "widget/slider/integerslider.h"           // 引入整数滑块控件
#include "widget/standardcombos/standardcombos.h"  // 引入标准组合框集合，如帧率、像素宽高比等

namespace olive {

/**
 * @class SequenceDialogParameterTab
 * @brief 序列对话框中的参数设置选项卡。
 *
 * 这个类是 QWidget 的子类，用于在序列对话框中提供一个界面，
 * 用户可以在此界面上配置序列的各种详细参数，如视频分辨率、帧率、音频采样率等。
 */
class SequenceDialogParameterTab : public QWidget {
  Q_OBJECT
 public:
  /**
   * @brief SequenceDialogParameterTab 构造函数。
   *
   * @param sequence 指向要编辑其参数的 Sequence 对象的指针。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit SequenceDialogParameterTab(Sequence* sequence, QWidget* parent = nullptr);

  /**
   * @brief 获取当前选择的视频宽度。
   * @return [[nodiscard]] int 视频宽度值。
   * [[nodiscard]] 属性提示编译器调用者应该使用此函数的返回值。
   */
  [[nodiscard]] int GetSelectedVideoWidth() const { return width_slider_->GetValue(); }

  /**
   * @brief 获取当前选择的视频高度。
   * @return [[nodiscard]] int 视频高度值。
   */
  [[nodiscard]] int GetSelectedVideoHeight() const { return height_slider_->GetValue(); }

  /**
   * @brief 获取当前选择的视频帧率。
   * @return [[nodiscard]] rational 视频帧率（以有理数表示）。
   */
  [[nodiscard]] rational GetSelectedVideoFrameRate() const { return framerate_combo_->GetFrameRate(); }

  /**
   * @brief 获取当前选择的视频像素宽高比。
   * @return [[nodiscard]] rational 视频像素宽高比（以有理数表示）。
   */
  [[nodiscard]] rational GetSelectedVideoPixelAspect() const { return pixelaspect_combo_->GetPixelAspectRatio(); }

  /**
   * @brief 获取当前选择的视频隔行扫描模式。
   * @return [[nodiscard]] VideoParams::Interlacing 视频隔行扫描模式。
   */
  [[nodiscard]] VideoParams::Interlacing GetSelectedVideoInterlacingMode() const {
    return interlacing_combo_->GetInterlaceMode();
  }

  /**
   * @brief 获取当前选择的音频采样率。
   * @return [[nodiscard]] int 音频采样率。
   */
  [[nodiscard]] int GetSelectedAudioSampleRate() const { return audio_sample_rate_field_->GetSampleRate(); }

  /**
   * @brief 获取当前选择的音频声道布局。
   * @return [[nodiscard]] uint64_t 音频声道布局的标识符。
   */
  [[nodiscard]] uint64_t GetSelectedAudioChannelLayout() const { return audio_channels_field_->GetChannelLayout(); }

  /**
   * @brief 获取当前选择的预览分辨率缩放比例。
   * @return [[nodiscard]] int 预览分辨率的除数（例如，2 表示 1/2 分辨率）。
   */
  [[nodiscard]] int GetSelectedPreviewResolution() const { return preview_resolution_field_->GetDivider(); }

  /**
   * @brief 获取当前选择的预览像素格式。
   * @return [[nodiscard]] PixelFormat 预览的像素格式。
   */
  [[nodiscard]] PixelFormat GetSelectedPreviewFormat() const { return preview_format_field_->GetPixelFormat(); }

  /**
   * @brief 获取当前选择的是否启用预览自动缓存。
   * @return [[nodiscard]] static bool 如果启用自动缓存则返回 true，否则返回 false。
   * 注意：此处的注释表明该功能可能被临时禁用。
   */
  [[nodiscard]] static bool GetSelectedPreviewAutoCache() {
    // return preview_autocache_field_->isChecked();
    //  临时：禁用序列自动缓存，想看看片段缓存是否会取代它。
    return false;
  }

 public slots:
  /**
   * @brief 公有槽函数：当预设更改时调用。
   *
   * 用给定的预设值更新此选项卡上的所有参数控件。
   * @param preset 应用于此选项卡的 SequencePreset 对象。
   */
  void PresetChanged(const SequencePreset& preset);

 signals:
  /**
   * @brief 信号：当用户请求将当前参数保存为新预设时发出。
   * @param preset 包含当前参数的 SequencePreset 对象。
   */
  void SaveParametersAsPreset(const SequencePreset& preset);

 private:
  /**
   * @brief 指向视频宽度设置滑块控件的指针。
   */
  IntegerSlider* width_slider_;

  /**
   * @brief 指向视频高度设置滑块控件的指针。
   */
  IntegerSlider* height_slider_;

  /**
   * @brief 指向视频帧率选择组合框控件的指针。
   */
  FrameRateComboBox* framerate_combo_;

  /**
   * @brief 指向视频像素宽高比选择组合框控件的指针。
   */
  PixelAspectRatioComboBox* pixelaspect_combo_;

  /**
   * @brief 指向视频隔行扫描模式选择组合框控件的指针。
   */
  InterlacedComboBox* interlacing_combo_;

  /**
   * @brief 指向音频采样率选择组合框控件的指针。
   */
  SampleRateComboBox* audio_sample_rate_field_;

  /**
   * @brief 指向音频声道布局选择组合框控件的指针。
   */
  ChannelLayoutComboBox* audio_channels_field_;

  /**
   * @brief 指向预览分辨率缩放比例选择组合框控件的指针。
   */
  VideoDividerComboBox* preview_resolution_field_;

  /**
   * @brief 指向用于显示预览分辨率具体数值的标签控件的指针。
   */
  QLabel* preview_resolution_label_;

  /**
   * @brief 指向预览像素格式选择组合框控件的指针。
   */
  PixelFormatComboBox* preview_format_field_;

  /**
   * @brief 指向预览自动缓存启用复选框控件的指针。
   */
  QCheckBox* preview_autocache_field_;

 private slots:
  /**
   * @brief 私有槽函数：当“保存预设”按钮被点击时调用。
   *
   * 触发 SaveParametersAsPreset 信号，将当前参数打包成预设对象发出。
   */
  void SavePresetClicked();

  /**
   * @brief 私有槽函数：更新预览分辨率标签的显示文本。
   *
   * 当预览分辨率缩放比例更改时，此函数会更新标签以显示实际的分辨率。
   */
  void UpdatePreviewResolutionLabel();
};

}  // namespace olive

#endif  // SEQUENCEDIALOGPARAMETERTAB_H
