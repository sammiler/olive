#ifndef COLORWHEELWIDGET_H
#define COLORWHEELWIDGET_H

#include <QOpenGLWidget> // Qt OpenGL 控件基类 - 注意：实际基类是 ColorSwatchWidget -> QWidget
#include <QPixmap>       // Qt 图像类，用于缓存渲染结果
#include <QPoint>        // Qt 二维点类
#include <QWidget>       // Qt 控件基类

#include "colorswatchwidget.h" // 引入颜色样本控件基类

// 前向声明 Qt 类
class QResizeEvent;
class QPaintEvent;

namespace olive {

/**
 * @brief ColorWheelWidget 类是一个用于显示和选择颜色的色轮控件。
 *
 * 它继承自 ColorSwatchWidget，并实现了通过点击或拖动色轮来选择颜色的功能。
 * 色轮的渲染结果会被缓存以提高性能。
 * 它使用 HSL/HSV 颜色模型（通常是色相/饱和度在轮上，亮度/值可能由其他控件控制或固定）。
 */
class ColorWheelWidget : public ColorSwatchWidget {
  Q_OBJECT // Qt 元对象系统宏，用于支持信号和槽机制

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit ColorWheelWidget(QWidget* parent = nullptr);

 signals:
  /**
   * @brief 当色轮的直径（或有效半径）发生改变时发出此信号。
   *
   * 这通常在控件大小调整后发生。
   * @param radius 色轮的半径（或直径的一半）。
   */
  void DiameterChanged(int radius);

 protected:
  /**
   * @brief 根据屏幕坐标（控件内）获取色轮上对应的颜色。
   *
   * 此方法被重写以实现从色轮的特定点击位置计算颜色值。
   * @param p 控件坐标系中的点。
   * @return 返回在点 p 处从色轮上拾取到的 Color 对象。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Color GetColorFromScreenPos(const QPoint& p) const override;

  /**
   * @brief 控件大小调整事件处理函数。
   *
   * 当控件大小改变时调用此函数。它会触发色轮的重绘和缓存更新。
   * @param e 尺寸调整事件指针。
   */
  void resizeEvent(QResizeEvent* e) override;

  /**
   * @brief 绘制事件处理函数。
   *
   * 在此函数中渲染色轮。如果已有缓存的图像且不需要强制重绘，则直接绘制缓存。
   * @param e 绘制事件指针。
   */
  void paintEvent(QPaintEvent* e) override;

  /**
   * @brief 当选定颜色发生改变时的事件处理函数（由基类或外部调用）。
   *
   * 此方法被重写以在选定颜色改变时执行特定操作，例如更新色轮上选择器的位置
   * （如果颜色变化是外部引起的）。
   * @param c 新的选定颜色。
   * @param external 布尔值，指示颜色变化是否来自外部。
   */
  void SelectedColorChangedEvent(const Color& c, bool external) override;

 private:
  /**
   * @brief 获取色轮的有效直径。
   *
   * 通常是控件宽度和高度中的较小值。
   * @return 色轮的直径。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] int GetDiameter() const;

  /**
   * @brief 获取色轮的有效半径。
   * @return 色轮的半径。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] qreal GetRadius() const;

  /**
   * @brief 用于辅助几何计算的三角形结构体。
   *
   * 存储直角三角形的对边、邻边和斜边长度。
   */
  struct Triangle {
    qreal opposite;   ///< 对边长度。
    qreal adjacent;   ///< 邻边长度。
    qreal hypotenuse; ///< 斜边长度。
  };

  /**
   * @brief 根据中心点和目标点计算三角形的几何属性。
   * @param center 色轮的中心点。
   * @param p 目标点。
   * @return 返回一个 Triangle 结构体，包含计算得到的边长。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] static Triangle GetTriangleFromCoords(const QPoint& center, const QPoint& p);
  /**
   * @brief 根据中心点和目标点的 x, y 坐标（相对于中心点）计算三角形的几何属性。
   * @param center 色轮的中心点 (用于确定原点，但实际计算使用相对坐标 y, x)。
   * @param y 目标点相对于中心点的 y 坐标差。
   * @param x 目标点相对于中心点的 x 坐标差。
   * @return 返回一个 Triangle 结构体，包含计算得到的边长。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] static Triangle GetTriangleFromCoords(const QPoint& center, qreal y, qreal x);

  /**
   * @brief 根据三角形的几何属性（通常代表色轮上的位置）计算颜色。
   *
   * 这里的颜色模型可能是 HSV/HSL，其中角度对应色相，到中心的距离对应饱和度。
   * @param tri 描述色轮上位置的 Triangle 结构体。
   * @return 返回计算得到的 Color 对象。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Color GetColorFromTriangle(const Triangle& tri) const;
  /**
   * @brief 根据给定的颜色计算其在色轮上的对应坐标。
   *
   * 这是 GetColorFromTriangle 的逆运算。
   * @param c 要在色轮上定位的 Color 对象。
   * @return 返回颜色 c 在色轮上的 QPoint 坐标。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QPoint GetCoordsFromColor(const Color& c) const;

  QPixmap cached_wheel_; ///< 缓存的色轮图像，用于优化重绘性能。

  float val_; ///< 可能代表颜色的亮度 (Value) 或亮度 (Lightness)，通常与色轮结合使用以确定最终颜色。

  bool force_redraw_; ///< 标记是否需要强制重绘色轮（例如，在尺寸改变后）。
};

}  // namespace olive

#endif  // COLORWHEELWIDGET_H
