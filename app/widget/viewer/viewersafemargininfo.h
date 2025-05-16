#ifndef VIEWERSAFEMARGININFO_H
#define VIEWERSAFEMARGININFO_H

#include <QtMath>  // 包含 Qt 数学函数，例如 qFuzzyCompare

#include "common/define.h"  // 包含项目通用定义

namespace olive {

/**
 * @brief 存储查看器安全边距信息的类。
 *
 * 安全边距是在视频编辑中用于确保重要视觉元素（如文本或图形）
 * 不会因不同显示设备的过扫描（overscan）而被裁剪掉的指导线。
 * 此类封装了安全边距是否启用以及其比例的设置。
 */
class ViewerSafeMarginInfo {
 public:
  /**
   * @brief 默认构造函数。
   * 默认情况下，安全边距是禁用的。
   */
  ViewerSafeMarginInfo() : enabled_(false) {}

  /**
   * @brief 显式构造函数。
   * @param enabled 布尔值，指示安全边距是否启用。
   * @param ratio 双精度浮点数，表示安全边距的比例，默认为 0。
   * 如果 ratio 大于 0，则表示使用自定义比例。
   */
  explicit ViewerSafeMarginInfo(bool enabled, double ratio = 0) : enabled_(enabled), ratio_(ratio) {}

  /**
   * @brief 检查安全边距是否已启用。
   * @return 如果安全边距已启用，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool is_enabled() const { return enabled_; }

  /**
   * @brief 检查是否使用了自定义的安全边距比例。
   * @return 如果 ratio_ 大于 0，则返回 true，表示使用自定义比例；否则返回 false。
   */
  [[nodiscard]] bool custom_ratio() const { return (ratio_ > 0); }

  /**
   * @brief 获取安全边距的比例值。
   * @return 返回安全边距的比例（双精度浮点数）。
   */
  [[nodiscard]] double ratio() const { return ratio_; }

  /**
   * @brief 重载等于运算符 (==)。
   * 比较两个 ViewerSafeMarginInfo 对象是否相等。
   * 比较 enabled_ 状态和使用 qFuzzyCompare 比较 ratio_（用于浮点数比较）。
   * @param rhs 要比较的右侧 ViewerSafeMarginInfo 对象。
   * @return 如果两个对象相等，则返回 true；否则返回 false。
   */
  bool operator==(const ViewerSafeMarginInfo& rhs) const {
    return (enabled_ == rhs.enabled_ && qFuzzyCompare(ratio_, rhs.ratio_));
  }

  /**
   * @brief 重载不等于运算符 (!=)。
   * 比较两个 ViewerSafeMarginInfo 对象是否不相等。
   * @param rhs 要比较的右侧 ViewerSafeMarginInfo 对象。
   * @return 如果两个对象不相等，则返回 true；否则返回 false。
   */
  bool operator!=(const ViewerSafeMarginInfo& rhs) const {
    return (enabled_ != rhs.enabled_ || !qFuzzyCompare(ratio_, rhs.ratio_));
  }

 private:
  /**
   * @brief 指示安全边距是否启用的布尔标志。
   */
  bool enabled_;

  /**
   * @brief 安全边距的比例。
   * 如果为 0 或负数，可能表示使用预设比例或未启用自定义比例。
   * 如果大于 0，则表示自定义比例。
   */
  double ratio_{};
};

}  // namespace olive

#endif  // VIEWERSAFEMARGININFO_H
