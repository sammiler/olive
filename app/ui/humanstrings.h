#ifndef HUMANSTRINGS_H
#define HUMANSTRINGS_H

#include <olive/core/core.h>  // 引入 Olive 核心库的头文件，提供 SampleFormat 等核心类型
#include <QObject>            // 引入 QObject 基类，HumanStrings 本身继承自 QObject
#include <QString>            // 引入 QString 类，用于返回可读字符串

// 前向声明 core 命名空间中的相关类型，如果 olive/core/core.h 中已定义则无需再次声明
// 例如：
// namespace olive { namespace core { enum class SampleFormat; } }
// namespace olive { namespace core { class rational; } } // 假设 SampleFormat 和 rational 在 core 命名空间

namespace olive {

using namespace core;  // 使用 olive::core 命名空间，方便直接访问其成员如 SampleFormat

/**
 * @brief 提供将各种内部数据类型转换为人类可读字符串的静态工具方法。
 *
 * HumanStrings 类主要用于用户界面显示，将程序内部使用的枚举或数值
 * （如采样率、声道布局、采样格式）转换为用户更容易理解的文本描述。
 */
class HumanStrings : public QObject {
  Q_OBJECT
 public:
  /**
   * @brief 默认构造函数。
   *
   * 由于此类主要包含静态方法，通常不需要实例化。
   * explicit 关键字在此处对于默认构造函数不是必需的，但如果添加了其他构造函数，则可能有用。
   * 使用 = default 表示使用编译器生成的默认构造函数。
   */
  HumanStrings() = default;

  /**
   * @brief 将采样率（整数）转换为人类可读的字符串。
   *
   * 例如，将 48000 转换为 "48.0 kHz"。
   * @param sample_rate 以赫兹（Hz）为单位的采样率整数值。
   * @return 返回表示采样率的 QString，通常带有单位（如 kHz）。
   */
  static QString SampleRateToString(const int &sample_rate);

  /**
   * @brief 将声道布局（通常是一个位掩码或枚举的 uint64_t 值）转换为人类可读的字符串。
   *
   * 例如，将特定的声道布局值转换为 "Stereo", "5.1 Surround", "Mono" 等。
   * @param layout 代表声道布局的 uint64_t 值。
   * @return 返回描述声道布局的 QString。
   */
  static QString ChannelLayoutToString(const uint64_t &layout);

  /**
   * @brief 将采样格式（SampleFormat 枚举）转换为人类可读的字符串。
   *
   * 例如，将 SampleFormat::kFloatP 转换为 "32-bit Floating Point (Planar)"。
   * @param f 要转换的 SampleFormat 枚举值。
   * @return 返回描述采样格式的 QString。
   */
  static QString FormatToString(const SampleFormat &f);
};

}  // namespace olive

#endif  // HUMANSTRINGS_H
