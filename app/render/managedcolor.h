#ifndef MANAGEDCOLOR_H  // 防止头文件被重复包含的宏
#define MANAGEDCOLOR_H  // 定义 MANAGEDCOLOR_H 宏

#include <olive/core/core.h>  // 包含 Olive 核心定义，很可能 Color 和 PixelFormat 在这里定义
                              // (例如，olive::core::Color, olive::core::PixelFormat)

#include "colortransform.h"  // 包含 ColorTransform 类的定义

// 假设 Color 和 PixelFormat 类型在 <olive/core/core.h> 中定义，
// 并且是类似 olive::core::Color 和 olive::core::PixelFormat 的形式。
// ManagedColor 继承自 Color，所以 Color 的定义是必需的。

namespace olive {  // olive 项目的命名空间

/**
 * @brief ManagedColor 类扩展了基础的 Color 类，增加了色彩空间管理和转换的上下文信息。
 *
 * 它不仅存储颜色值 (通过继承自 Color 类)，还记录了这个颜色值的原始输入色彩空间
 * (`color_input_`) 以及期望将其转换到的输出色彩变换 (`color_transform_`)。
 *
 * 这使得颜色值可以在 Olive 的色彩管理流程中被正确地解释和转换。
 * 例如，一个从UI颜色拾取器获取的颜色可能是 sRGB 空间的 (这是 `color_input_`)，
 * 而它最终可能需要被转换到场景线性空间或特定的显示空间 (由 `color_transform_` 定义)。
 *
 * 这个类主要用于那些需要在不同色彩空间之间传递和处理的颜色值。
 */
class ManagedColor : public Color {  // ManagedColor 继承自 (olive::core::)Color
 public:
  // 默认构造函数
  ManagedColor();
  /**
   * @brief 构造函数，通过 RGBA 分量创建 ManagedColor。
   * 继承的 Color 部分会存储这些值。色彩空间信息需要额外设置。
   * @param r 红色分量。
   * @param g 绿色分量。
   * @param b 蓝色分量。
   * @param a (可选) Alpha 分量，默认为 1.0 (不透明)。
   */
  ManagedColor(const double& r, const double& g, const double& b, const double& a = 1.0);
  /**
   * @brief 构造函数，从原始像素数据、像素格式和通道布局创建 ManagedColor。
   * 这个构造函数会根据提供的格式从 `data` 中读取颜色值。
   * 继承的 Color 部分会存储这些值。色彩空间信息需要额外设置。
   * @param data 指向原始像素数据的指针。
   * @param format 像素数据的格式 (例如，RGB8, RGBA_FLOAT32)。
   * @param channel_layout 通道布局 (例如，RGB, RGBA, BGR)。
   */
  ManagedColor(const char* data, const PixelFormat& format, int channel_layout);
  /**
   * @brief 构造函数，从一个基础的 Color 对象创建 ManagedColor。
   * 颜色值被复制，色彩空间信息需要额外设置。
   * @param c 要复制其颜色值的 Color 对象。
   */
  explicit ManagedColor(const Color& c);

  /**
   * @brief 获取此颜色值的输入色彩空间名称。
   * @return 返回输入色彩空间名称字符串的常量引用。
   */
  [[nodiscard]] QString color_input() const;
  /**
   * @brief 设置此颜色值的输入色彩空间名称。
   * @param color_input 要设置的输入色彩空间名称。
   */
  void set_color_input(const QString& color_input);

  /**
   * @brief 获取此颜色值期望的输出色彩变换配置。
   * @return 返回 ColorTransform 对象的常量引用。
   */
  [[nodiscard]] const ColorTransform& color_output() const;
  /**
   * @brief 设置此颜色值期望的输出色彩变换配置。
   * @param color_output 要设置的 ColorTransform 对象。
   */
  void set_color_output(const ColorTransform& color_output);

 private:
  QString color_input_;  // 存储此颜色值的原始输入色彩空间的名称

  ColorTransform color_transform_;  // 存储此颜色值期望的输出色彩变换配置
};

}  // namespace olive

#endif  // MANAGEDCOLOR_H