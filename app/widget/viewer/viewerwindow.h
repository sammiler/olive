#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H

#include <QWidget>  // 包含 QWidget 类的头文件

#include "viewerdisplay.h"  // 包含 ViewerDisplayWidget 类的头文件

namespace olive {

/**
 * @brief 查看器窗口类。
 *
 * 这是一个 QWidget，作为 ViewerDisplayWidget 的容器。
 * 它处理与视频参数、分辨率和像素宽高比相关的设置，
 * 并将这些设置传递给其内部的显示小部件。
 * 它还处理键盘事件和关闭事件。
 */
class ViewerWindow : public QWidget {
 Q_OBJECT  // Q_OBJECT宏，用于支持 Qt 的元对象系统
     public :
     /**
      * @brief ViewerWindow 的显式构造函数。
      * @param parent 父 QWidget 指针，默认为 nullptr。
      */
     explicit ViewerWindow(QWidget* parent = nullptr);

  /**
   * @brief 获取内部的 ViewerDisplayWidget 指针。
   * @return 返回指向 ViewerDisplayWidget 实例的指针。
   */
  [[nodiscard]] ViewerDisplayWidget* display_widget() const;

  /**
   * @brief 设置视频参数，用于调整最终画面的宽高比。
   *
   * 相当于调用 SetResolution 和 SetPixelAspectRatio，但速度稍快，
   * 因为我们只计算一次矩阵而不是两次。
   * @param params 包含视频参数的对象。
   */
  void SetVideoParams(const VideoParams& params);

  /**
   * @brief 设置分辨率，用于调整最终画面的宽高比。
   * @param width 宽度。
   * @param height 高度。
   */
  void SetResolution(int width, int height);

  /**
   * @brief 设置像素宽高比，用于调整最终画面的宽高比。
   * @param pixel_aspect 像素宽高比。
   */
  void SetPixelAspectRatio(const rational& pixel_aspect);

 protected:
  /**
   * @brief 处理键盘按下事件。
   * @param e 键盘事件指针。
   */
  void keyPressEvent(QKeyEvent* e) override;

  /**
   * @brief 处理窗口关闭事件。
   * @param e 关闭事件指针。
   */
  void closeEvent(QCloseEvent* e) override;

 private:
  /**
   * @brief 更新内部的变换矩阵。
   * 此函数可能会根据当前的宽度、高度和像素宽高比来更新显示小部件的变换。
   */
  void UpdateMatrix();

  /**
   * @brief 存储视频内容的宽度。
   */
  int width_{};

  /**
   * @brief 存储视频内容的高度。
   */
  int height_{};

  /**
   * @brief 指向内部 ViewerDisplayWidget 实例的指针。
   * ViewerWindow 管理这个显示小部件的生命周期。
   */
  ViewerDisplayWidget* display_widget_;

  /**
   * @brief 存储像素宽高比。
   */
  rational pixel_aspect_;
};

}  // namespace olive

#endif  // VIEWERWINDOW_H
