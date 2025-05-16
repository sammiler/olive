#ifndef STRINGSLIDER_H
#define STRINGSLIDER_H

#include "base/sliderbase.h" // 滑块基类

namespace olive {

/**
 * @brief StringSlider 类是一个专门用于处理和显示字符串 (QString) 的滑块控件。
 *
 * 它继承自 SliderBase，并为字符串类型的值提供了具体的实现，
 * 包括值与字符串之间的转换（在这种情况下通常是直接使用）以及特定类型的信号。
 * 字符串滑块通常不涉及数值拖拽调整，因此删除了 SetDragMultiplier 方法。
 */
class StringSlider : public SliderBase {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit StringSlider(QWidget* parent = nullptr);

  /**
   * @brief 删除基类中的 SetDragMultiplier 方法。
   *
   * 对于字符串滑块，拖拽乘数的概念通常不适用。
   * @param d 拖拽乘数值 (未使用)。
   */
  void SetDragMultiplier(const double& d) = delete;

  /**
   * @brief 获取滑块当前的字符串值。
   * @return 返回 QString 类型的值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QString GetValue() const;

  /**
   * @brief 设置滑块的字符串值。
   * @param v 要设置的 QString 类型的值。
   */
  void SetValue(const QString& v);

  /**
   * @brief 设置滑块的默认字符串值。
   * @param v 要设置的默认 QString 类型的值。
   */
  void SetDefaultValue(const QString& v);

 signals:
  /**
   * @brief 当滑块的字符串数值发生改变时发出此信号。
   * @param str 新的 QString 类型的值。
   */
  void ValueChanged(const QString& str);

 protected:
  /**
   * @brief 重写基类方法，将 QVariant 类型的值（内部应为 QString）转换为 QString 以供显示。
   *
   * 对于字符串滑块，这通常是直接返回值本身。
   * @param value 要转换的 QVariant 值。
   * @return 返回值的字符串表示。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QString ValueToString(const QVariant& value) const override;

  /**
   * @brief 重写基类方法，将 QString 转换为 QVariant 类型的值（内部为 QString）。
   *
   * 对于字符串滑块，这通常是直接用输入字符串构造 QVariant。
   * @param s 要转换的 QString。
   * @param ok 指向布尔值的指针，用于指示转换是否成功 (对于字符串通常总是成功)。
   * @return 返回转换后的 QVariant 值。
   */
  QVariant StringToValue(const QString& s, bool* ok) const override;

  /**
   * @brief 重写基类方法，当滑块的值发生改变时，发出特定类型的 ValueChanged(const QString&) 信号。
   * @param value 改变后的新值 (QVariant，内部为 QString)。
   */
  void ValueSignalEvent(const QVariant& value) override;
};

}  // namespace olive

#endif  // STRINGSLIDER_H
