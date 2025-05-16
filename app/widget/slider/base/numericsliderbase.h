#ifndef NUMERICSLIDERBASE_H
#define NUMERICSLIDERBASE_H

#include "sliderbase.h" // 滑块基类
#include <QVariant>     // Qt 通用数据类型类 (用于存储不同类型的数值)
#include <QWidget>      // Qt 控件基类 (SliderBase 的基类)
#include <QObject>      // Qt 对象模型基类 (SliderBase 的基类)

// 前向声明项目内部类 (根据用户要求，不添加)
// class SliderLadder; // 在 sliderladder.h 中定义，此处作为成员指针类型

namespace olive {

/**
 * @brief NumericSliderBase 类是所有处理数值类型（如整数、浮点数）的滑块控件的基类。
 *
 * 它继承自 SliderBase，并添加了与数值相关的特定功能，
 * 例如处理拖拽调整值、值范围（最小值/最大值）、偏移量以及可选的“梯形调整器”(SliderLadder)
 * 以便进行更精细的数值调整。
 */
class NumericSliderBase : public SliderBase {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit NumericSliderBase(QWidget* parent = nullptr);

  /**
   * @brief 设置梯形调整器 (SliderLadder) 中元素的数量。
   * @param b 元素的数量。
   */
  void SetLadderElementCount(int b) { ladder_element_count_ = b; }

  /**
   * @brief 设置拖拽乘数。
   *
   * 当用户拖动滑块时，实际值的变化量会乘以这个乘数，
   * 允许用户调整拖拽的灵敏度。
   * @param d 拖拽乘数值。
   */
  void SetDragMultiplier(const double& d);

  /**
   * @brief 设置滑块的偏移量。
   *
   * 偏移量可能会影响值的显示或内部计算。
   * @param v 要设置的偏移量 (QVariant 类型)。
   */
  void SetOffset(const QVariant& v);

  /**
   * @brief 检查滑块当前是否正在被用户拖拽。
   * @return 如果正在拖拽，则返回 true；否则返回 false。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool IsDragging() const;

 protected:
  /**
   * @brief 获取当前设置的偏移量。
   * @return 返回一个 const 引用，指向 QVariant 类型的偏移量。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const QVariant& GetOffset() const { return offset_; }

  /**
   * @brief 虚函数，用于在拖拽过程中根据起始值和拖拽距离调整数值。
   *
   * 派生类可以重写此函数以实现特定类型的数值调整逻辑。
   * @param start 拖拽开始时的原始值。
   * @param drag 鼠标拖拽的距离或一个表示拖拽程度的因子。
   * @return 返回调整后的新值 (QVariant 类型)。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] virtual QVariant AdjustDragDistanceInternal(const QVariant& start, const double& drag) const;

  /**
   * @brief 内部方法，用于设置滑块的最小值。
   * @param v 最小值 (QVariant 类型)。
   */
  void SetMinimumInternal(const QVariant& v);

  /**
   * @brief 内部方法，用于设置滑块的最大值。
   * @param v 最大值 (QVariant 类型)。
   */
  void SetMaximumInternal(const QVariant& v);

  /**
   * @brief 虚函数，比较两个 QVariant 值，判断前者是否大于后者。
   *
   * 派生类应重写此函数以提供特定数值类型的比较逻辑。
   * @param lhs 左操作数。
   * @param rhs 右操作数。
   * @return 如果 lhs 大于 rhs，则返回 true。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] virtual bool ValueGreaterThan(const QVariant& lhs, const QVariant& rhs) const;

  /**
   * @brief 虚函数，比较两个 QVariant 值，判断前者是否小于后者。
   *
   * 派生类应重写此函数以提供特定数值类型的比较逻辑。
   * @param lhs 左操作数。
   * @param rhs 右操作数。
   * @return 如果 lhs 小于 rhs，则返回 true。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] virtual bool ValueLessThan(const QVariant& lhs, const QVariant& rhs) const;

  /**
   * @brief 重写基类的方法，检查当前是否可以设置值（例如，控件是否已启用）。
   * @return 如果可以设置值，则返回 true。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool CanSetValue() const override;

 private:
  /**
   * @brief 检查当前是否正在使用梯形调整器 (SliderLadder)。
   * @return 如果梯形调整器已创建并可见，则返回 true。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool UsingLadders() const;

  /**
   * @brief 重写基类的方法，在设置值之前对其进行调整（例如，确保值在最小/最大范围内）。
   * @param value 要调整的原始值。
   * @return 返回调整后的值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QVariant AdjustValue(const QVariant& value) const override;

  SliderLadder* drag_ladder_; ///< 指向梯形调整器控件的指针，用于精细调整数值。

  int ladder_element_count_; ///< 梯形调整器中显示的元素数量。

  bool dragged_; ///< 标记滑块当前是否被用户拖拽（通过标签）。

  bool has_min_;      ///< 标记是否设置了最小值。
  QVariant min_value_; ///< 存储滑块的最小值。

  bool has_max_;      ///< 标记是否设置了最大值。
  QVariant max_value_; ///< 存储滑块的最大值。

  double dragged_diff_; ///< 存储拖拽过程中鼠标位置与起始位置的差值。

  QVariant drag_start_value_; ///< 存储开始拖拽（通过标签）时的滑块值。

  QVariant offset_; ///< 存储滑块的偏移量。

  double drag_multiplier_; ///< 拖拽乘数，影响拖拽灵敏度。

  bool setting_drag_value_; ///< 标记当前是否正在通过拖拽设置值，以避免信号的递归触发。

  /**
   * @brief 一个静态标志，指示应用程序中是否有任何效果相关的滑块正在被拖动。
   *
   * 这可能用于全局状态管理，例如，在拖动滑块时禁用某些其他UI更新。
   */
  static bool effects_slider_is_being_dragged_;

 private slots:
  /**
   * @brief 当滑块的标签部分被按下时调用的槽函数。
   *
   * 通常会初始化拖拽操作或显示梯形调整器。
   */
  void LabelPressed();

  /**
   * @brief 重新定位梯形调整器控件（通常在滑块或其父窗口移动或大小改变时）。
   */
  void RepositionLadder();

  /**
   * @brief 当梯形调整器 (SliderLadder) 的值被拖动改变时调用的槽函数。
   * @param value 梯形调整器报告的原始值变化量。
   * @param multiplier 梯形调整器当前使用的乘数。
   */
  void LadderDragged(int value, double multiplier);

  /**
   * @brief 当梯形调整器 (SliderLadder) 上的拖拽操作结束（鼠标释放）时调用的槽函数。
   */
  void LadderReleased();
};

}  // namespace olive

#endif  // NUMERICSLIDERBASE_H
