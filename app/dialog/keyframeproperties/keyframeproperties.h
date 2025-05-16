#ifndef KEYFRAMEPROPERTIESDIALOG_H
#define KEYFRAMEPROPERTIESDIALOG_H

#include <QComboBox>  // 下拉选择框控件 (用于关键帧类型)
#include <QDialog>    // QDialog 基类
#include <QGroupBox>  // 分组框控件 (用于贝塞尔控制柄)
#include <QWidget>    // 为了 QWidget* parent 参数
#include <vector>     // 为了 std::vector<NodeKeyframe*>

// Olive 内部头文件
#include "node/keyframe.h"  // 包含 NodeKeyframe 类和 NodeKeyframe::Type 枚举
// 假设 floatslider.h 声明了 FloatSlider 类
#include "widget/slider/floatslider.h"
// 假设 rationalslider.h 声明了 RationalSlider 类，并且 rational 类型已定义
#include "widget/slider/rationalslider.h"
// #include "common/define.h" // 如果 rational 等类型未通过其他头文件包含

namespace olive {

/**
 * @brief “关键帧属性”对话框类。
 *
 * 此对话框用于显示和编辑一个或多个选定关键帧的共同属性。
 * 用户可以修改关键帧的时间位置、插值类型（例如线性、恒定、贝塞尔），
 * 以及在选择贝塞尔插值类型时调整其入和出控制柄的坐标。
 */
class KeyframePropertiesDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 KeyframePropertiesDialog 对象。
   * @param keys 指向一个包含要编辑的 NodeKeyframe 对象指针的 std::vector 的常量引用。
   * 对话框将显示这些关键帧的共同属性，并允许批量修改。
   * @param timebase 关键帧所在上下文的时间基准 (例如，序列的帧率)，用于 RationalSlider 显示时间。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  KeyframePropertiesDialog(const std::vector<NodeKeyframe*>& keys, const rational& timebase, QWidget* parent = nullptr);

  // ~KeyframePropertiesDialog() override; // 默认析构函数通常足够

 public slots:
  /**
   * @brief 重写 QDialog::accept() 槽函数。
   * 当用户点击“确定”或等效按钮以应用更改时调用。
   * 此函数会将对话框中UI控件设置的属性值（如时间、类型、贝塞尔控制点）
   * 应用到构造时传入的所有关键帧 (`keys_`)。
   * 这些更改通常应该通过撤销/重做框架来处理（尽管在此头文件中未直接显示撤销命令的创建）。
   */
  void accept() override;

 private:
  /**
   * @brief 静态辅助函数，用于设置贝塞尔控制柄滑块的初始状态。
   * @param slider 指向要设置的 FloatSlider 控件的指针。
   * @param all_same 一个布尔值，指示所有选定关键帧的对应贝塞尔控制柄值是否相同。
   * 如果为 false，滑块可能显示为“多值”或禁用状态。
   * @param value 如果 `all_same` 为 true，则此值为要设置到滑块上的共同值。
   */
  static void SetUpBezierSlider(FloatSlider* slider, bool all_same, double value);

  /**
   * @brief 存储指向要编辑的关键帧对象的向量的常量引用。
   * 对话框通过此引用访问和修改关键帧。
   */
  const std::vector<NodeKeyframe*>& keys_;

  /**
   * @brief 关键帧所在上下文的时间基准。
   * 用于 RationalSlider 正确显示和解释时间值。
   */
  rational timebase_;

  /**
   * @brief 指向 RationalSlider 控件的指针，用于编辑关键帧的时间位置。
   */
  RationalSlider* time_slider_;

  /**
   * @brief 指向 QComboBox 控件的指针，用于选择关键帧的插值类型
   * (例如 NodeKeyframe::kLinear, NodeKeyframe::kConstant, NodeKeyframe::kBezier)。
   */
  QComboBox* type_select_;

  /**
   * @brief 指向 QGroupBox 控件的指针，用于容纳贝塞尔曲线入/出控制柄的设置滑块。
   * 当关键帧类型不是贝塞尔时，此分组框通常会被禁用或隐藏。
   */
  QGroupBox* bezier_group_;

  /**
   * @brief 指向 FloatSlider 控件的指针，用于编辑贝塞尔曲线入控制柄的X坐标。
   */
  FloatSlider* bezier_in_x_slider_;

  /**
   * @brief 指向 FloatSlider 控件的指针，用于编辑贝塞尔曲线入控制柄的Y坐标。
   */
  FloatSlider* bezier_in_y_slider_;

  /**
   * @brief 指向 FloatSlider 控件的指针，用于编辑贝塞尔曲线出控制柄的X坐标。
   */
  FloatSlider* bezier_out_x_slider_;

  /**
   * @brief 指向 FloatSlider 控件的指针，用于编辑贝塞尔曲线出控制柄的Y坐标。
   */
  FloatSlider* bezier_out_y_slider_;

 private slots:
  /**
   * @brief 当 `type_select_` 下拉框中选择的关键帧插值类型发生变化时调用的槽函数。
   * 此函数通常会根据新选择的类型来更新UI的状态，例如，
   * 如果选择了贝塞尔类型，则启用 `bezier_group_` 中的控制柄编辑控件；
   * 否则禁用它们。
   * @param index 新选中的关键帧类型在下拉框中的索引。
   */
  void KeyTypeChanged(int index);
};

}  // namespace olive

#endif  // KEYFRAMEPROPERTIESDIALOG_H