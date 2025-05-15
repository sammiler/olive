

#ifndef FFMPEGABSTRACTION_H
#define FFMPEGABSTRACTION_H

extern "C" {
#include <libavcodec/avcodec.h>   // FFmpeg 编解码核心库
#include <libavformat/avformat.h> // FFmpeg 封装格式处理库
#include <libavutil/frame.h>      // FFmpeg AVFrame 相关 (av_frame_alloc, av_frame_free)
#include <libswscale/swscale.h>   // FFmpeg 图像缩放和像素格式转换库
}

#include <olive/core/core.h> // 包含 olive::core 命名空间的基础定义，如 PixelFormat, SampleFormat
#include <memory>            // 为了 std::shared_ptr

#include "render/videoparams.h"  // 包含 VideoParams (可能也定义了PixelFormat, SampleFormat)

namespace olive {

using namespace core; // 使用 olive::core 命名空间

/**
 * @brief 提供 FFmpeg 相关转换和工具函数的静态工具类。
 *
 *此类封装了在 Olive 原生数据类型与 FFmpeg 数据类型之间进行转换的逻辑，
 * 例如像素格式、采样格式等的相互转换。它还包含一些处理 FFmpeg 特定
 * 概念（如颜色空间、JPEG颜色范围修正）的辅助函数。
 */
class FFmpegUtils {
 public:
  /**
   * @brief 根据给定的 FFmpeg 像素格式 (AVPixelFormat)，返回一个 Olive 支持的、
   * 且能以最小数据损失进行转换的目标 AVPixelFormat。
   *
   * @param pix_fmt 输入的 FFmpeg 像素格式。
   * @param maximum 可选参数，指定转换时允许的 Olive 最大像素格式深度/复杂度。
   * 如果为 PixelFormat::INVALID，则不施加此限制。
   * @return AVPixelFormat 一个推荐的、与 Olive 兼容的 FFmpeg 像素格式，用于转换。
   */
  static AVPixelFormat GetCompatiblePixelFormat(const AVPixelFormat& pix_fmt,
                                                PixelFormat maximum = PixelFormat(PixelFormat::INVALID));

  /**
   * @brief 根据给定的 Olive 原生像素格式 (PixelFormat)，返回一个等效的、
   * 且能以最小数据损失从原生帧转换为 AVFrame 的 Olive 原生像素格式。
   * （此函数签名似乎与上一个重载的意图略有重叠或混淆，通常是返回AVPixelFormat）。
   * 假设意图是找到最接近的 Olive 内部表示。
   * @param pix_fmt 输入的 Olive 原生像素格式。
   * @return PixelFormat 一个推荐的 Olive 原生像素格式。
   */
  static PixelFormat GetCompatiblePixelFormat(const PixelFormat& pix_fmt);

  /**
   * @brief 将给定的 Olive 原生像素格式 (PixelFormat) 转换为对应的 FFmpeg 像素格式 (AVPixelFormat)。
   * @param pix_fmt 输入的 Olive 原生像素格式。
   * @param channel_layout （此参数对于像素格式转换通常不是必需的，除非处理特定平面格式或打包格式，
   * 此处可能指与某些特定格式相关的通道布局信息，但通常像素格式本身已包含此信息。）
   * 假设在某些情况下可能影响平面格式的选择。
   * @return AVPixelFormat 对应的 FFmpeg 像素格式。
   */
  static AVPixelFormat GetFFmpegPixelFormat(const PixelFormat& pix_fmt, int channel_layout);

  /**
   * @brief 将给定的 FFmpeg 音频采样格式 (AVSampleFormat) 转换为对应的 Olive 原生采样格式 (SampleFormat)。
   * @param smp_fmt 输入的 FFmpeg 音频采样格式。
   * @return SampleFormat 对应的 Olive 原生采样格式。
   */
  static SampleFormat GetNativeSampleFormat(const AVSampleFormat& smp_fmt);

  /**
   * @brief 将给定的 Olive 原生音频采样格式 (SampleFormat) 转换为对应的 FFmpeg 音频采样格式 (AVSampleFormat)。
   * @param smp_fmt 输入的 Olive 原生音频采样格式。
   * @return AVSampleFormat 对应的 FFmpeg 音频采样格式。
   */
  static AVSampleFormat GetFFmpegSampleFormat(const SampleFormat& smp_fmt);

  /**
   * @brief 从 AVColorSpace 枚举成员获取对应的 SWS_CS_* 宏值 (用于 libswscale)。
   *
   * FFmpeg 内部对颜色空间的表示 (AVColorSpace) 与 libswscale 使用的颜色空间宏 (SWS_CS_*)
   * 并不直接等同，此函数用于进行这种转换。
   * @param cs FFmpeg 的 AVColorSpace 枚举值。
   * @return int 对应的 libswscale 颜色空间宏值 (例如 SWS_CS_ITU709)。
   */
  static int GetSwsColorspaceFromAVColorSpace(AVColorSpace cs);

  /**
   * @brief 将 "JPEG" (full-range) 颜色空间的像素格式转换为其对应的常规 (limited/broadcast-range) 像素格式。
   *
   * FFmpeg 中一些标记为 "JPEG" 的颜色空间（如 AV_PIX_FMT_YUVJ420P）实际上是全范围颜色，
   * 但这些格式已被弃用，推荐使用常规颜色空间并明确设置 `color_range` 属性。
   * 由于 FFmpeg 内部仍可能使用这些 "JPEG" 空间，此函数用于将其转换为等效的常规空间，
   * 以便与 Olive 自身对颜色范围的处理保持一致。
   * @param f 输入的 FFmpeg 像素格式，可能是一个 "JPEG" 颜色空间格式。
   * @return AVPixelFormat 转换后的常规颜色空间像素格式。如果输入格式不是 "JPEG" 空间，则原样返回。
   */
  static AVPixelFormat ConvertJPEGSpaceToRegularSpace(AVPixelFormat f);
};

/**
 * @brief 指向 AVFrame 对象的共享指针类型定义，并附带自定义删除器。
 * 当 AVFramePtr 超出作用域或被重置时，它将自动调用 av_frame_free() 来释放 AVFrame。
 */
using AVFramePtr = std::shared_ptr<AVFrame>;

/**
 * @brief 创建一个管理现有 AVFrame 指针的 AVFramePtr 共享指针。
 * @param f 指向要管理的 AVFrame 对象的原始指针。该指针不应由调用者再次释放。
 * @return AVFramePtr 指向 AVFrame 的共享指针，该指针将在销毁时自动释放 AVFrame。
 */
inline AVFramePtr CreateAVFramePtr(AVFrame* f) {
  return {f, [](AVFrame* g) { av_frame_free(&g); }}; // 自定义删除器
}

/**
 * @brief 分配一个新的 AVFrame 并返回一个管理它的 AVFramePtr 共享指针。
 * @return AVFramePtr 指向新分配的 AVFrame 的共享指针。如果分配失败，内部指针可能为 nullptr。
 */
inline AVFramePtr CreateAVFramePtr() {
  return CreateAVFramePtr(av_frame_alloc());
}

}  // namespace olive

#endif  // FFMPEGABSTRACTION_H