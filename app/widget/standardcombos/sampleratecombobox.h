#ifndef SAMPLERATECOMBOBOX_H
#define SAMPLERATECOMBOBOX_H

#include <olive/core/core.h> // Olive 核心库，可能包含 AudioParams 和 kSupportedSampleRates
#include <QComboBox>         // Qt 组合框控件基类
#include <QVariant>          // Qt 通用数据类型类 (用于 addItem 的 userData)
#include <QWidget>           // Qt 控件基类 (QComboBox 的基类)

#include "ui/humanstrings.h"   // 用于将枚举或ID转换为用户可读的字符串

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QString; // HumanStrings::SampleRateToString 返回类型，addItem 参数

namespace olive {

using namespace core; // 使用 olive::core 命名空间，方便直接使用该命名空间下的类型，如 AudioParams

/**
 * @brief SampleRateComboBox 类是一个自定义的 QComboBox，用于选择音频采样率。
 *
 * 它在构造时会填充预定义的、支持的音频采样率选项，
 * 每个选项显示为用户可读的字符串（例如 "48000 Hz"），
 * 并关联一个整数类型的内部采样率值。
 */
class SampleRateComboBox : public QComboBox {
  Q_OBJECT // Qt 元对象系统宏 (虽然此类中没有显式定义信号或槽，但基类 QComboBox 有)

 public:
  /**
   * @brief 构造函数。
   *
   * 初始化组合框，并从 AudioParams::kSupportedSampleRates 中获取支持的采样率，
   * 使用 HumanStrings::SampleRateToString 将其转换为用户可读的字符串，然后添加到下拉列表中。
   * 每个列表项的用户数据 (userData) 存储的是原始的 int 采样率值。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit SampleRateComboBox(QWidget* parent = nullptr) : QComboBox(parent) {
    // 遍历 AudioParams 中定义的所有支持的采样率
    foreach (int sr, AudioParams::kSupportedSampleRates) {
      // 将 int 类型的采样率值转换为用户可读的字符串，并添加到组合框中
      // 同时，将原始的 int 值作为 QVariant 存储在对应项的用户数据中
      this->addItem(HumanStrings::SampleRateToString(sr), sr);
    }
  }

  /**
   * @brief 获取当前选中的音频采样率。
   * @return 返回 int 类型的采样率值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] int GetSampleRate() const { return this->currentData().toInt(); }

  /**
   * @brief 根据给定的采样率值设置组合框的当前选中项。
   * @param rate 要设置的 int 类型的采样率。
   */
  void SetSampleRate(int rate) {
    // 遍历组合框中的所有项
    for (int i = 0; i < this->count(); i++) {
      // 如果当前项的用户数据（即采样率整数值）与给定的 rate 匹配
      if (this->itemData(i).toInt() == rate) {
        this->setCurrentIndex(i); // 设置此项为当前选中项
        break;                    // 找到匹配项后即可退出循环
      }
    }
  }
};

}  // namespace olive

#endif  // SAMPLERATECOMBOBOX_H
