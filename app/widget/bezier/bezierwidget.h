#ifndef BEZIERWIDGET_H
#define BEZIERWIDGET_H

#include <olive/core/core.h>  // 引入 Olive 核心库，可能包含 Bezier 结构定义
#include <QCheckBox>          // Qt 复选框控件
#include <QWidget>            // Qt 控件基类

#include "widget/slider/floatslider.h"  // 引入自定义的浮点数滑块控件

namespace olive {

using namespace core;  // 使用 olive::core 命名空间，方便直接使用该命名空间下的类型，如 Bezier

/**
 * @brief BezierWidget 类是一个用于编辑贝塞尔曲线参数的自定义 Qt 控件。
 *
 * 该控件提供了一系列浮点数滑块 (FloatSlider)，允许用户调整
 * 贝塞尔曲线的起点、终点以及两个控制点的 x 和 y 坐标。
 * 当贝塞尔曲线的任何参数发生变化时，会发出 ValueChanged 信号。
 */
class BezierWidget : public QWidget {
 Q_OBJECT  // Qt 元对象系统宏，用于支持信号和槽机制
     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit BezierWidget(QWidget *parent = nullptr);

  /**
   * @brief 获取当前控件所表示的贝塞尔曲线对象。
   * @return 返回一个 Bezier 对象，包含当前设置的曲线参数。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Bezier GetValue() const;

  /**
   * @brief 设置控件显示的贝塞尔曲线参数。
   * @param b 要设置的 Bezier 对象。
   */
  void SetValue(const Bezier &b);

  /**
   * @brief 获取控制贝塞尔曲线起点 x 坐标的滑块。
   * @return 指向起点 x 坐标 FloatSlider 控件的指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] FloatSlider *x_slider() const { return x_slider_; }

  /**
   * @brief 获取控制贝塞尔曲线起点 y 坐标的滑块。
   * @return 指向起点 y 坐标 FloatSlider 控件的指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] FloatSlider *y_slider() const { return y_slider_; }

  /**
   * @brief 获取控制贝塞尔曲线第一个控制点 x 坐标的滑块。
   * @return 指向第一个控制点 x 坐标 FloatSlider 控件的指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] FloatSlider *cp1_x_slider() const { return cp1_x_slider_; }

  /**
   * @brief 获取控制贝塞尔曲线第一个控制点 y 坐标的滑块。
   * @return 指向第一个控制点 y 坐标 FloatSlider 控件的指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] FloatSlider *cp1_y_slider() const { return cp1_y_slider_; }

  /**
   * @brief 获取控制贝塞尔曲线第二个控制点 x 坐标的滑块。
   * @return 指向第二个控制点 x 坐标 FloatSlider 控件的指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] FloatSlider *cp2_x_slider() const { return cp2_x_slider_; }

  /**
   * @brief 获取控制贝塞尔曲线第二个控制点 y 坐标的滑块。
   * @return 指向第二个控制点 y 坐标 FloatSlider 控件的指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] FloatSlider *cp2_y_slider() const { return cp2_y_slider_; }

 signals:
  /**
   * @brief 当贝塞尔曲线的任何参数值发生改变时，发射此信号。
   */
  void ValueChanged();

 private:
  FloatSlider *x_slider_;      ///< 控制贝塞尔曲线起点 x 坐标的滑块。
  FloatSlider *y_slider_;      ///< 控制贝塞尔曲线起点 y 坐标的滑块。
  FloatSlider *cp1_x_slider_;  ///< 控制贝塞尔曲线第一个控制点 x 坐标的滑块。
  FloatSlider *cp1_y_slider_;  ///< 控制贝塞尔曲线第一个控制点 y 坐标的滑块。
  FloatSlider *cp2_x_slider_;  ///< 控制贝塞尔曲线第二个控制点 x 坐标的滑块。
  FloatSlider *cp2_y_slider_;  ///< 控制贝塞尔曲线第二个控制点 y 坐标的滑块。
};

}  // namespace olive

#endif  // BEZIERWIDGET_H
