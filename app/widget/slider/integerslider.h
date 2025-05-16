#ifndef INTEGERSLIDER_H
#define INTEGERSLIDER_H

#include "base/numericsliderbase.h" // 数值滑块基类

namespace olive {

/**
 * @brief IntegerSlider 类是一个专门用于处理和显示整数 (int64_t) 的滑块控件。
 *
 * 它继承自 NumericSliderBase，并为整数类型的值提供了具体的实现，
 * 包括值与字符串之间的转换、拖拽调整逻辑以及特定类型的信号。
 */
class IntegerSlider : public NumericSliderBase {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit IntegerSlider(QWidget* parent = nullptr);

  /**
   * @brief 获取滑块当前的整数值。
   * @return 返回 int64_t 类型的值。
   */
  int64_t GetValue();

  /**
   * @brief 设置滑块的整数值。
   * @param v 要设置的 int64_t 类型的值。
   */
  void SetValue(const int64_t& v);

  /**
   * @brief 设置滑块允许的最小整数值。
   * @param d 最小 int64_t 类型的值。
   */
  void SetMinimum(const int64_t& d);

  /**
   * @brief 设置滑块允许的最大整数值。
   * @param d 最大 int64_t 类型的值。
   */
  void SetMaximum(const int64_t& d);

  /**
   * @brief 设置滑块的默认整数值。
   * @param d 要设置的默认 int64_t 类型的值。
   */
  void SetDefaultValue(const int64_t& d);

 protected:
  /**
   * @brief 重写基类方法，将 QVariant 类型的值（内部应为 int64_t）转换为 QString 以供显示。
   * @param v 要转换的 QVariant 值。
   * @return 返回值的字符串表示。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QString ValueToString(const QVariant& v) const override;

  /**
   * @brief 重写基类方法，将 QString 转换为 QVariant 类型的值（内部为 int64_t）。
   * @param s 要转换的 QString。
   * @param ok 指向布尔值的指针，用于指示转换是否成功。
   * @return 返回转换后的 QVariant 值。
   */
  QVariant StringToValue(const QString& s, bool* ok) const override;

  /**
   * @brief 重写基类方法，当滑块的值发生改变时，发出特定类型的 ValueChanged(int64_t) 信号。
   * @param value 改变后的新值 (QVariant，内部为 int64_t)。
   */
  void ValueSignalEvent(const QVariant& value) override;

  /**
   * @brief 重写基类方法，用于在拖拽过程中根据起始值和拖拽距离调整数值。
   *
   * 针对整数进行调整，通常会将拖拽距离（浮点数）转换为整数变化量。
   * @param start 拖拽开始时的原始值 (QVariant，内部为 int64_t)。
   * @param drag 鼠标拖拽的距离或一个表示拖拽程度的因子。
   * @return 返回调整后的新值 (QVariant，内部为 int64_t)。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QVariant AdjustDragDistanceInternal(const QVariant& start, const double& drag) const override;

 signals:
  /**
   * @brief 当滑块的整数数值发生改变时发出此信号。
   * @param value 新的 int64_t 类型的值。
   */
  void ValueChanged(int64_t);
};

}  // namespace olive

#endif  // INTEGERSLIDER_H
