#ifndef DECIMALSLIDERBASE_H
#define DECIMALSLIDERBASE_H

#include "numericsliderbase.h"

namespace olive {

/**
 * @brief DecimalSliderBase 类是用于处理带小数的浮点数值的滑块控件的基类。
 *
 * 它继承自 NumericSliderBase，并增加了控制小数位数显示以及
 * 如何将浮点数转换为字符串表示的功能。
 */
class DecimalSliderBase : public NumericSliderBase {
  // Q_OBJECT // NumericSliderBase 可能已经包含了 Q_OBJECT，或者此基类不需要自定义信号槽。遵循不修改原则。

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit DecimalSliderBase(QWidget* parent = nullptr);

  /**
   * @brief 获取当前设置的用于显示的小数位数。
   * @return 返回小数位数 (int 类型)。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] int GetDecimalPlaces() const { return decimal_places_; }
  /**
   * @brief 设置用于显示的小数位数。
   * @param i 要设置的小数位数。
   */
  void SetDecimalPlaces(int i);

  /**
   * @brief 获取是否启用了自动修剪小数位的功能。
   *
   * 如果启用，则在将浮点数转换为字符串时，会移除末尾多余的零。
   * @return 如果启用了自动修剪，则返回 true；否则返回 false。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool GetAutoTrimDecimalPlaces() const { return autotrim_decimal_places_; };
  /**
   * @brief 设置是否启用自动修剪小数位的功能。
   * @param e 如果为 true，则启用自动修剪。
   */
  void SetAutoTrimDecimalPlaces(bool e);

  /**
   * @brief 静态辅助函数，将浮点数转换为字符串，并根据指定的小数位数和自动修剪设置进行格式化。
   * @param val 要转换的 double 类型浮点数。
   * @param decimal_places 要保留的小数位数。
   * @param autotrim_decimal_places 是否自动修剪末尾多余的零。
   * @return 返回格式化后的 QString。
   */
  static QString FloatToString(double val, int decimal_places, bool autotrim_decimal_places);

 private:
  int decimal_places_;  ///< 存储当前设置的用于显示的小数位数。

  bool autotrim_decimal_places_;  ///< 标记是否启用了自动修剪小数末尾零的功能。
};

}  // namespace olive

#endif  // DECIMALSLIDERBASE_H
