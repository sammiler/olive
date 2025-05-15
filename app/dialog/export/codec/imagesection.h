#ifndef IMAGESECTION_H
#define IMAGESECTION_H

#include <QCheckBox> // 复选框控件
#include <QWidget>   // QWidget 基类
#include <QString>   // (可能在 .cpp 中使用)

// 假设 codecsection.h 声明了 CodecSection 基类
#include "codecsection.h"
// 假设 rationalslider.h 声明了 RationalSlider 类，并且 rational 类型已定义
#include "widget/slider/rationalslider.h"
// #include "common/define.h" // 如果需要 common/define.h 中的内容 (如 rational)

namespace olive {

/**
 * @brief 用于在导出对话框中配置图像（或图像序列）特定选项的区域。
 *
 * 此类继承自 CodecSection，提供了设置图像导出参数的用户界面元素，
 * 例如是否将输出保存为图像序列，以及在导出单帧图像时选择特定帧（时间点）。
 */
class ImageSection : public CodecSection {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 ImageSection 对象。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit ImageSection(QWidget* parent = nullptr);

  // 默认析构函数通常足够，因为 Qt 的父子对象机制会处理内部的UI控件指针
  // ~ImageSection() override;

  /**
   * @brief 检查“图像序列”复选框当前是否被选中。
   * @return bool 如果复选框被选中，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool IsImageSequenceChecked() const { return image_sequence_checkbox_->isChecked(); }

  /**
   * @brief 设置“图像序列”复选框的选中状态。
   * @param e 如果为 true，则选中复选框；如果为 false，则取消选中。
   */
  void SetImageSequenceChecked(bool e) { image_sequence_checkbox_->setChecked(e); }

  /**
   * @brief 设置帧选择滑块 (RationalSlider) 的时间基准。
   * 时间基准定义了滑块值与实际时间之间的关系，例如项目的帧率。
   * @param r 要设置的时间基准 (rational 类型)。
   */
  void SetTimebase(const rational& r) { frame_slider_->SetTimebase(r); }

  /**
   * @brief 获取当前帧选择滑块选中的时间点。
   * @return rational 当前选中的时间点 (rational 类型)。
   */
  [[nodiscard]] rational GetTime() const { return frame_slider_->GetValue(); }

  /**
   * @brief 设置帧选择滑块的当前时间点。
   * @param t 要设置的时间点 (rational 类型)。
   */
  void SetTime(const rational& t) { frame_slider_->SetValue(t); }

 signals:
  /**
   * @brief 当帧选择滑块的值（即选中的时间点）发生变化时发出此信号。
   * @param t 新的选中时间点 (rational 类型)。
   */
  void TimeChanged(const rational& t);

 private:
  /**
   * @brief 指向 QCheckBox 对象的指针，用于让用户选择是否将输出保存为图像序列。
   */
  QCheckBox* image_sequence_checkbox_;

  /**
   * @brief 指向 RationalSlider 对象的指针，用于当不导出图像序列时，选择要导出的单个帧的时间点。
   */
  RationalSlider* frame_slider_;

 private slots:
  /**
   * @brief 当 `image_sequence_checkbox_` 的状态（选中/未选中）发生改变时调用的槽函数。
   * 此函数通常会根据复选框的状态来启用或禁用 `frame_slider_`，
   * 因为当导出图像序列时，选择单个帧的滑块通常是无意义的。
   * @param e 复选框新的选中状态 (true 表示选中，false 表示未选中)。
   */
  void ImageSequenceCheckBoxToggled(bool e);
};

}  // namespace olive

#endif  // IMAGESECTION_H