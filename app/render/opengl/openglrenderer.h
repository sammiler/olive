#ifndef OPENGLCONTEXT_H // 防止头文件被重复包含的宏
#define OPENGLCONTEXT_H // 定义 OPENGLCONTEXT_H 宏

#include <QOffscreenSurface>      // Qt 离屏表面类，用于在没有可见窗口的情况下进行OpenGL渲染
#include <QOpenGLBuffer>          // Qt OpenGL 缓冲区对象类 (VBO, EBO)
#include <QOpenGLFunctions>       // Qt 封装的 OpenGL 函数访问基类
#include <QOpenGLShader>          // Qt OpenGL 着色器程序和着色器对象类
#include <QOpenGLVertexArrayObject> // Qt OpenGL 顶点数组对象 (VAO) 类
#include <QThread>                // Qt 线程类 (可能用于在单独的线程中运行渲染)
#include <QTimer>                 // Qt 定时器类

#include "render/renderer.h"      // 包含 Renderer 抽象基类的定义

// 假设 olive::Texture, olive::ShaderJob, olive::ShaderCode, olive::VideoParams,
// olive::PixelFormat, olive::Color 等类型的定义已通过 "render/renderer.h"
// 或其他方式被间接包含。

namespace olive { // olive 项目的命名空间

/**
 * @brief OpenGLRenderer 类是 Renderer 接口的一个具体实现，使用 OpenGL API 进行渲染。
 *
 * 它负责管理 OpenGL 上下文、着色器、纹理以及执行渲染操作 (如清屏、绘制、数据上传/下载)。
 * 这个类封装了与特定图形 API (OpenGL) 相关的底层细节，为上层逻辑 (如 NodeTraverser)
 * 提供一个统一的渲染接口。
 *
 * 它可能在单独的渲染线程中运行，以避免阻塞主 UI 线程。
 * (注意：文件名是 OpenGLContext.h，而类名是 OpenGLRenderer，这可能是一个历史遗留问题或特定命名约定)
 */
class OpenGLRenderer : public Renderer { // OpenGLRenderer 继承自 Renderer 抽象基类
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  /**
   * @brief 构造函数。
   * @param parent 父对象指针，默认为 nullptr。
   */
  explicit OpenGLRenderer(QObject *parent = nullptr);

  // 析构函数
  ~OpenGLRenderer() override;

  /**
   * @brief 使用一个已存在的外部 QOpenGLContext 初始化渲染器。
   * @param existing_ctx 指向外部 QOpenGLContext 的指针。
   */
  void Init(QOpenGLContext *existing_ctx);

  /**
   * @brief (重写 Renderer::Init) 初始化渲染器自身创建的 OpenGL 上下文。
   * @return 如果初始化成功，返回 true。
   */
  bool Init() override;

  /**
   * @brief (重写 Renderer::PostDestroy) 在渲染器销毁前执行的清理操作 (通常在渲染线程中)。
   */
  void PostDestroy() override;

  /**
   * @brief (重写 Renderer::PostInit) 在渲染器初始化后执行的额外设置 (通常在渲染线程中)。
   */
  void PostInit() override;

  /**
   * @brief (重写 Renderer::ClearDestination) 清空目标纹理或当前帧缓冲。
   * @param texture (可选) 如果提供，则清空此特定纹理。如果为 nullptr，则清空当前绑定的帧缓冲。
   * @param r, g, b, a 清除操作使用的颜色值 (0.0 到 1.0)。
   */
  void ClearDestination(olive::Texture *texture = nullptr, double r = 0.0, double g = 0.0, double b = 0.0,
                        double a = 0.0) override;

  /**
   * @brief (重写 Renderer::CreateNativeShader) 根据 ShaderCode 创建一个特定于 OpenGL 的着色器程序。
   * @param code 包含着色器源代码和元信息的 ShaderCode 对象。
   * @return 返回一个 QVariant，其中包含原生着色器句柄 (例如 GLuint programID)。
   */
  QVariant CreateNativeShader(olive::ShaderCode code) override;

  /**
   * @brief (重写 Renderer::DestroyNativeShader) 销毁一个原生 OpenGL 着色器程序。
   * @param shader 包含原生着色器句柄的 QVariant。
   */
  void DestroyNativeShader(QVariant shader) override;

  /**
   * @brief (重写 Renderer::UploadToTexture) 将CPU内存中的像素数据上传到指定的OpenGL纹理。
   * @param handle 包含原生纹理句柄 (GLuint) 的 QVariant。
   * @param params 描述纹理和数据格式的 VideoParams。
   * @param data 指向要上传的像素数据的指针。
   * @param linesize 图像数据中每行的字节数。
   */
  void UploadToTexture(const QVariant &handle, const VideoParams &params, const void *data, int linesize) override;

  /**
   * @brief (重写 Renderer::DownloadFromTexture) 从指定的OpenGL纹理下载像素数据到CPU内存。
   * @param id 包含原生纹理句柄 (GLuint) 的 QVariant。
   * @param params 描述纹理和期望数据格式的 VideoParams。
   * @param data 指向用于存储下载像素数据的内存缓冲区的指针。
   * @param linesize 图像数据中每行的字节数。
   */
  void DownloadFromTexture(const QVariant &id, const VideoParams &params, void *data, int linesize) override;

  /**
   * @brief (重写 Renderer::Flush) 刷新所有挂起的OpenGL命令，确保它们被执行。
   */
  void Flush() override;

  /**
   * @brief (重写 Renderer::GetPixelFromTexture) 从指定的OpenGL纹理中读取特定点的像素颜色值。
   * @param texture 指向 olive::Texture 对象的指针 (包含原生纹理句柄)。
   * @param pt 要采样的像素坐标 (通常是归一化坐标或像素坐标，取决于实现)。
   * @return 返回采样点的 Color 对象。
   */
  Color GetPixelFromTexture(olive::Texture *texture, const QPointF &pt) override;

 protected:
  /**
   * @brief (重写 Renderer::Blit) 使用指定的着色器和参数，将输入纹理 (在 ShaderJob 中定义) 渲染到目标纹理。
   * 这是核心的渲染操作。
   * @param shader 包含原生着色器程序句柄的 QVariant。
   * @param job 包含输入纹理、uniforms和其他渲染参数的 ShaderJob。
   * @param destination 指向目标 olive::Texture 对象的指针。
   * @param destination_params 目标纹理的视频参数。
   * @param clear_destination 是否在渲染前清空目标纹理。
   */
  void Blit(QVariant shader, olive::ShaderJob job, olive::Texture *destination, olive::VideoParams destination_params,
            bool clear_destination) override;

  /**
   * @brief (重写 Renderer::CreateNativeTexture) 创建一个原生OpenGL纹理。
   * @param width, height, depth 纹理的尺寸。
   * @param format 像素格式。
   * @param channel_count 通道数量。
   * @param data (可选) 指向初始化纹理数据的指针。
   * @param linesize (可选) 初始化数据中每行的字节数。
   * @return 返回一个 QVariant，其中包含原生纹理句柄 (GLuint)。
   */
  QVariant CreateNativeTexture(int width, int height, int depth, PixelFormat format, int channel_count,
                               const void *data = nullptr, int linesize = 0) override;

  /**
   * @brief (重写 Renderer::DestroyNativeTexture) 销毁一个原生OpenGL纹理。
   * @param texture 包含原生纹理句柄的 QVariant。
   */
  void DestroyNativeTexture(QVariant texture) override;

  /**
   * @brief (重写 Renderer::DestroyInternal) 渲染器内部资源的销毁逻辑，在 PostDestroy 之前调用。
   */
  void DestroyInternal() override;

 private:
  // --- 私有辅助方法 ---

  /**
   * @brief (静态) 根据 PixelFormat 和通道布局获取OpenGL内部格式 (internalformat)。
   */
  static GLint GetInternalFormat(PixelFormat format, int channel_layout);

  /**
   * @brief (静态) 根据 PixelFormat 获取OpenGL像素类型 (type)。
   */
  static GLenum GetPixelType(PixelFormat format);

  /**
   * @brief (静态) 根据通道数量获取OpenGL像素格式 (format)。
   */
  static GLenum GetPixelFormat(int channel_count);

  /**
   * @brief 将指定的OpenGL纹理附加为当前帧缓冲对象 (FBO) 的渲染目标。
   * @param texture 包含原生纹理句柄的 QVariant。
   */
  void AttachTextureAsDestination(const QVariant &texture);

  /**
   * @brief 从当前帧缓冲对象分离渲染目标。
   */
  void DetachTextureAsDestination();

  /**
   * @brief 准备输入纹理以供着色器采样，设置纹理单元、绑定纹理并设置插值模式。
   * @param target 纹理目标 (例如 GL_TEXTURE_2D)。
   * @param interp 纹理插值模式。
   */
  void PrepareInputTexture(GLenum target, Texture::Interpolation interp);

  /**
   * @brief 内部清空当前绑定帧缓冲的方法。
   */
  void ClearDestinationInternal(double r = 0.0, double g = 0.0, double b = 0.0, double a = 0.0);

  /**
   * @brief 编译指定类型的OpenGL着色器源码。
   * @param type 着色器类型 (例如 GL_VERTEX_SHADER, GL_FRAGMENT_SHADER)。
   * @param code 着色器的GLSL源代码。
   * @return 返回编译后的着色器对象句柄 (GLuint)。
   */
  GLuint CompileShader(GLenum type, const QString &code);

  // --- 成员变量 ---

  QOpenGLContext *context_; // 指向当前OpenGL上下文的指针

  QOpenGLFunctions *functions_{}; // 指向封装了OpenGL核心函数 (特定版本) 的对象的指针，
                                 // 通过 context_->functions() 或 context_->versionFunctions<QOpenGLFunctions_x_y_Core>() 获取。
                                 // 初始化为 nullptr 或使用 Qt 6 的 QOpenGLVersionFunctions。

  QOffscreenSurface surface_; // 离屏表面，用于无窗口渲染

  GLuint framebuffer_; // 帧缓冲对象 (FBO) 的句柄，用于离屏渲染到纹理

  // 用于纹理缓存的键结构体，通过纹理参数来唯一标识一个纹理
  struct TextureCacheKey {
    int width{};         // 宽度
    int height{};        // 高度
    int depth{};         // 深度 (用于3D纹理)
    PixelFormat format;  // 像素格式
    int channel_count{}; // 通道数量

    // 重载等于运算符，用于 QMap/QHash
    bool operator==(const TextureCacheKey &rhs) const {
      return width == rhs.width && height == rhs.height && depth == rhs.depth &&
             static_cast<PixelFormat::Format>(format) == static_cast<PixelFormat::Format>(rhs.format) && // 确保比较的是枚举的底层值或特定成员
             channel_count == rhs.channel_count;
    }
  };

  // 存储已创建纹理的参数信息，键是纹理句柄 (GLuint)，值是 TextureCacheKey
  QMap<GLuint, TextureCacheKey> texture_params_;

  static const int kTextureCacheMaxSize; // 纹理缓存的最大尺寸 (可能未使用或在.cpp中定义)
};

}  // namespace olive

#endif  // OPENGLCONTEXT_H