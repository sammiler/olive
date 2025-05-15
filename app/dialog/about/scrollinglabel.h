#ifndef SCROLLINGLABEL_H
#define SCROLLINGLABEL_H

#include <QTimer>    // 用于动画定时器
#include <QWidget>   // QWidget 基类
#include <QStringList> // 用于存储多行文本
#include <QPaintEvent> // 为了 paintEvent 参数
#include <QPainter>  // 如果在 paintEvent 中直接使用 (通常是的)

#include "common/define.h" // 可能包含一些通用定义

namespace olive {

/**
 * @brief 实现文本自动滚动效果的自定义标签控件。
 *
 *此类继承自 QWidget，用于显示可以垂直滚动的多行文本。
 * 滚动动画由 QTimer 控制，通常用于在有限的空间内展示较长的文本内容，
 * 例如在“关于”对话框中显示许可证或贡献者名单。
 */
class ScrollingLabel : public QWidget {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个 ScrollingLabel 对象。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit ScrollingLabel(QWidget* parent = nullptr);

  /**
   * @brief 使用初始文本列表构造一个 ScrollingLabel 对象。
   * @param text 要显示的文本行列表。
   * @param parent 父 QWidget 对象指针，默认为 nullptr。
   */
  explicit ScrollingLabel(const QStringList& text, QWidget* parent = nullptr);

  // 默认析构函数通常足够，因为 timer_ 如果有父对象会被自动清理。
  // ~ScrollingLabel() override;

  /**
   * @brief 设置要在标签中显示的文本内容。
   * @param text 包含多行文本的 QStringList。
   */
  void SetText(const QStringList& text);

  /**
   * @brief 开始滚动动画。
   * 启动内部的 QTimer。
   */
  void StartAnimating() { timer_.start(); }

  /**
   * @brief 停止滚动动画。
   * 停止内部的 QTimer。
   */
  void StopAnimating() { timer_.stop(); }

 protected:
  /**
   * @brief 重写 QWidget::paintEvent() 以绘制滚动的文本内容。
   * @param e 指向 QPaintEvent 对象的指针，包含绘制事件的信息。
   */
  void paintEvent(QPaintEvent* e) override;

 private:
  /**
   * @brief (已注释掉或未使用的函数) 设置扫描线（像素行）的透明度。
   * 此函数可能用于实现文本边缘的淡入淡出效果，但当前未在 ScrollingLabel 的典型实现中直接调用。
   * @param scan_line 指向扫描线像素数据的指针 (uchar* 类型)。
   * @param width 扫描线的宽度（像素数）。
   * @param channels 每个像素的通道数 (例如 RGB 为 3, RGBA 为 4)。
   * @param mul 透明度乘数 (0.0 到 1.0)。
   */
  static void SetOpacityOfScanLine(uchar* scan_line, int width, int channels, double mul);

  /**
   * @brief 定义文本绘制时的最小行高（像素）。
   * 用于确保即使字体很小，行间距也不会过于拥挤。
   */
  static const int kMinLineHeight;

  /**
   * @brief 存储要在标签中显示的文本行列表。
   */
  QStringList text_;

  /**
   * @brief 根据当前字体和文本内容计算得到的总文本高度（像素）。
   */
  int text_height_{0}; // 使用花括号进行值初始化

  /**
   * @brief 用于驱动滚动动画的 QTimer 对象。
   */
  QTimer timer_;

  /**
   * @brief 当前的动画滚动偏移量或状态。
   * 其具体含义取决于 `paintEvent` 和 `AnimationUpdate` 中的实现逻辑。
   * 通常表示文本垂直滚动的像素量。
   */
  int animate_{0}; // 使用花括号进行值初始化

 private slots:
  /**
   * @brief 当 QTimer 超时时调用的槽函数，用于更新动画状态并触发重绘。
   * 此函数会修改 `animate_` 变量，然后调用 `update()` 来请求重新绘制控件。
   */
  void AnimationUpdate();
};

}  // namespace olive

#endif  // SCROLLINGLABEL_H