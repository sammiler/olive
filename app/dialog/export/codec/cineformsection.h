#ifndef CINEFORMSECTION_H
#define CINEFORMSECTION_H

#include <QComboBox> // 下拉选择框控件，用于质量选择
#include <QWidget>   // 为了 QWidget* parent 参数

// 假设 codecsection.h 声明了 CodecSection 基类和 EncodingParams 类
#include "codecsection.h"
// #include "common/define.h" // 如果需要 common/define.h 中的内容

namespace olive {

/**
 * @brief 用于在导出对话框中配置 GoPro CineForm 编解码器特定选项的区域。
 *
 * 此类继承自 CodecSection，提供了设置 CineForm 编码参数的用户界面元素，
 * 例如选择编码质量级别。
 */
class CineformSection : public CodecSection {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 CineformSection 对象。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit CineformSection(QWidget *parent = nullptr);

  // 默认析构函数通常足够，因为 Qt 的父子对象机制会处理 quality_combobox_
  // ~CineformSection() override;

  /**
   * @brief 将此 CineformSection 中的用户设置（例如选择的质量级别）添加到 EncodingParams 对象中。
   * 此方法被导出流程调用，以收集所有编码相关的参数。
   * @param params 指向 EncodingParams 对象的指针，CineForm 特定的编码选项将被写入此对象。
   */
  void AddOpts(EncodingParams *params) override;

  /**
   * @brief 根据提供的 EncodingParams 对象中的设置来配置此 CineformSection 的UI控件。
   * 例如，如果加载了包含 CineForm 设置的预设，则会调用此方法来更新界面显示。
   * @param p 指向包含要应用的编码参数的 EncodingParams 对象的常量指针。
   */
  void SetOpts(const EncodingParams *p) override;

private:
  /**
   * @brief 指向 QComboBox 控件的指针，用于让用户选择 CineForm 编码的质量级别。
   * （例如：低、中、高、电影级等）
   */
  QComboBox *quality_combobox_;
};

}  // namespace olive

#endif  // CINEFORMSECTION_H