#ifndef WAVEFORMSCOPE_H
#define WAVEFORMSCOPE_H

#include "widget/scope/scopebase/scopebase.h" // 示波器基类 (继承自 ManagedDisplayWidget)
#include "render/texture.h"                   // 纹理类 (TexturePtr)
#include "render/shadercode.h"                // 着色器代码封装类
#include <QVariant>                           // Qt 通用数据类型 (用于 pipeline 参数)
#include <QWidget>                            // Qt 控件基类 (ScopeBase 的基类 ManagedDisplayWidget 的基类)

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QOpenGLContext; // ScopeBase/ManagedDisplayWidget 可能使用

namespace olive {

/**
 * @brief WaveformScope 类是一个用于显示图像亮度或颜色分量波形图的示波器控件。
 *
 * 它继承自 ScopeBase，利用 OpenGL 和自定义着色器来分析输入图像的每一列（或行）像素，
 * 并将像素的亮度（或选定的颜色通道）值绘制为垂直线段，形成波形图。
 * 波形图可以帮助用户评估图像的曝光、对比度和色彩平衡。
 */
class WaveformScope : public ScopeBase {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit WaveformScope(QWidget* parent = nullptr);

  /**
   * @brief 使用宏定义的默认析构函数。
   *
   * MANAGEDDISPLAYWIDGET_DEFAULT_DESTRUCTOR 宏通常会处理 OpenGL 上下文的
   * makeCurrent(), OnDestroy(), doneCurrent() 调用序列，以确保资源被正确释放。
   * @param WaveformScope 当前类名。
   */
  MANAGEDDISPLAYWIDGET_DEFAULT_DESTRUCTOR(WaveformScope)

 protected:
  /**
   * @brief 重写函数，生成用于波形图计算和绘制的 GLSL 着色器代码。
   * @return 返回包含着色器代码的 ShaderCode 对象。
   */
  ShaderCode GenerateShaderCode() override;

  /**
   * @brief 重写的绘制示波器内容的函数。
   *
   * 此函数负责执行波形图的计算和渲染。
   * @param managed_tex 指向经过色彩管理的输入图像纹理的智能指针。
   * @param pipeline QVariant，包含主渲染管线（着色器程序）的句柄或相关数据。
   */
  void DrawScope(TexturePtr managed_tex, QVariant pipeline) override;
};

}  // namespace olive

#endif  // WAVEFORMSCOPE_H
