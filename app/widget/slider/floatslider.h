#ifndef FLOATSLIDER_H
#define FLOATSLIDER_H

#include "base/decimalsliderbase.h"

namespace olive {

/**
 * @brief FloatSlider 类是一个专门用于处理和显示浮点数 (double) 的滑块控件。
 *
 * 它继承自 DecimalSliderBase，并增加了根据不同的显示类型（如普通值、分贝、百分比）
 * 来转换和格式化浮点数值的功能。
 */
class FloatSlider : public DecimalSliderBase {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit FloatSlider(QWidget* parent = nullptr);

  /**
   * @brief DisplayType 枚举定义了浮点数值可以如何被解释和显示。
   */
  enum DisplayType {
    kNormal,     ///< 普通数值显示，直接显示浮点值。
    kDecibel,    ///< 以分贝形式显示，通常用于音频增益等。
    kPercentage  ///< 以百分比形式显示。
  };

  /**
   * @brief 获取滑块当前的浮点数值。
   * @return 返回 double 类型的值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] double GetValue() const;

  /**
   * @brief 设置滑块的浮点数值。
   * @param d 要设置的 double 类型的值。
   */
  void SetValue(const double& d);

  /**
   * @brief 设置滑块的默认浮点数值。
   * @param d 要设置的默认 double 类型的值。
   */
  void SetDefaultValue(const double& d);

  /**
   * @brief 设置滑块允许的最小浮点数值。
   * @param d 最小 double 类型的值。
   */
  void SetMinimum(const double& d);

  /**
   * @brief 设置滑块允许的最大浮点数值。
   * @param d 最大 double 类型的值。
   */
  void SetMaximum(const double& d);

  /**
   * @brief 设置滑块值的显示类型（普通、分贝或百分比）。
   * @param type DisplayType 枚举值。
   */
  void SetDisplayType(const DisplayType& type);

  /**
   * @brief 静态方法，根据指定的显示类型将内部存储的浮点值转换为用于显示的浮点值。
   * 例如，如果 display 类型是 kDecibel，则 val (线性值) 会被转换为分贝值。
   * @param val 内部存储的原始 double 值。
   * @param display 目标显示类型。
   * @return 返回转换后用于显示的 double 值。
   */
  static double TransformValueToDisplay(double val, DisplayType display);

  /**
   * @brief 静态方法，根据指定的显示类型将用户在UI上看到或输入的值转换回内部存储的浮点值。
   * 例如，如果 display 类型是 kDecibel，则 val (分贝值) 会被转换为线性值。
   * @param val 用户界面上显示的或输入的 double 值。
   * @param display 当前的显示类型。
   * @return 返回转换后的内部存储 double 值。
   */
  static double TransformDisplayToValue(double val, DisplayType display);

  /**
   * @brief 静态方法，根据显示类型、小数位数和自动修剪设置，将浮点值转换为字符串。
   * @param val 要转换的 double 值（通常是已经过 TransformValueToDisplay 转换后的值）。
   * @param display 当前的显示类型。
   * @param decimal_places 要保留的小数位数。
   * @param autotrim_decimal_places 是否自动修剪末尾多余的零。
   * @return 返回格式化后的 QString。
   */
  static QString ValueToString(double val, DisplayType display, int decimal_places, bool autotrim_decimal_places);

 protected:
  /**
   * @brief 重写基类方法，将 QVariant 类型的值转换为 QString 以供显示。
   *
   * 此实现会考虑当前的 display_type_。
   * @param v 要转换的 QVariant 值 (内部应为 double)。
   * @return 返回值的字符串表示。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QString ValueToString(const QVariant& v) const override;

  /**
   * @brief 重写基类方法，将 QString 转换为 QVariant 类型的值。
   *
   * 此实现会考虑当前的 display_type_，将显示的字符串转换回内部的 double 值。
   * @param s 要转换的 QString。
   * @param ok 指向布尔值的指针，用于指示转换是否成功。
   * @return 返回转换后的 QVariant 值 (内部为 double)。
   */
  QVariant StringToValue(const QString& s, bool* ok) const override;

  /**
   * @brief 重写基类方法，用于在拖拽过程中根据起始值和拖拽距离调整数值。
   *
   * 针对浮点数进行调整。
   * @param start 拖拽开始时的原始值 (QVariant，内部为 double)。
   * @param drag 鼠标拖拽的距离或一个表示拖拽程度的因子。
   * @return 返回调整后的新值 (QVariant，内部为 double)。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QVariant AdjustDragDistanceInternal(const QVariant& start, const double& drag) const override;

  /**
   * @brief 重写基类方法，当滑块的值发生改变时，发出特定类型的 ValueChanged(double) 信号。
   * @param value 改变后的新值 (QVariant，内部为 double)。
   */
  void ValueSignalEvent(const QVariant& value) override;

 signals:
  /**
   * @brief 当滑块的浮点数值发生改变时发出此信号。
   * @param value 新的 double 类型的值。
   */
  void ValueChanged(double value);

 private:
  DisplayType display_type_;  ///< 存储当前滑块值的显示类型。
};

}  // namespace olive

#endif  // FLOATSLIDER_H
