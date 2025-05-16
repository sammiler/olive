#ifndef SLIDERLADDER_H
#define SLIDERLADDER_H

#include <QLabel>
#include <QTimer>
#include <QWidget>

#include "common/define.h"

namespace olive {

/**
 * @brief SliderLadderElement 类代表滑块梯形调整器 (SliderLadder) 中的单个元素或层级。
 *
 * 每个元素显示一个数值和一个可选的乘数，并且可以被高亮显示。
 * 用户通过在这些元素上垂直拖动鼠标来进行精细的数值调整。
 */
class SliderLadderElement : public QWidget {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param multiplier 此梯级元素的调整乘数。
      * @param width_hint 用于计算元素宽度的提示字符串（例如，一个包含多个数字的字符串以确定最大宽度）。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit SliderLadderElement(const double& multiplier, const QString& width_hint, QWidget* parent = nullptr);

  /**
   * @brief 设置此元素的高亮状态。
   * @param e 如果为 true，则高亮显示元素；否则取消高亮。
   */
  void SetHighlighted(bool e);

  /**
   * @brief 设置此元素显示的数值文本。
   * @param value 要显示的数值的字符串表示。
   */
  void SetValue(const QString& value);

  /**
   * @brief 设置乘数标签是否可见。
   * @param e 如果为 true，则显示乘数；否则隐藏。
   */
  void SetMultiplierVisible(bool e);

  /**
   * @brief 获取此梯级元素的调整乘数。
   * @return 返回 double 类型的乘数值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] double GetMultiplier() const { return multiplier_; }

 private:
  /**
   * @brief 更新内部标签显示的文本，结合当前值和乘数（如果可见）。
   */
  void UpdateLabel();

  QLabel* label_;  ///< 用于显示数值和可选乘数的标签。

  double multiplier_;        ///< 此梯级元素的调整乘数。
  QString value_;            ///< 此梯级元素当前显示的数值的字符串形式。
  bool highlighted_;         ///< 标记此元素当前是否高亮显示。
  bool multiplier_visible_;  ///< 标记乘数部分是否应在标签中显示。
};

/**
 * @brief SliderLadder 类是一个“梯形调整器”控件，用于对数值进行多级精细调整。
 *
 * 它通常在用户点击并拖动一个滑块的标签时弹出，显示为一系列垂直排列的
 * SliderLadderElement。用户可以通过在这些元素上垂直拖动鼠标来以不同的
 * 精度（由元素的乘数决定）调整关联滑块的值。
 */
class SliderLadder : public QFrame {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param drag_multiplier 基础的拖拽乘数，与每个梯级元素的乘数结合使用。
      * @param nb_outer_values 梯形调整器外部（通常是精度较低的）层级的数量。
      * @param width_hint 用于计算梯级元素宽度的提示字符串。
      * @param parent 父控件指针，默认为 nullptr。
      */
     SliderLadder(double drag_multiplier, int nb_outer_values, const QString& width_hint, QWidget* parent = nullptr);

  /**
   * @brief 析构函数。
   */
  ~SliderLadder() override;

  /**
   * @brief 设置梯形调整器中所有元素当前显示的基准值。
   * @param s 基准值的字符串表示。
   */
  void SetValue(const QString& s);

  /**
   * @brief 开始监听鼠标输入，激活梯形调整器的拖拽功能。
   *
   * 通常在梯形调整器显示时调用。
   */
  void StartListeningToMouseInput();

 protected:
  /**
   * @brief 重写 QWidget 的鼠标释放事件处理函数。
   *
   * 当用户释放鼠标按钮时，发出 Released() 信号并可能隐藏梯形调整器。
   * @param event 鼠标事件指针。
   */
  void mouseReleaseEvent(QMouseEvent* event) override;

  /**
   * @brief 重写 QWidget 的关闭事件处理函数。
   *
   * 当梯形调整器窗口被关闭时，发出 Released() 信号。
   * @param event 关闭事件指针。
   */
  void closeEvent(QCloseEvent* event) override;

 signals:
  /**
   * @brief 当用户通过拖动梯形调整器改变值时发出此信号。
   * @param value 原始值的变化量（通常是基于像素的拖动距离）。
   * @param multiplier 当前活动梯级元素的组合乘数。
   */
  void DraggedByValue(int value, double multiplier);

  /**
   * @brief 当用户在梯形调整器上释放鼠标按钮（结束拖拽）或关闭梯形调整器时发出此信号。
   */
  void Released();

 private:
  /**
   * @brief 检查当前是否正在使用（显示）梯形调整器。
   * @return 如果正在使用，则返回 true。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool UsingLadders() const;

  int drag_start_x_;  ///< 开始拖拽时鼠标的全局 X 坐标。
  int drag_start_y_;  ///< 开始拖拽时鼠标的全局 Y 坐标。
  int wrap_count_;    ///< 鼠标在屏幕边缘环绕的次数，用于处理大幅度拖拽。

  QList<SliderLadderElement*> elements_;  ///< 存储构成梯形调整器的所有 SliderLadderElement 的列表。

  int active_element_;  ///< 当前鼠标悬停或拖拽在其上的梯级元素的索引。

  QTimer drag_timer_;  ///< 用于在拖拽过程中定时更新和发出信号的定时器。

  QScreen* screen_;  ///< 指向当前屏幕的指针，用于获取鼠标位置和屏幕几何信息。

 private slots:
  /**
   * @brief 定时器超时槽函数，在拖拽过程中被 drag_timer_ 定期调用。
   *
   * 此函数会检查鼠标位置，更新活动梯级元素，并发出 DraggedByValue 信号。
   */
  void TimerUpdate();
};

}  // namespace olive

#endif  // SLIDERLADDER_H
