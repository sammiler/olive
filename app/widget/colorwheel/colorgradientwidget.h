#ifndef COLORGRADIENTGLWIDGET_H
#define COLORGRADIENTGLWIDGET_H

#include "colorswatchwidget.h"  // 引入颜色样本控件基类

// 前向声明 Qt 类，如果 ColorSwatchWidget.h 中没有包含的话
class QPaintEvent;
class QPoint;
class QPixmap;

namespace olive {

/**
 * @brief ColorGradientWidget 类是一个用于显示和选择颜色渐变的控件。
 *
 * 它继承自 ColorSwatchWidget，专门用于渲染一个颜色渐变条（水平或垂直），
 * 用户可以从中选择一个颜色。渐变的起始颜色、结束颜色以及当前选定值
 * 都可以被设置和获取。
 */
class ColorGradientWidget : public ColorSwatchWidget {
 Q_OBJECT  // Qt 元对象系统宏，用于支持信号和槽机制

     public :
     /**
      * @brief 构造函数。
      * @param orientation 渐变的方向 (水平 Qt::Horizontal 或垂直 Qt::Vertical)。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit ColorGradientWidget(Qt::Orientation orientation, QWidget* parent = nullptr);

 protected:
  /**
   * @brief 根据屏幕上的点坐标获取对应的颜色。
   *
   * 此方法被重写以从渐变中计算出指定点下的颜色。
   * @param p 控件坐标系中的点。
   * @return 返回在点 p 处的颜色。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Color GetColorFromScreenPos(const QPoint& p) const override;

  /**
   * @brief 绘制事件处理函数。
   *
   * 在此函数中渲染颜色渐变。
   * @param e 绘制事件指针。
   */
  void paintEvent(QPaintEvent* e) override;

  /**
   * @brief 当选定颜色发生改变时的事件处理函数。
   *
   * 此方法被重写以响应基类中选定颜色的变化。
   * @param c 新的选定颜色。
   * @param external 布尔值，指示颜色变化是否来自外部（非用户直接在此控件上操作）。
   */
  void SelectedColorChangedEvent(const Color& c, bool external) override;

 private:
  /**
   * @brief 在两个颜色之间进行线性插值。
   * @param a 起始颜色。
   * @param b 结束颜色。
   * @param i 当前插值点的位置。
   * @param max 插值范围的最大值。
   * @return 返回插值计算得到的颜色。
   */
  static Color LerpColor(const Color& a, const Color& b, int i, int max);

  QPixmap cached_gradient_;  ///< 缓存的渐变图像，用于提高重绘效率。

  Qt::Orientation orientation_;  ///< 渐变的方向 (水平或垂直)。

  Color start_;  ///< 渐变的起始颜色。
  Color end_;    ///< 渐变的结束颜色。

  float val_;  ///< 当前在渐变中选定的值 (通常在 0.0 到 1.0 之间)。
};

}  // namespace olive

#endif  // COLORGRADIENTGLWIDGET_H
