#ifndef SAMPLEFORMATCOMBOBOX_H
#define SAMPLEFORMATCOMBOBOX_H

#include <olive/core/core.h> // Olive 核心库，包含 SampleFormat 枚举和 AudioParams (虽然 AudioParams 未直接使用，但 SampleFormat 来自其中)
#include <QComboBox>         // Qt 组合框控件基类
#include <QVariant>          // Qt 通用数据类型类 (用于 addItem 的 userData)
#include <vector>            // 标准 C++ vector 容器 (用于 SetAvailableFormats 参数)
#include <cstdint>           // 标准整数类型 (uint64_t - 虽然这里用的是 SampleFormat 枚举，但其底层类型可能是整数)


#include "ui/humanstrings.h"   // 用于将枚举或ID转换为用户可读的字符串

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QWidget; // QComboBox 的基类 QWidget
// class QString; // HumanStrings::FormatToString 返回类型，addItem 参数

namespace olive {

using namespace core; // 使用 olive::core 命名空间，方便直接使用该命名空间下的类型，如 SampleFormat

/**
 * @brief SampleFormatComboBox 类是一个自定义的 QComboBox，用于选择音频采样格式。
 *
 * 它允许动态设置可供选择的采样格式列表，或者填充预定义的“打包”格式。
 * 当列表内容更新时，它可以尝试恢复之前选中的格式。
 */
class SampleFormatComboBox : public QComboBox {
  Q_OBJECT // Qt 元对象系统宏 (虽然此类中没有显式定义信号或槽，但基类 QComboBox 有)

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit SampleFormatComboBox(QWidget *parent = nullptr) : QComboBox(parent), attempt_to_restore_format_(true) {}

  /**
   * @brief 设置在更新格式列表时是否尝试恢复之前的选中格式。
   * @param e 如果为 true，则尝试恢复；否则不尝试。
   */
  void SetAttemptToRestoreFormat(bool e) { attempt_to_restore_format_ = e; }

  /**
   * @brief 使用提供的采样格式列表填充组合框。
   * @param formats 包含要添加到组合框中的 SampleFormat 对象的 std::vector。
   */
  void SetAvailableFormats(const std::vector<SampleFormat> &formats) {
    auto tmp = SampleFormat(SampleFormat::INVALID); // 临时变量，用于存储当前选中的格式

    if (attempt_to_restore_format_) { // 如果需要尝试恢复格式
      tmp = GetSampleFormat(); // 获取当前选中的格式
    }

    clear(); // 清空组合框中的所有现有项
    // 遍历提供的格式列表
    foreach (const SampleFormat &of, formats) {
      AddFormatItem(of); // 添加每个格式到组合框
    }

    if (attempt_to_restore_format_) { // 如果需要尝试恢复格式
      SetSampleFormat(tmp); // 尝试设置回之前选中的格式
    }
  }

  /**
   * @brief 填充组合框，仅包含“打包的”(packed) 音频采样格式。
   *
   * 打包格式的范围由 SampleFormat::PACKED_START 和 SampleFormat::PACKED_END 定义。
   */
  void SetPackedFormats() {
    auto tmp = SampleFormat(SampleFormat::INVALID); // 临时变量，用于存储当前选中的格式

    if (attempt_to_restore_format_) { // 如果需要尝试恢复格式
      tmp = GetSampleFormat(); // 获取当前选中的格式
    }

    clear(); // 清空组合框
    // 遍历所有打包格式的枚举值
    for (int i = SampleFormat::PACKED_START; i < SampleFormat::PACKED_END; i++) {
      AddFormatItem(SampleFormat(static_cast<SampleFormat::Format>(i))); // 添加每个打包格式
    }

    if (attempt_to_restore_format_) { // 如果需要尝试恢复格式
      SetSampleFormat(tmp); // 尝试设置回之前选中的格式
    }
  }

  /**
   * @brief 获取当前选中的音频采样格式。
   * @return 返回 SampleFormat 对象。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] SampleFormat GetSampleFormat() const {
    // 从当前选中项的用户数据中获取整数值，并将其转换为 SampleFormat::Format 枚举类型，
    // 然后构造并返回一个 SampleFormat 对象。
    return SampleFormat(static_cast<SampleFormat::Format>(this->currentData().toInt()));
  }

  /**
   * @brief 根据给定的采样格式设置组合框的当前选中项。
   * @param fmt 要设置的 SampleFormat 对象。
   */
  void SetSampleFormat(SampleFormat fmt) {
    // 遍历组合框中的所有项
    for (int i = 0; i < this->count(); i++) {
      // 如果当前项的用户数据（即采样格式的枚举整数值）与给定的 fmt 匹配
      if (this->itemData(i).toInt() == static_cast<SampleFormat::Format>(fmt)) {
        this->setCurrentIndex(i); // 设置此项为当前选中项
        break;                    // 找到匹配项后即可退出循环
      }
    }
  }

 private:
  /**
   * @brief 向组合框中添加一个采样格式项。
   *
   * 使用 HumanStrings 将 SampleFormat 转换为用户可读的字符串作为显示文本，
   * 并将原始的 SampleFormat::Format 枚举值作为用户数据存储。
   * @param f 要添加的 SampleFormat 对象。
   */
  void AddFormatItem(SampleFormat f) {
    this->addItem(HumanStrings::FormatToString(f), static_cast<SampleFormat::Format>(f));
  }

  bool attempt_to_restore_format_; ///< 标记在列表内容改变后是否尝试恢复之前的选中项。
};

}  // namespace olive

#endif  // SAMPLEFORMATCOMBOBOX_H
