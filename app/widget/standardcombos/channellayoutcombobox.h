#ifndef CHANNELLAYOUTCOMBOBOX_H
#define CHANNELLAYOUTCOMBOBOX_H

#include <olive/core/core.h>  // Olive 核心库，可能包含 AudioParams 和 rational 等类型定义
#include <QComboBox>          // Qt 组合框控件基类
#include <QVariant>           // Qt 通用数据类型类 (用于 addItem 的 userData)
#include <cstdint>            // 标准整数类型 (uint64_t)

#include "ui/humanstrings.h"  // 用于将枚举或ID转换为用户可读的字符串

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QWidget; // QComboBox 的基类 QWidget
// class QString; // HumanStrings::ChannelLayoutToString 返回类型，addItem 参数

namespace olive {

using namespace core;  // 使用 olive::core 命名空间，方便直接使用该命名空间下的类型，如 AudioParams

/**
 * @brief ChannelLayoutComboBox 类是一个自定义的 QComboBox，专门用于选择音频通道布局。
 *
 * 它在构造时会填充预定义的、支持的音频通道布局选项，
 * 每个选项显示为用户可读的字符串，并关联一个 uint64_t 类型的内部标识符。
 */
class ChannelLayoutComboBox : public QComboBox {
 Q_OBJECT  // Qt 元对象系统宏 (虽然此类中没有显式定义信号或槽，但基类 QComboBox 有)

     public :
     /**
      * @brief 构造函数。
      *
      * 初始化组合框，并从 AudioParams::kSupportedChannelLayouts 中获取支持的通道布局，
      * 使用 HumanStrings::ChannelLayoutToString 将其转换为用户可读的字符串，然后添加到下拉列表中。
      * 每个列表项的用户数据 (userData) 存储的是原始的 uint64_t 通道布局标识符。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit ChannelLayoutComboBox(QWidget* parent = nullptr)
     : QComboBox(parent) {
    // 遍历 AudioParams 中定义的所有支持的通道布局
    foreach (const uint64_t& ch_layout, AudioParams::kSupportedChannelLayouts) {
      // 将 uint64_t 类型的通道布局标识符转换为用户可读的字符串，并添加到组合框中
      // 同时，将原始的 uint64_t 值作为 QVariant 存储在对应项的用户数据中
      this->addItem(HumanStrings::ChannelLayoutToString(ch_layout), QVariant::fromValue(ch_layout));
    }
  }

  /**
   * @brief 获取当前选中的音频通道布局的内部标识符。
   * @return 返回 uint64_t 类型的通道布局标识符。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] uint64_t GetChannelLayout() const { return this->currentData().toULongLong(); }

  /**
   * @brief 根据给定的内部标识符设置组合框的当前选中项。
   * @param ch 要设置的 uint64_t 通道布局标识符。
   */
  void SetChannelLayout(uint64_t ch) {
    // 遍历组合框中的所有项
    for (int i = 0; i < this->count(); i++) {
      // 如果当前项的用户数据（即通道布局标识符）与给定的 ch 匹配
      if (this->itemData(i).toULongLong() == ch) {
        this->setCurrentIndex(i);  // 设置此项为当前选中项
        break;                     // 找到匹配项后即可退出循环
      }
    }
  }
};

}  // namespace olive

#endif  // CHANNELLAYOUTCOMBOBOX_H
