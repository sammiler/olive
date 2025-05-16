#ifndef VIEWERSIZER_H
#define VIEWERSIZER_H

#include <olive/core/core.h>  // 包含 olive 核心库的头文件
#include <QScrollBar>         // 包含 QScrollBar 类的头文件
#include <QWidget>            // 包含 QWidget 类的头文件

namespace olive {

using namespace core;  // 使用 core 命名空间

/**
 * @brief 一个容器小部件，用于强制子小部件的宽高比。
 *
 * 使用提供的宽度和高度，此小部件计算宽高比，并强制子小部件
 * 保持在该宽高比内，并在小部件内居中。
 *
 * 宽高比通过宽度除以高度计算。如果宽高比为零（宽度或高度为0），
 * 则小部件将被隐藏，直到提供有效的尺寸。
 */
class ViewerSizer : public QWidget {
 Q_OBJECT  // Q_OBJECT宏，用于支持 Qt 的元对象系统（信号、槽等）
     public :
     /**
      * @brief ViewerSizer 的显式构造函数。
      * @param parent 父 QWidget 指针，默认为 nullptr。
      */
     explicit ViewerSizer(QWidget* parent = nullptr);

  /**
   * @brief 设置由此小部件调整的子小部件。
   *
   * ViewerSizer 会获得此小部件的所有权。如果先前已设置过小部件，则会销毁它。
   * @param widget 要设置的子 QWidget 指针。
   */
  void SetWidget(QWidget* widget);

  /**
   * @brief 获取容器的尺寸。
   * @return 返回容器的 QSize。
   */
  [[nodiscard]] QSize GetContainerSize() const;

  /**
   * @brief 缩放级别数量的常量。
   */
  static constexpr int kZoomLevelCount = 10;
  /**
   * @brief 定义的一系列缩放级别。
   */
  static constexpr double kZoomLevels[kZoomLevelCount] = {0.05, 0.1, 0.25, 0.5, 0.75, 1.0, 1.5, 2.0, 4.0, 8.0};

 public slots:
  /**
   * @brief 设置要使用的分辨率。
   *
   * 这不是查看器的实际分辨率，它用于计算宽高比。
   * @param width 子小部件的宽度。
   * @param height 子小部件的高度。
   */
  void SetChildSize(int width, int height);

  /**
   * @brief 设置像素宽高比。
   * @param pixel_aspect 像素宽高比，使用 rational 类型表示。
   */
  void SetPixelAspectRatio(const rational& pixel_aspect);

  /**
   * @brief 设置子小部件的缩放值。
   *
   * 该数字是一个整数百分比（例如，100 表示 100%）。设置为 0 以自动适应。
   * @param percent 缩放百分比。
   */
  void SetZoom(double percent);
  /**
   * @brief 以锚点为中心设置缩放。
   * @param next_scale 下一个缩放比例。
   * @param cursor_x 鼠标光标的 X 坐标（锚点）。
   * @param cursor_y 鼠标光标的 Y 坐标（锚点）。
   */
  void SetZoomAnchored(double next_scale, double cursor_x, double cursor_y);

  /**
   * @brief 处理手形拖动移动。
   * @param x X方向的移动量。
   * @param y Y方向的移动量。
   */
  void HandDragMove(int x, int y);

  /**
   * @brief 事件过滤器，用于处理子小部件的事件。
   * @param watched 被观察的对象。
   * @param event 发生的事件。
   * @return 如果事件被过滤和处理，则返回 true；否则返回 false。
   */
  bool eventFilter(QObject* watched, QEvent* event) override;

 signals:
  /**
   * @brief 请求缩放的信号。
   * 当需要应用缩放变换时发出。
   * @param matrix 缩放变换矩阵。
   */
  void RequestScale(const QMatrix4x4& matrix);

  /**
   * @brief 请求平移的信号。
   * 当需要应用平移变换时发出。
   * @param matrix 平移变换矩阵。
   */
  void RequestTranslate(const QMatrix4x4& matrix);

 protected:
  /**
   * @brief 监听调整大小事件，以确保子小部件保持正确的大小。
   * @param event 调整大小事件。
   */
  void resizeEvent(QResizeEvent* event) override;

 private:
  /**
   * @brief 主要的尺寸调整函数，调整 widget_ 以适应 aspect_ratio_（如果宽高比为0则隐藏）。
   */
  void UpdateSize();

  /**
   * @brief 获取缩放后的值。
   * @param value 原始值。
   * @return 返回缩放后的整数值。
   */
  [[nodiscard]] int GetZoomedValue(int value) const;

  /**
   * @brief 获取实际的当前缩放比例。
   * @return 返回当前的缩放比例（双精度浮点数）。
   */
  [[nodiscard]] double GetRealCurrentZoom() const;

  /**
   * @brief 指向被管理的子小部件的指针。
   *
   * 如果此指针为 nullptr，则所有尺寸调整操作均无效。
   */
  QWidget* widget_;

  /**
   * @brief 内部存储的子小部件宽度。
   */
  int width_;
  /**
   * @brief 内部存储的子小部件高度。
   */
  int height_;

  /**
   * @brief 像素宽高比。
   */
  rational pixel_aspect_;

  /**
   * @brief 内部缩放值。
   * 0 表示自动适应。
   */
  double zoom_;
  /**
   * @brief 小部件当前的实际缩放比例。
   */
  double current_widget_scale_;

  /**
   * @brief 水平滚动条。
   */
  QScrollBar* horiz_scrollbar_;
  /**
   * @brief 垂直滚动条。
   */
  QScrollBar* vert_scrollbar_;

 private slots:
  /**
   * @brief 当滚动条移动时调用的槽函数。
   * 用于更新子小部件的位置以响应滚动。
   */
  void ScrollBarMoved();
};

}  // namespace olive

#endif  // VIEWERSIZER_H
