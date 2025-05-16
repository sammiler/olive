#ifndef RATIONALSLIDER_H
#define RATIONALSLIDER_H

#include <olive/core/core.h> // Olive 核心库，包含 rational 类型定义
#include <QMouseEvent>       // Qt 鼠标事件类 (虽然未直接使用，但基类可能使用)
#include <QWidget>           // Qt 控件基类 (DecimalSliderBase 的基类 NumericSliderBase 的基类 SliderBase 的基类)
#include <QVariant>          // Qt 通用数据类型类 (用于重写基类方法)
#include <QString>           // Qt 字符串类 (用于重写基类方法)
#include <QVector>           // Qt 动态数组容器 (用于 disabled_ 成员)


#include "base/decimalsliderbase.h" // 带小数的数值滑块基类

namespace olive {

using namespace core; // 使用 olive::core 命名空间，方便直接使用 rational 等类型

/**
 * @brief RationalSlider 类是一个基于 olive::rational 类型的滑块控件。
 *
 * 此滑块可以将 rational 类型的值显示为时间码（掉帧或不掉帧）、时间戳（帧数），
 * 或浮点数（秒）。它支持设置时间基准，并允许用户（如果未锁定）在不同的显示类型之间切换。
 */
class RationalSlider : public DecimalSliderBase {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief DisplayType 枚举定义了 rational 值可以如何被解释和显示。
   */
  enum DisplayType {
    kTime,     ///< 以时间码形式显示 (例如 HH:MM:SS:FF 或 HH:MM:SS;FF)。
    kFloat,    ///< 以浮点数形式显示 (通常代表秒)。
    kRational  ///< 以原始分数形式显示 (例如 num/den)。
  };

  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit RationalSlider(QWidget* parent = nullptr);

  /**
   * @brief 获取滑块当前的 rational 类型的值。
   * @return 返回 rational 类型的值。
   */
  rational GetValue(); // 注意：通常 getter 会声明为 const，但此处遵循原始代码

  /**
   * @brief 设置滑块的默认 rational 类型的值。
   * @param r 要设置的默认 rational 值。
   */
  void SetDefaultValue(const rational& r);

  /**
   * @brief 设置滑块允许的最小 rational 类型的值。
   * @param d 最小 rational 值。
   */
  void SetMinimum(const rational& d);

  /**
   * @brief 设置滑块允许的最大 rational 类型的值。
   * @param d 最大 rational 值。
   */
  void SetMaximum(const rational& d);

  /**
   * @brief 设置滑块值的显示类型。
   * @param type DisplayType 枚举值。
   */
  void SetDisplayType(const DisplayType& type);

  /**
   * @brief 设置用户是否可以更改显示类型。
   * @param e 如果为 true，则锁定显示类型，用户不能通过菜单更改；否则允许更改。
   */
  void SetLockDisplayType(bool e);

  /**
   * @brief 获取用户是否可以更改显示类型的状态。
   * @return 如果显示类型被锁定，则返回 true；否则返回 false。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool GetLockDisplayType() const;

  /**
   * @brief 在显示类型选择菜单中禁用指定的显示类型。
   * @param type 要禁用的 DisplayType。
   */
  void DisableDisplayType(DisplayType type);

 public slots:
  /**
   * @brief 设置滑块的时间基准。
   *
   * 时间基准（通常是帧率）会影响时间码的显示和值的最小增量。
   * @param timebase 新的时间基准 (rational 类型)。
   */
  void SetTimebase(const rational& timebase);

  /**
   * @brief 设置滑块的 rational 类型的值。
   * @param d 要设置的 rational 值。
   */
  void SetValue(const rational& d);

 protected:
  /**
   * @brief 重写基类方法，将 QVariant 类型的值（内部应为 rational）转换为 QString 以供显示。
   *
   * 此实现会根据当前的 display_type_ 和 timebase_ 来格式化 rational 值。
   * @param v 要转换的 QVariant 值。
   * @return 返回值的字符串表示。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QString ValueToString(const QVariant& v) const override;

  /**
   * @brief 重写基类方法，将 QString 转换为 QVariant 类型的值（内部为 rational）。
   *
   * 此实现会尝试根据当前的 display_type_ 和 timebase_ 来解析字符串。
   * @param s 要转换的 QString。
   * @param ok 指向布尔值的指针，用于指示转换是否成功。
   * @return 返回转换后的 QVariant 值。
   */
  QVariant StringToValue(const QString& s, bool* ok) const override;

  /**
   * @brief 重写基类方法，用于在拖拽过程中根据起始值和拖拽距离调整数值。
   *
   * 针对 rational 类型进行调整，并考虑时间基准。
   * @param start 拖拽开始时的原始值 (QVariant，内部为 rational)。
   * @param drag 鼠标拖拽的距离或一个表示拖拽程度的因子。
   * @return 返回调整后的新值 (QVariant，内部为 rational)。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QVariant AdjustDragDistanceInternal(const QVariant& start, const double& drag) const override;

  /**
   * @brief 重写基类方法，当滑块的值发生改变时，发出特定类型的 ValueChanged(rational) 信号。
   * @param v 改变后的新值 (QVariant，内部为 rational)。
   */
  void ValueSignalEvent(const QVariant& v) override;

  /**
   * @brief 重写基类方法，比较两个 QVariant 值（内部为 rational），判断前者是否大于后者。
   * @param lhs 左操作数。
   * @param rhs 右操作数。
   * @return 如果 lhs 大于 rhs，则返回 true。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool ValueGreaterThan(const QVariant& lhs, const QVariant& rhs) const override;

  /**
   * @brief 重写基类方法，比较两个 QVariant 值（内部为 rational），判断前者是否小于后者。
   * @param lhs 左操作数。
   * @param rhs 右操作数。
   * @return 如果 lhs 小于 rhs，则返回 true。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool ValueLessThan(const QVariant& lhs, const QVariant& rhs) const override;

 signals:
  /**
   * @brief 当滑块的 rational 数值发生改变时发出此信号。
   * @param value 新的 rational 类型的值。
   */
  void ValueChanged(rational value);

 private slots:
  /**
   * @brief 显示用于选择显示类型的上下文菜单。
   *
   * 通常在用户点击滑块标签的特定区域或通过其他方式触发。
   */
  void ShowDisplayTypeMenu();

  /**
   * @brief 当用户从显示类型菜单中选择一项时调用的槽函数。
   *
   * 此函数会更新滑块的 display_type_。
   */
  void SetDisplayTypeFromMenu();

 private:
  DisplayType display_type_; ///< 存储当前滑块值的显示类型。

  rational timebase_; ///< 存储当前滑块使用的时间基准（例如帧率）。

  bool lock_display_type_; ///< 标记用户是否可以更改显示类型。

  QVector<DisplayType> disabled_; ///< 存储在菜单中被禁用的显示类型列表。
};

}  // namespace olive

#endif  // RATIONALSLIDER_H
