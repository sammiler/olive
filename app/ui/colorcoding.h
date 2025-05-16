#ifndef COLORCODING_H
#define COLORCODING_H

#include <olive/core/core.h>  // 引入 Olive 核心库的头文件，提供 Color 等核心类型
#include <QColor>             // 引入 QColor，用于 GetUISelectorColor 的返回值 Qt::GlobalColor
#include <QObject>            // 引入 QObject 基类，ColorCoding 本身继承自 QObject
#include <QString>            // 引入 QString，用于颜色名称
#include <QVector>            // 引入 QVector，用于存储颜色列表

// 前向声明 core::Color，如果 olive/core/core.h 中已定义则无需再次声明
// namespace olive { namespace core { class Color; } }

namespace olive {

using namespace core;  // 使用 olive::core 命名空间，方便直接访问其成员如 Color

/**
 * @brief 提供一套标准的颜色编码及其相关操作。
 *
 * ColorCoding 类定义了一组预设的颜色（通过 Code 枚举），并提供了
 * 将这些颜色代码转换为可读名称、实际颜色对象 (olive::core::Color)
 * 以及 Qt UI 选择器颜色 (Qt::GlobalColor) 的静态方法。
 * 它还维护一个标准颜色列表供应用程序使用。
 */
class ColorCoding : public QObject {
  Q_OBJECT
 public:
  /**
   * @brief 定义了一组预设的颜色代码。
   *
   * 这些枚举值用于内部表示和引用标准颜色集中的特定颜色。
   */
  enum Code {
    kRed,     ///< 红色
    kMaroon,  ///< 褐红色/栗色
    kOrange,  ///< 橙色
    kBrown,   ///< 棕色
    kYellow,  ///< 黄色
    kOlive,   ///< 橄榄色
    kLime,    ///< 酸橙色/亮绿色
    kGreen,   ///< 绿色
    kCyan,    ///< 青色/蓝绿色
    kTeal,    ///< 蓝绿色/水鸭色
    kBlue,    ///< 蓝色
    kNavy,    ///< 海军蓝/深蓝色
    kPink,    ///< 粉红色
    kPurple,  ///< 紫色
    kSilver,  ///< 银色
    kGray     ///< 灰色
  };

  /**
   * @brief 根据颜色代码获取其对应的可读名称。
   * @param c 颜色代码的整数值 (对应于 Code 枚举)。
   * @return 返回颜色的本地化 QString 名称。
   */
  static QString GetColorName(int c);

  /**
   * @brief 根据颜色代码获取其对应的 olive::core::Color 对象。
   * @param c 颜色代码的整数值 (对应于 Code 枚举)。
   * @return 返回代表该颜色的 core::Color 对象。
   */
  static Color GetColor(int c);

  /**
   * @brief 根据 olive::core::Color 对象获取一个近似的 Qt::GlobalColor，用于 UI 选择器。
   *
   * 这个函数尝试将一个自定义的 Color 对象匹配到 Qt 预定义的全局颜色之一，
   * 主要用于在某些标准 Qt 控件中显示一个大致相似的颜色。
   * @param c 输入的 core::Color 对象。
   * @return 返回一个最接近的 Qt::GlobalColor 枚举值。
   */
  static Qt::GlobalColor GetUISelectorColor(const Color& c);

  /**
   * @brief 获取标准颜色列表。
   * @return 返回一个包含所有标准 olive::core::Color 对象的 QVector 的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  static const QVector<Color>& standard_colors() { return colors_; }

 private:
  /**
   * @brief 存储标准颜色定义的静态 QVector。
   *
   * 这个向量在类初始化时填充，包含了 Code 枚举中所有颜色对应的 core::Color 对象。
   */
  static QVector<Color> colors_;
};

}  // namespace olive

#endif  // COLORCODING_H
