#ifndef RESIZABLESCROLLBAR_H
#define RESIZABLESCROLLBAR_H

#include <QScrollBar>    // Qt 滚动条控件基类
#include <QMouseEvent>   // Qt 鼠标事件类
#include <QRect>         // Qt 矩形类

#include "common/define.h" // 项目通用定义

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QWidget; // QScrollBar 的基类 QAbstractSlider 的基类 QWidget

namespace olive {

/**
 * @brief ResizableScrollBar 类是一个自定义的 QScrollBar，它允许用户通过拖动其端点来调整滚动条本身的宽度/厚度。
 *
 * 例如，一个垂直滚动条可以通过拖动其顶部或底部边缘来使其变宽或变窄。
 * 它通过发出信号来通知外部有关调整大小操作的开始、过程和结束。
 */
class ResizableScrollBar : public QScrollBar {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit ResizableScrollBar(QWidget* parent = nullptr);
  /**
   * @brief 构造函数，允许指定滚动条的方向。
   * @param orientation 滚动条的方向 (Qt::Horizontal 或 Qt::Vertical)。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit ResizableScrollBar(Qt::Orientation orientation, QWidget* parent = nullptr);

 signals:
  /**
   * @brief 当用户开始拖动滚动条的调整大小手柄时发出此信号。
   * @param old_bar_width 开始调整前滚动条的宽度/厚度。
   * @param top_handle 如果拖动的是顶部/左侧手柄则为 true，如果是底部/右侧手柄则为 false。
   */
  void ResizeBegan(int old_bar_width, bool top_handle);

  /**
   * @brief 当用户拖动调整大小手柄导致滚动条宽度/厚度发生变化时发出此信号。
   * @param movement 自上次 ResizeMoved 或 ResizeBegan 信号以来，鼠标在拖动方向上的移动量。
   */
  void ResizeMoved(int movement);

  /**
   * @brief 当用户释放鼠标按钮，结束调整大小操作时发出此信号。
   */
  void ResizeEnded();

 protected:
  /**
   * @brief 重写 QWidget 的鼠标按下事件处理函数。
   *
   * 用于检测用户是否在调整大小手柄上按下了鼠标，并开始调整大小操作。
   * @param event 鼠标事件指针。
   */
  void mousePressEvent(QMouseEvent* event) override;

  /**
   * @brief 重写 QWidget 的鼠标移动事件处理函数。
   *
   * 如果正在进行调整大小操作，则根据鼠标的移动发出 ResizeMoved 信号。
   * @param event 鼠标事件指针。
   */
  void mouseMoveEvent(QMouseEvent* event) override;

  /**
   * @brief 重写 QWidget 的鼠标释放事件处理函数。
   *
   * 如果正在进行调整大小操作，则结束操作并发出 ResizeEnded 信号。
   * @param event 鼠标事件指针。
   */
  void mouseReleaseEvent(QMouseEvent* event) override;

 private:
  /**
   * @brief 获取滚动条（不包括手柄）的几何矩形区域。
   * @return 返回滚动条的 QRect。
   */
  QRect GetScrollBarRect();

  static const int kHandleWidth; ///< 定义调整大小手柄的宽度/厚度（像素）。

  /**
   * @brief MouseHandleState 枚举定义了鼠标当前在哪一个调整大小手柄上（或不在任何手柄上）。
   */
  enum MouseHandleState {
    kNotInHandle,    ///< 鼠标不在任何调整大小手柄上。
    kInTopHandle,    ///< 鼠标在顶部（或左侧，对于水平滚动条）的调整大小手柄上。
    kInBottomHandle  ///< 鼠标在底部（或右侧，对于水平滚动条）的调整大小手柄上。
  };

  /**
   * @brief 执行控件的初始化操作。
   */
  void Init();

  /**
   * @brief 根据鼠标事件获取鼠标在滚动条主轴方向上的活动位置。
   * @param event 鼠标事件指针。
   * @return 返回鼠标在滚动条主轴上的坐标值。
   */
  int GetActiveMousePos(QMouseEvent* event);

  /**
   * @brief 获取滚动条在主轴方向上的当前尺寸（宽度或高度）。
   * @return 返回滚动条的尺寸。
   */
  int GetActiveBarSize();

  MouseHandleState mouse_handle_state_; ///< 存储当前鼠标相对于调整大小手柄的状态。

  bool dragging_{}; ///< 标记当前是否正在拖动调整大小手柄。初始化为 false。

  int drag_start_point_{}; ///< 开始拖动调整大小时，鼠标在主轴方向上的初始位置。初始化为 0。
};

}  // namespace olive

#endif  // RESIZABLESCROLLBAR_H
