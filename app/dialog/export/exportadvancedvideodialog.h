#ifndef EXPORTADVANCEDVIDEODIALOG_H
#define EXPORTADVANCEDVIDEODIALOG_H

#include <QComboBox> // 下拉选择框控件
#include <QDialog>   // QDialog 基类
#include <QStringList> // 为了构造函数参数 pix_fmts
#include <QWidget>   // 为了 QWidget* parent 参数

// 假设 encoder.h 声明了 EncodingParams 类 (其中可能定义了 VideoParams::ColorRange)
#include "codec/encoder.h" // 包含 EncodingParams, VideoParams::ColorRange
// 假设 integerslider.h 声明了 IntegerSlider 类
#include "widget/slider/integerslider.h"
// #include "common/define.h" // 如果需要 common/define.h 中的内容

namespace olive {

/**
 * @brief 提供高级视频导出选项配置的对话框类。
 *
 * 此对话框允许用户调整更细致的视频编码参数，例如：
 * - 使用的编码线程数。
 * - 输出的像素格式。
 * - YUV颜色范围 (例如 TV/Limited Range vs. PC/Full Range)。
 */
class ExportAdvancedVideoDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 ExportAdvancedVideoDialog 对象。
   * @param pix_fmts 一个包含可选像素格式名称的字符串列表，用于填充像素格式下拉框。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit ExportAdvancedVideoDialog(const QList<QString>& pix_fmts, QWidget* parent = nullptr);

  // 默认析构函数通常足够，因为 Qt 的父子对象机制会处理内部的UI控件指针
  // ~ExportAdvancedVideoDialog() override;

  /**
   * @brief 获取用户设置的编码线程数。
   * @return int 线程数量。
   */
  [[nodiscard]] int threads() const { return static_cast<int>(thread_slider_->GetValue()); }

  /**
   * @brief 设置编码线程数UI控件的值。
   * @param t 要设置的线程数量。
   */
  void set_threads(int t) { thread_slider_->SetValue(t); }

  /**
   * @brief 获取用户选择的输出像素格式名称。
   * @return QString 像素格式的名称字符串 (例如 "yuv420p")。
   */
  [[nodiscard]] QString pix_fmt() const { return pixel_format_combobox_->currentText(); }

  /**
   * @brief 设置像素格式下拉框中当前选中的项。
   * @param s 要选中的像素格式的名称字符串。
   */
  void set_pix_fmt(const QString& s) { pixel_format_combobox_->setCurrentText(s); }

  /**
   * @brief 获取用户选择的YUV颜色范围。
   * @return VideoParams::ColorRange YUV颜色范围的枚举值。
   */
  [[nodiscard]] VideoParams::ColorRange yuv_range() const {
    // 假设下拉框的索引直接对应 VideoParams::ColorRange 枚举值
    return static_cast<VideoParams::ColorRange>(yuv_color_range_combobox_->currentIndex());
  }

  /**
   * @brief 设置YUV颜色范围下拉框的当前选中项。
   * @param i 要设置的 VideoParams::ColorRange 枚举值。
   */
  void set_yuv_range(VideoParams::ColorRange i) { yuv_color_range_combobox_->setCurrentIndex(static_cast<int>(i)); }

 private:
  /**
   * @brief 指向 IntegerSlider 控件的指针，用于设置编码线程数。
   */
  IntegerSlider* thread_slider_;

  /**
   * @brief 指向 QComboBox 控件的指针，用于选择输出像素格式。
   */
  QComboBox* pixel_format_combobox_;

  /**
   * @brief 指向 QComboBox 控件的指针，用于选择YUV颜色范围。
   */
  QComboBox* yuv_color_range_combobox_;
};

}  // namespace olive

#endif  // EXPORTADVANCEDVIDEODIALOG_H