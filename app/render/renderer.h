#ifndef RENDERCONTEXT_H // 防止头文件被重复包含的宏
#define RENDERCONTEXT_H // 定义 RENDERCONTEXT_H 宏

#include <QMutex>   // Qt 互斥锁类
#include <QObject>  // Qt 对象模型基类
#include <QVariant> // Qt 通用数据类型 QVariant
#include <utility>  // 标准库 utility 头文件，提供 std::move

#include "common/define.h"                 // 可能包含项目通用的定义或宏
#include "node/node.h"                     // 可能包含 Node 类的定义 (虽然未直接使用，但逻辑上相关)
#include "render/colorprocessor.h"         // 包含 ColorProcessor (颜色处理器) 相关的定义
#include "render/job/colortransformjob.h"  // 包含 ColorTransformJob 的定义
#include "render/videoparams.h"            // 包含 VideoParams (视频参数) 的定义
#include "texture.h"                       // 包含 Texture (纹理) 相关的定义 (Texture, TexturePtr, PixelFormat)

// 假设 ShaderJob, ShaderCode, Color 等类型已通过其他方式被间接包含。
// std::list, QHash, QVector 也假设已包含。

namespace olive { // olive 项目的命名空间

class ShaderJob; // 向前声明 ShaderJob 类

/**
 * @brief Renderer 类是一个抽象基类，定义了渲染操作的通用接口。
 *
 * 它是 Olive 渲染系统的核心，旨在将上层逻辑 (如 NodeTraverser) 与具体的图形API实现
 * (如 OpenGLRenderer) 解耦。派生类需要实现此类中声明的纯虚函数，以提供特定
 * 图形API的渲染能力。
 *
 * Renderer 负责管理原生图形资源 (如纹理、着色器)，并执行诸如清屏、绘制 (Blit)、
 * 数据上传/下载等操作。它还可能包含一些内部缓存机制，例如颜色转换上下文缓存和纹理缓存。
 *
 * (注意：文件名是 RenderContext.h，而类名是 Renderer，这可能是一个历史遗留问题或特定命名约定)
 */
class Renderer : public QObject { // Renderer 继承自 QObject
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  /**
   * @brief 构造函数。
   * @param parent 父对象指针，默认为 nullptr。
   */
  explicit Renderer(QObject *parent = nullptr);

  /**
   * @brief (纯虚函数) 初始化渲染器。
   * 派生类需要实现此方法以设置其图形上下文和所需资源。
   * @return 如果初始化成功，返回 true。
   */
  virtual bool Init() = 0;

  /**
   * @brief 根据视频参数创建一个新的纹理对象 (TexturePtr)。
   * 内部会调用 CreateNativeTexture 来创建原生图形API纹理。
   * @param params 描述纹理属性的 VideoParams。
   * @param data (可选) 指向初始化纹理数据的指针。
   * @param linesize (可选) 初始化数据中每行的字节数。
   * @return 返回一个指向新创建纹理的 TexturePtr。
   */
  TexturePtr CreateTexture(const VideoParams &params, const void *data = nullptr, int linesize = 0);

  /**
   * @brief 销毁一个纹理对象及其关联的原生图形资源。
   * @param texture 指向要销毁的 Texture 对象的指针。
   */
  void DestroyTexture(Texture *texture);

  /**
   * @brief 将 ShaderJob 的结果渲染到指定的目标纹理。
   * 这是一个便捷的重载版本，目标参数从 `destination` 纹理对象中获取。
   * @param shader 包含原生着色器句柄的 QVariant。
   * @param job 包含输入、uniforms 等信息的 ShaderJob。
   * @param destination 指向目标 olive::Texture 对象的指针。
   * @param clear_destination (可选) 是否在渲染前清空目标纹理，默认为 true。
   */
  void BlitToTexture(QVariant shader, olive::ShaderJob job, olive::Texture *destination,
                     bool clear_destination = true) {
    // 调用另一个 Blit 重载，使用目标纹理的参数
    Blit(std::move(shader), std::move(job), destination, destination->params(), clear_destination);
  }

  /**
   * @brief 将 ShaderJob 的结果渲染到具有指定参数的帧缓冲 (可能不直接渲染到特定纹理对象)。
   * 这是一个便捷的重载版本，目标纹理为 nullptr，表示渲染到当前绑定的帧缓冲或默认帧缓冲。
   * @param shader 包含原生着色器句柄的 QVariant。
   * @param job ShaderJob。
   * @param params 目标帧缓冲的视频参数。
   * @param clear_destination (可选) 是否在渲染前清空目标，默认为 true。
   */
  void Blit(QVariant shader, olive::ShaderJob job, olive::VideoParams params, bool clear_destination = true) {
    // 调用另一个 Blit 重载，目标纹理为 nullptr
    Blit(std::move(shader), std::move(job), nullptr, std::move(params), clear_destination);
  }

  /**
   * @brief 执行一个带有色彩管理的 Blit 操作，将 ColorTransformJob 的结果渲染到指定目标纹理和参数。
   * @param color_job 描述颜色转换和输入源的 ColorTransformJob。
   * @param destination 指向目标 olive::Texture 对象的指针。
   * @param params 目标纹理的视频参数。
   */
  void BlitColorManaged(const ColorTransformJob &color_job, Texture *destination, const VideoParams &params);
  /**
   * @brief 便捷重载，目标参数从 `destination` 纹理获取。
   */
  void BlitColorManaged(const ColorTransformJob &job, Texture *destination) {
    BlitColorManaged(job, destination, destination->params());
  }
  /**
   * @brief 便捷重载，目标纹理为 nullptr (渲染到当前帧缓冲)。
   */
  void BlitColorManaged(const ColorTransformJob &job, const VideoParams &params) {
    BlitColorManaged(job, nullptr, params);
  }

  /**
   * @brief 将两个输入纹理 (顶部场和底部场) 交错合成为一个输出纹理。
   * @param top 指向顶部场纹理的 TexturePtr。
   * @param bottom 指向底部场纹理的 TexturePtr。
   * @param params 输出交错纹理的视频参数。
   * @return 返回指向新创建的交错纹理的 TexturePtr。
   */
  TexturePtr InterlaceTexture(const TexturePtr &top, const TexturePtr &bottom, const VideoParams &params);

  /**
   * @brief 获取一个默认的着色器程序 (通常是一个简单的传递着色器或纹理拷贝着色器)。
   * @return 返回包含原生默认着色器句柄的 QVariant。
   */
  QVariant GetDefaultShader();

  /**
   * @brief 销毁渲染器，释放所有资源。
   * 内部会调用 PostDestroy。
   */
  void Destroy();

  /**
   * @brief (纯虚函数) 在渲染器销毁前执行的特定于派生类的清理操作。
   * 通常在渲染线程中被调用。
   */
  virtual void PostDestroy() = 0;

  /**
   * @brief (纯虚函数) 在渲染器初始化后执行的特定于派生类的额外设置。
   * 通常在渲染线程中被调用。
   */
  virtual void PostInit() = 0;

  /**
   * @brief (纯虚函数) 清空目标纹理或当前帧缓冲。
   * @param texture (可选) 如果提供，则清空此特定纹理。如果为 nullptr，则清空当前绑定的帧缓冲。
   * @param r, g, b, a 清除操作使用的颜色值 (0.0 到 1.0)。
   */
  virtual void ClearDestination(olive::Texture *texture = nullptr, double r = 0.0, double g = 0.0, double b = 0.0,
                                double a = 1.0) = 0; // 注意：原始代码中 a 默认为 1.0

  /**
   * @brief (纯虚函数) 根据 ShaderCode 创建一个原生图形API着色器程序。
   * @param code 包含着色器源代码和元信息的 ShaderCode 对象。
   * @return 返回一个 QVariant，其中包含原生着色器句柄。
   */
  virtual QVariant CreateNativeShader(olive::ShaderCode code) = 0;

  /**
   * @brief (纯虚函数) 销毁一个原生图形API着色器程序。
   * @param shader 包含原生着色器句柄的 QVariant。
   */
  virtual void DestroyNativeShader(QVariant shader) = 0;

  /**
   * @brief (纯虚函数) 将CPU内存中的像素数据上传到指定的原生图形API纹理。
   * @param handle 包含原生纹理句柄的 QVariant。
   * @param params 描述纹理和数据格式的 VideoParams。
   * @param data 指向要上传的像素数据的指针。
   * @param linesize 图像数据中每行的字节数。
   */
  virtual void UploadToTexture(const QVariant &handle, const VideoParams &params, const void *data, int linesize) = 0;

  /**
   * @brief (纯虚函数) 从指定的原生图形API纹理下载像素数据到CPU内存。
   * @param handle 包含原生纹理句柄的 QVariant。 (参数名在声明中是 handle，在文档注释中是 id)
   * @param params 描述纹理和期望数据格式的 VideoParams。
   * @param data 指向用于存储下载像素数据的内存缓冲区的指针。
   * @param linesize 图像数据中每行的字节数。
   */
  virtual void DownloadFromTexture(const QVariant &handle, const VideoParams &params, void *data, int linesize) = 0;

  /**
   * @brief (纯虚函数) 刷新所有挂起的图形API命令，确保它们被执行。
   */
  virtual void Flush() = 0;

  /**
   * @brief (纯虚函数) 从指定的纹理中读取特定点的像素颜色值。
   * @param texture 指向 olive::Texture 对象的指针。
   * @param pt 要采样的像素坐标。
   * @return 返回采样点的 Color 对象。
   */
  virtual Color GetPixelFromTexture(olive::Texture *texture, const QPointF &pt) = 0;

 protected:
  /**
   * @brief (纯虚函数) 核心的 Blit (位块传送) 操作。
   * 使用指定的着色器和参数，将输入 (在 ShaderJob 中定义) 渲染到目标。
   * @param shader 包含原生着色器程序句柄的 QVariant。
   * @param job 包含输入、uniforms等信息的 ShaderJob。
   * @param destination (可选) 指向目标 olive::Texture 对象的指针。如果为 nullptr，则渲染到当前绑定的帧缓冲。
   * @param destination_params 目标帧缓冲的视频参数。
   * @param clear_destination 是否在渲染前清空目标。
   */
  virtual void Blit(QVariant shader, olive::ShaderJob job, olive::Texture *destination,
                    olive::VideoParams destination_params, bool clear_destination) = 0;

  /**
   * @brief (纯虚函数) 创建一个原生图形API纹理。
   * @param width, height, depth 纹理的尺寸。
   * @param format 像素格式。
   * @param channel_count 通道数量。
   * @param data (可选) 指向初始化纹理数据的指针。
   * @param linesize (可选) 初始化数据中每行的字节数。
   * @return 返回一个 QVariant，其中包含原生纹理句柄。
   */
  virtual QVariant CreateNativeTexture(int width, int height, int depth, PixelFormat format, int channel_count,
                                       const void *data = nullptr, int linesize = 0) = 0;

  /**
   * @brief (纯虚函数) 销毁一个原生图形API纹理。
   * @param texture 包含原生纹理句柄的 QVariant。
   */
  virtual void DestroyNativeTexture(QVariant texture) = 0;

  /**
   * @brief (纯虚函数) 渲染器内部特定于派生类的资源销毁逻辑。
   * 在 PostDestroy 之前，或在 Destroy 的早期阶段被调用。
   */
  virtual void DestroyInternal() = 0;

 private:
  // 内部结构体，用于缓存颜色转换所需的上下文信息 (例如已编译的着色器和LUT纹理)
  struct ColorContext {
    // 内部结构体，用于描述一个查找表 (LUT) 纹理
    struct LUT {
      TexturePtr texture;             // 指向LUT纹理的 TexturePtr
      Texture::Interpolation interpolation; // LUT纹理采样时使用的插值模式
      QString name;                   // LUT的名称 (可能用于在着色器中绑定)
    };

    QVariant compiled_shader;     // 预编译的用于此颜色转换的着色器句柄
    QVector<LUT> lut3d_textures;  // 需要的3D LUT纹理列表
    QVector<LUT> lut1d_textures;  // 需要的1D LUT纹理列表
  };

  /**
   * @brief 根据原生图形API纹理句柄和视频参数创建一个 Texture 对象。
   * @param v 包含原生纹理句柄的 QVariant。
   * @param params 纹理的视频参数。
   * @return 返回一个 TexturePtr。
   */
  TexturePtr CreateTextureFromNativeHandle(const QVariant &v, const VideoParams &params);

  /**
   * @brief 获取或创建用于指定 ColorTransformJob 的颜色转换上下文 (ColorContext)。
   * 如果上下文中包含的着色器或LUT尚未准备好，此方法可能会触发它们的编译或加载。
   * @param color_job 描述颜色转换的 ColorTransformJob。
   * @param ctx (输出参数) 指向用于存储获取到的 ColorContext 的指针。
   * @return 如果成功获取或创建上下文，则返回 true。
   */
  bool GetColorContext(const ColorTransformJob &color_job, ColorContext *ctx);

  // 清理旧的、不再使用的纹理 (通常是纹理缓存中的)
  void ClearOldTextures();

  // 颜色转换上下文缓存：键是颜色转换的唯一ID (通常来自 ColorProcessor)，值是 ColorContext
  QHash<QString, ColorContext> color_cache_;

  // 内部结构体，用于在纹理缓存中存储纹理及其元数据
  struct CachedTexture {
    int width;           // 宽度
    int height;          // 高度
    int depth;           // 深度
    PixelFormat format;  // 像素格式
    int channel_count;   // 通道数
    QVariant handle;     // 原生纹理句柄
    qint64 accessed;     // 上次访问时间戳 (用于LRU淘汰)
  };

  static const int MAX_TEXTURE_LIFE = 5000; // 纹理在缓存中保持的最长生命周期 (毫秒)
  static const bool USE_TEXTURE_CACHE = true; // 是否启用纹理缓存的全局开关
  std::list<CachedTexture> texture_cache_; // 纹理缓存列表 (使用 std::list 可能便于移除中间元素)

  QMutex color_cache_mutex_; // 用于保护 `color_cache_` 访问的互斥锁

  QVariant default_shader_; // 缓存的默认着色器句柄

  QVariant interlace_texture_; // (可能) 用于交错操作的特定着色器或纹理句柄

  QMutex texture_cache_lock_; // 用于保护 `texture_cache_` 访问的互斥锁
};

}  // namespace olive

#endif  // RENDERCONTEXT_H