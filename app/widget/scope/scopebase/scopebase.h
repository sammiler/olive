#ifndef SCOPEBASE_H
#define SCOPEBASE_H

#include <QShowEvent>                              // Qt 显示事件类
#include <QVariant>                                // Qt 通用数据类型 (用于 pipeline_)
#include <QWidget>                                 // Qt 控件基类 (ManagedDisplayWidget 的基类)
#include "codec/frame.h"                           // 帧数据结构 (虽然未直接使用 Frame，但 TexturePtr 可能与帧数据相关)
#include "render/colorprocessor.h"                 // 色彩处理器类 (ColorProcessorPtr)
#include "render/shadercode.h"                     // 着色器代码封装类
#include "render/texture.h"                        // 纹理类 (TexturePtr)
#include "widget/manageddisplay/manageddisplay.h"  // 受管显示控件基类 (ScopeBase 的父类)

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QOpenGLContext; // ManagedDisplayWidget 可能使用

namespace olive {

/**
 * @brief ScopeBase 类是各种视频示波器（如直方图、波形图）的基类。
 *
 * 它继承自 ManagedDisplayWidget，提供了 OpenGL 渲染和色彩管理的基础功能。
 * 派生类需要实现具体的着色器代码生成和绘制逻辑，以显示特定类型的示波器。
 * 它接收视频帧作为输入纹理，并管理其显示和更新。
 */
class ScopeBase : public ManagedDisplayWidget {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit ScopeBase(QWidget* parent = nullptr);

  /**
   * @brief 使用宏定义的默认析构函数。
   *
   * MANAGEDDISPLAYWIDGET_DEFAULT_DESTRUCTOR 宏通常会处理 OpenGL 上下文的
   * makeCurrent(), OnDestroy(), doneCurrent() 调用序列，以确保资源被正确释放。
   * @param ScopeBase 当前类名。
   */
  MANAGEDDISPLAYWIDGET_DEFAULT_DESTRUCTOR(ScopeBase)

 public slots:
  /**
   * @brief 设置要在示波器中显示的视频帧（作为纹理）。
   * @param frame 指向包含视频帧数据的 Texture 对象的智能指针 (TexturePtr)。
   */
  void SetBuffer(TexturePtr frame);

 protected slots:  // 这些是重写 ManagedDisplayWidget 的虚函数槽
  /**
   * @brief 重写的 OpenGL 初始化槽函数。
   *
   * 在 OpenGL 上下文准备好后调用，用于创建此示波器所需的着色器和管线。
   */
  void OnInit() override;

  /**
   * @brief 重写的 OpenGL 绘制槽函数。
   *
   * 在需要重绘时调用，负责执行示波器的主要绘制逻辑。
   */
  void OnPaint() override;

  /**
   * @brief 重写的 OpenGL 销毁槽函数。
   *
   * 在 OpenGL 上下文即将销毁前调用，用于释放已创建的 OpenGL 资源，如着色器管线和纹理。
   */
  void OnDestroy() override;

 protected:
  /**
   * @brief 重写 QWidget 的 showEvent 方法。
   *
   * 当控件第一次显示时，可能会触发一次更新或重绘。
   * @param e 显示事件指针。
   */
  void showEvent(QShowEvent* e) override;

  /**
   * @brief 纯虚函数，派生类必须实现此函数以生成其特定的 GLSL 着色器代码。
   * @return 返回包含着色器代码的 ShaderCode 对象。
   */
  virtual ShaderCode GenerateShaderCode() = 0;

  /**
   * @brief 绘制函数。
   *
   * 如果您的子类示波器需要额外的绘制操作（在主着色器管线执行后），请重写此函数。
   * 默认实现为空。
   * @param managed_tex 指向经过色彩管理的输入图像纹理的智能指针。
   * @param pipeline QVariant，包含主渲染管线（着色器程序）的句柄或相关数据。
   */
  virtual void DrawScope(TexturePtr managed_tex, QVariant pipeline);

 private:
  QVariant pipeline_;  ///< 存储主渲染管线（通常是编译后的着色器程序）的句柄或相关数据。

  TexturePtr texture_;  ///< 指向原始输入视频帧纹理的智能指针。

  TexturePtr managed_tex_;  ///< 指向经过色彩管理（如果需要）处理后的视频帧纹理的智能指针。
                            ///< 这是实际传递给 DrawScope 进行分析和显示的纹理。

  bool managed_tex_up_to_date_;  ///< 标记 managed_tex_ 是否已根据最新的 texture_ 和色彩管理设置更新。
};

}  // namespace olive

#endif  // SCOPEBASE_H
