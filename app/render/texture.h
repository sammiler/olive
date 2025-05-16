#ifndef RENDERTEXTURE_H  // 防止头文件被重复包含的宏
#define RENDERTEXTURE_H  // 定义 RENDERTEXTURE_H 宏

#include <QVariant>  // Qt 通用数据类型 QVariant (用于存储原生纹理句柄)
#include <memory>    // 标准库内存管理头文件 (用于 std::shared_ptr, std::make_shared)
#include <utility>   // 标准库 utility 头文件 (用于 std::move)

#include "render/videoparams.h"  // 包含 VideoParams (视频参数) 和 PixelFormat 的定义

// 假设 AcceleratedJob, Renderer, QVector2D, rational 等类型
// 已通过其他方式被间接包含。

namespace olive {  // olive 项目的命名空间

class AcceleratedJob;  // 向前声明 AcceleratedJob 类
class Renderer;        // 向前声明 Renderer 类

class Texture;  // 向前声明 Texture 类自身
// 类型别名：TexturePtr 是一个指向 Texture 对象的共享指针 (std::shared_ptr<Texture>)
using TexturePtr = std::shared_ptr<Texture>;

/**
 * @brief Texture 类代表一个图像纹理。
 *
 * 它可以是一个“真实”的纹理，关联到一个渲染器后端 (Renderer) 并拥有一个原生图形API句柄 (`id_`)。
 * 或者，它可以是一个“虚拟”的纹理，不直接拥有原生句柄，而是通过一个 AcceleratedJob (`job_`)
 * 来描述如何生成这个纹理的数据 (例如，通过着色器、从文件加载或程序化生成)。
 *
 * Texture 对象存储了其视频参数 (`params_`)，如分辨率、像素格式等。
 * 它提供了上传和下载像素数据到/从原生纹理的方法 (如果它是一个真实纹理)。
 *
 * 这个类是 Olive 渲染管线中图像数据的主要表示形式。
 */
class Texture {
 public:
  // 定义纹理采样时的插值模式枚举
  enum Interpolation {
    kNearest,         // 最近邻插值 (块状效果，速度快)
    kLinear,          // 线性插值 (平滑效果)
    kMipmappedLinear  // 使用 Mipmap 进行线性插值 (用于缩小纹理时减少摩尔纹和闪烁)
  };

  // 默认的纹理插值模式常量
  static const Interpolation kDefaultInterpolation;

  /**
   * @brief 构造一个“虚拟”纹理 (dummy texture)，没有关联的渲染器后端。
   * 这种纹理通常通过一个 AcceleratedJob 来定义其内容。
   * @param param 纹理的视频参数。
   */
  explicit Texture(VideoParams param) : renderer_(nullptr), params_(std::move(param)), job_(nullptr) {}

  /**
   * @brief (模板构造函数) 构造一个“虚拟”纹理，其内容由一个 AcceleratedJob 定义。
   * @tparam T AcceleratedJob 的具体类型 (例如 ShaderJob, FootageJob)。
   * @param p 纹理的视频参数。
   * @param j 描述如何生成纹理数据的 AcceleratedJob 对象。
   */
  template <typename T>
  Texture(const VideoParams& p, const T& j) : Texture(p) {  // 委托给第一个构造函数初始化 renderer_ 和 params_
    job_ = new T(j);  // 创建并存储 Job 对象 (注意：这里是裸指针，需要管理其生命周期)
  }

  /**
   * @brief 构造一个“真实”纹理，关联到一个渲染器后端和原生图形API句柄。
   * @param renderer 指向 Renderer 实例的指针。
   * @param native 包含原生纹理句柄的 QVariant。
   * @param param 纹理的视频参数。
   */
  Texture(Renderer* renderer, QVariant native, VideoParams param)
      : renderer_(renderer), params_(std::move(param)), id_(std::move(native)), job_(nullptr) {}

  // 析构函数。如果 job_ 是一个已分配的 AcceleratedJob，需要在此处或通过其他机制释放它。
  // 如果 id_ 是一个原生纹理句柄，也可能需要通过 renderer_ 来释放。
  ~Texture();

  /**
   * @brief 获取原生纹理句柄 (如果这是一个真实纹理)。
   * @return 返回包含原生句柄的 QVariant。如果不是真实纹理，QVariant 可能无效。
   */
  [[nodiscard]] QVariant id() const { return id_; }

  /**
   * @brief 获取纹理的视频参数。
   * @return 返回 VideoParams 对象的常量引用。
   */
  [[nodiscard]] const VideoParams& params() const { return params_; }

  /**
   * @brief (静态模板工厂方法) 创建一个新的 TexturePtr，其内容由一个 AcceleratedJob 定义。
   * @tparam T AcceleratedJob 的具体类型。
   * @param p 纹理的视频参数。
   * @param j 描述如何生成纹理数据的 AcceleratedJob 对象。
   * @return 返回一个指向新创建 Texture 对象的 TexturePtr。
   */
  template <typename T>
  static TexturePtr Job(const VideoParams& p, const T& j) {
    return std::make_shared<Texture>(p, j);  // 使用 std::make_shared 创建共享指针
  }

  /**
   * @brief (模板方法) 将当前纹理 (通常是一个真实纹理或已解析的虚拟纹理) 转换为一个新的、
   *        由指定 Job 定义的虚拟纹理。参数会沿用当前纹理的参数。
   * (这个方法名 toJob 可能有点误导，它实际上是创建一个新的基于Job的纹理，
   *  而不是将当前纹理转换成一个Job对象本身。)
   * @tparam T AcceleratedJob 的具体类型。
   * @param job 新的 AcceleratedJob 对象。
   * @return 返回一个新的 TexturePtr。
   */
  template <typename T>
  TexturePtr toJob(const T& job) {
    return Texture::Job(params_, job);  // 使用当前纹理的参数和新的 Job 创建
  }

  /**
   * @brief 将CPU内存中的像素数据上传到此纹理 (如果它是一个真实纹理)。
   * 内部会调用 renderer_->UploadToTexture。
   * @param data 指向要上传的像素数据的指针。
   * @param linesize 图像数据中每行的字节数。
   */
  void Upload(void* data, int linesize);

  /**
   * @brief 从此纹理 (如果它是一个真实纹理) 下载像素数据到CPU内存。
   * 内部会调用 renderer_->DownloadFromTexture。
   * @param data 指向用于存储下载像素数据的内存缓冲区的指针。
   * @param linesize 图像数据中每行的字节数。
   */
  void Download(void* data, int linesize);

  /**
   * @brief 检查此纹理是否为“虚拟”纹理 (即没有关联的渲染器后端)。
   * @return 如果 renderer_ 为 nullptr，则返回 true。
   */
  [[nodiscard]] bool IsDummy() const { return !renderer_; }

  // --- 便捷的参数访问方法 (从 params_ 获取) ---
  [[nodiscard]] int width() const { return params_.effective_width(); }    // 有效宽度
  [[nodiscard]] int height() const { return params_.effective_height(); }  // 有效高度

  /**
   * @brief 获取考虑了像素宽高比的“虚拟”分辨率 (用于正确显示非方形像素)。
   * @return 返回一个 QVector2D，包含校正后的宽度和原始高度。
   */
  [[nodiscard]] QVector2D virtual_resolution() const {
    return {static_cast<float>(params_.square_pixel_width()), static_cast<float>(params_.height())};
  }

  [[nodiscard]] PixelFormat format() const { return params_.format(); }        // 像素格式
  [[nodiscard]] int channel_count() const { return params_.channel_count(); }  // 通道数量
  [[nodiscard]] int divider() const { return params_.divider(); }              // (可能用于平面格式的除数)
  [[nodiscard]] const rational& pixel_aspect_ratio() const { return params_.pixel_aspect_ratio(); }  // 像素宽高比

  /**
   * @brief 获取与此纹理关联的渲染器后端 (如果它是一个真实纹理)。
   * @return 返回 Renderer 指针，如果是虚拟纹理则返回 nullptr。
   */
  [[nodiscard]] Renderer* renderer() const { return renderer_; }

  /**
   * @brief 检查此纹理是否通过一个 AcceleratedJob 来定义其内容。
   * @return 如果 job_ 不为 nullptr，则返回 true。
   */
  [[nodiscard]] bool IsJob() const { return job_; }
  /**
   * @brief 获取与此纹理关联的 AcceleratedJob (如果存在)。
   * @return 返回 AcceleratedJob 指针，如果不存在则返回 nullptr。
   */
  [[nodiscard]] AcceleratedJob* job() const { return job_; }

 private:
  Renderer* renderer_;  // 指向渲染器后端的指针 (如果为真实纹理)

  VideoParams params_;  // 纹理的视频参数

  QVariant id_;  // 原生纹理句柄 (例如 OpenGL 中的 GLuint)，存储在 QVariant 中

  AcceleratedJob* job_;  // (可选) 指向描述如何生成此纹理数据的 AcceleratedJob 的指针。
                         // 注意：这里是裸指针，其生命周期需要被仔细管理。
                         // 如果 Texture 对象拥有这个 job_，则需要在析构函数中 delete它。
                         // 如果不拥有，则需要确保 job_ 在 Texture 析构前保持有效或被正确处理。
};

}  // namespace olive

// 声明 TexturePtr 类型为元类型，以便在 QVariant 中使用或在信号槽中传递
Q_DECLARE_METATYPE(olive::TexturePtr)

#endif  // RENDERTEXTURE_H