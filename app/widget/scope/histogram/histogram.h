#ifndef HISTOGRAMSCOPE_H
#define HISTOGRAMSCOPE_H

#include "widget/scope/scopebase/scopebase.h" // 示波器基类 (可能继承自 ManagedDisplayWidget)
#include "render/texture.h"                   // 纹理类 (TexturePtr)
#include "render/shadercode.h"                // 着色器代码封装类
#include "render/managedcolor.h"              // 色彩管理相关 (Color, ColorProcessorPtr - 虽然未直接用，但 ScopeBase 可能用)
#include <QVariant>                           // Qt 通用数据类型 (用于 shader_ 和 pipeline_secondary_)
#include <QWidget>                            // Qt 控件基类 (ScopeBase 的基类 ManagedDisplayWidget 的基类)

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QOpenGLContext; // ScopeBase/ManagedDisplayWidget 可能使用

namespace olive {

/**
 * @brief HistogramScope 类是一个用于显示图像颜色分量直方图的示波器控件。
 *
 * 它继承自 ScopeBase，利用 OpenGL 和自定义着色器来计算和渲染输入图像的
 * 亮度或 RGB 通道的直方图。
 * 直方图通常分两步计算：首先计算每行像素值的总和，然后汇总这些行总和来生成最终的直方图。
 */
class HistogramScope : public ScopeBase {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit HistogramScope(QWidget* parent = nullptr);

  /**
   * @brief 使用宏定义的默认析构函数。
   *
   * MANAGEDDISPLAYWIDGET_DEFAULT_DESTRUCTOR 宏通常会处理 OpenGL 上下文的
   * makeCurrent(), OnDestroy(), doneCurrent() 调用序列，以确保资源被正确释放。
   * @param HistogramScope 当前类名。
   */
  MANAGEDDISPLAYWIDGET_DEFAULT_DESTRUCTOR(HistogramScope)

 protected slots: // 虽然标记为 protected slots，但它们是重写 ManagedDisplayWidget/ScopeBase 的虚函数槽
  /**
   * @brief 重写的 OpenGL 初始化槽函数。
   *
   * 在 OpenGL 上下文准备好后调用，用于创建着色器、纹理等 OpenGL 资源。
   */
  void OnInit() override;

  /**
   * @brief 重写的 OpenGL 销毁槽函数。
   *
   * 在 OpenGL 上下文即将销毁前调用，用于释放已创建的 OpenGL 资源。
   */
  void OnDestroy() override;

 protected:
  /**
   * @brief 重写函数，生成用于直方图主要计算和绘制阶段的 GLSL 着色器代码。
   * @return 返回包含着色器代码的 ShaderCode 对象。
   */
  ShaderCode GenerateShaderCode() override;
  /**
   * @brief 创建用于直方图计算的辅助（第二阶段）着色器管线。
   *
   * 这个着色器可能用于汇总第一阶段（例如，行求和）的结果来生成最终的直方图。
   * @return 返回一个 QVariant，其中包含辅助着色器管线的句柄或相关数据。
   */
  QVariant CreateSecondaryShader();

  /**
   * @brief 重写的绘制示波器内容的函数。
   *
   * 此函数负责执行直方图的计算和渲染。
   * @param managed_tex 指向经过色彩管理的输入图像纹理的智能指针。
   * @param pipeline QVariant，包含主渲染管线（着色器程序）的句柄或相关数据。
   */
  void DrawScope(TexturePtr managed_tex, QVariant pipeline) override;

 private:
  QVariant pipeline_secondary_;   ///< 存储辅助（第二阶段）着色器管线的句柄或相关数据。
  TexturePtr texture_row_sums_; ///< 指向用于存储每行像素值总和的中间结果的纹理的智能指针。
};

}  // namespace olive

#endif  // HISTOGRAMSCOPE_H
