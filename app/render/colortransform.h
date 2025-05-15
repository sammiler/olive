#ifndef COLORTRANSFORM_H // 防止头文件被重复包含的宏
#define COLORTRANSFORM_H // 定义 COLORTRANSFORM_H 宏

#include <QString> // Qt 字符串类

#include "common/define.h"    // 可能包含项目通用的定义或宏
#include "common/ocioutils.h" // 包含 OpenColorIO (OCIO) 工具类或相关定义的头文件
                              // (虽然在此文件中未直接使用 OCIO 类型，但逻辑上相关)

namespace olive { // olive 项目的命名空间

/**
 * @brief ColorTransform 类用于描述一个具体的颜色转换操作。
 *
 * 它可以表示两种主要的颜色转换类型：
 * 1.  **色彩空间到色彩空间的转换：** 仅指定一个输出色彩空间 (`output_`)。
 *     这种情况下，`is_display_` 通常为 false。
 * 2.  **显示变换：** 指定一个显示设备 (`display_`，内部存储在 `output_` 中)、
 *     一个视图变换 (`view_`) 和一个可选的外观/风格 (`look_`)。
 *     这种情况下，`is_display_` 为 true。显示变换通常用于将场景参考色彩空间
 *     的图像转换为特定显示设备能够正确显示的色彩空间和外观。
 *
 * 这个类通常与 ColorManager 和 ColorProcessor 一起使用，
 * ColorManager 会根据 ColorTransform 的配置来创建相应的 ColorProcessor
 * 以执行实际的颜色转换。
 */
class ColorTransform {
 public:
  /**
   * @brief 默认构造函数。
   * 初始化为非显示变换 (`is_display_` = false)。
   * `output_`, `view_`, `look_` 会进行默认初始化 (空字符串)。
   */
  ColorTransform() { is_display_ = false; }

  /**
   * @brief 构造函数，用于创建一个色彩空间到色彩空间的转换。
   * @param output 目标输出色彩空间的名称。
   */
  explicit ColorTransform(const QString& output) {
    is_display_ = false; // 标记为非显示变换
    output_ = output;   // 设置输出色彩空间
                         // view_ 和 look_ 保持默认 (空字符串)
  }

  /**
   * @brief 构造函数，用于创建一个显示变换。
   * @param display 目标显示设备的名称 (例如 "sRGB", "Rec.709 Display")。
   * @param view 视图变换的名称 (例如 "Film", "Video")。
   * @param look (可选) 应用的外观/风格的名称 (例如 "Gritty", "Cool")。
   */
  ColorTransform(const QString& display, const QString& view, const QString& look) {
    is_display_ = true;   // 标记为显示变换
    output_ = display;    // 显示设备名称存储在 output_ 中
    view_ = view;         // 设置视图变换
    look_ = look;         // 设置外观
  }

  /**
   * @brief 检查此颜色转换是否为显示变换。
   * @return 如果是显示变换，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool is_display() const { return is_display_; }

  /**
   * @brief 获取显示设备的名称 (如果 `is_display_` 为 true)。
   * 对于非显示变换，此方法仍返回 `output_`，即目标色彩空间。
   * @return 返回显示设备名称或目标色彩空间名称的常量引用。
   */
  [[nodiscard]] const QString& display() const { return output_; }

  /**
   * @brief 获取目标输出色彩空间的名称 (如果 `is_display_` 为 false)，
   * 或者显示设备的名称 (如果 `is_display_` 为 true)。
   * 实质上与 `display()` 方法返回相同的值。
   * @return 返回目标输出/显示设备名称的常量引用。
   */
  [[nodiscard]] const QString& output() const { return output_; }

  /**
   * @brief 获取视图变换的名称 (仅当 `is_display_` 为 true 时有意义)。
   * @return 返回视图变换名称的常量引用。如果不是显示变换，通常返回空字符串。
   */
  [[nodiscard]] const QString& view() const { return view_; }

  /**
   * @brief 获取外观/风格的名称 (仅当 `is_display_` 为 true 时有意义)。
   * @return 返回外观名称的常量引用。如果不是显示变换或未指定外观，通常返回空字符串。
   */
  [[nodiscard]] const QString& look() const { return look_; }

 private:
  QString output_; // 存储目标输出色彩空间名称或显示设备名称

  bool is_display_; // 标记是否为显示变换
  QString view_;    // 存储视图变换的名称 (用于显示变换)
  QString look_;    // 存储外观/风格的名称 (用于显示变换)
};

}  // namespace olive

#endif  // COLORTRANSFORM_H