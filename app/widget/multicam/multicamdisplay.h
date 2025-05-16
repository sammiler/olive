#ifndef MULTICAMDISPLAY_H
#define MULTICAMDISPLAY_H

#include "node/input/multicam/multicamnode.h" // 多机位节点类，提供多路视频输入
#include "widget/viewer/viewerdisplay.h"      // 查看器显示控件基类

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QWidget;
// class QString;
// class QVariant;

namespace olive {

/**
 * @brief MulticamDisplay 类是一个专门用于显示多机位（多摄像头）画面的控件。
 *
 * 它继承自 ViewerDisplayWidget，并与一个 MultiCamNode 关联，
 * 以网格形式（行和列）渲染来自该节点的多个视频输入流。
 * 渲染通常通过一个动态生成的着色器来完成，该着色器负责将各个输入画面合成到网格中。
 */
class MulticamDisplay : public ViewerDisplayWidget {
  Q_OBJECT // Qt 元对象系统宏，用于支持信号和槽机制

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit MulticamDisplay(QWidget *parent = nullptr);

  /**
   * @brief 设置与此显示控件关联的多机位节点。
   *
   * 当设置新的节点时，控件会根据节点的配置（如行列数）更新其显示。
   * @param n 指向 MultiCamNode 对象的指针。
   */
  void SetMulticamNode(MultiCamNode *n);

protected:
  /**
   * @brief 重写的绘制事件处理函数。
   *
   * 此函数负责实际的渲染逻辑，将来自 MultiCamNode 的多个视频帧
   * 使用自定义着色器绘制到控件的网格中。
   */
  void OnPaint() override;

  /**
   * @brief 重写的销毁事件处理函数。
   *
   * 在 OpenGL 上下文即将销毁时调用，用于释放此控件占用的 OpenGL 资源，
   * 例如着色器程序。
   */
  void OnDestroy() override;

  /**
   * @brief 重写的方法，用于从通用的 QVariant 数据加载自定义纹理。
   *
   * 在多机位显示中，这可能用于处理来自 MultiCamNode 的特定帧数据格式，
   * 并将其转换为渲染所需的纹理。
   * @param v 包含帧数据的 QVariant 对象。
   * @return 返回一个 TexturePtr (指向纹理对象的智能指针)。
   */
  TexturePtr LoadCustomTextureFromFrame(const QVariant &v) override;

private:
  /**
   * @brief 静态方法，根据给定的行数和列数动态生成用于多机位显示的 GLSL 着色器代码。
   * @param rows 网格的行数。
   * @param cols 网格的列数。
   * @return 返回包含生成好的着色器代码的 QString。
   */
  static QString GenerateShaderCode(int rows, int cols);

  MultiCamNode *node_; ///< 指向当前关联的多机位节点 (MultiCamNode) 的指针，提供视频源和布局信息。

  QVariant shader_; ///< 存储编译后的着色器程序（或其句柄/ID），用于渲染多机位网格。QVariant 可能用于存储不同渲染后端下的着色器对象。
  int rows_;        ///< 多机位显示网格的行数。
  int cols_;        ///< 多机位显示网格的列数。
};

}  // namespace olive

#endif  // MULTICAMDISPLAY_H
