#ifndef OIIOUTILS_H
#define OIIOUTILS_H

#include <OpenImageIO/imagebuf.h>  // OpenImageIO 图像缓冲区类
#include <OpenImageIO/imageio.h>   // 为了 OIIO::ImageSpec (虽然可能已通过 imagebuf.h 间接包含)
#include <OpenImageIO/typedesc.h>  // OpenImageIO 类型描述符

#include "codec/frame.h"         // 包含 olive::Frame, olive::PixelFormat, olive::rational
#include "common/define.h"       // 包含 olive::PixelFormat (如果上述都未包含)
#include "render/videoparams.h"  // 包含 olive::PixelFormat (如果 frame.h 未完全定义或为了VideoParams中的其他类型)

// 前向声明 (如果需要)
// namespace OIIO {
// class ImageBuf;
// class ImageSpec;
// }
// namespace olive {
// class Frame;
// enum class PixelFormat;
// class rational;
// }

namespace olive {

/**
 * @brief 提供 OpenImageIO (OIIO) 相关转换和工具函数的静态工具类。
 *
 * 此类封装了在 Olive 内部数据结构 (如 Frame, PixelFormat) 与
 * OpenImageIO 库使用的数据结构 (如 ImageBuf, TypeDesc::BASETYPE, ImageSpec)
 * 之间进行转换的逻辑。
 */
class OIIOUtils {
 public:
  /**
   * @brief 将 Olive 的 PixelFormat 枚举值转换为对应的 OIIO 基础类型 (TypeDesc::BASETYPE)。
   *
   * @param format 输入的 Olive 原生 PixelFormat 枚举值。
   * @return OIIO::TypeDesc::BASETYPE 对应的 OIIO 基础类型。
   * 例如，PixelFormat::U8 映射到 OIIO::TypeDesc::UINT8。
   * 如果输入格式无效或没有直接映射，则返回 OIIO::TypeDesc::UNKNOWN。
   */
  static OIIO::TypeDesc::BASETYPE GetOIIOBaseTypeFromFormat(PixelFormat format) {
    switch (static_cast<PixelFormat::Format>(format)) {  // 强制转换为底层枚举类型以用于 switch
      case PixelFormat::U8:
        return OIIO::TypeDesc::UINT8;
      case PixelFormat::U16:
        return OIIO::TypeDesc::UINT16;
      case PixelFormat::F16:
        return OIIO::TypeDesc::HALF;  // OIIO 中的半精度浮点
      case PixelFormat::F32:
        return OIIO::TypeDesc::FLOAT;
      case PixelFormat::INVALID:  // 处理无效格式
      case PixelFormat::COUNT:    // 处理计数器值 (通常不应作为实际格式传递)
        break;                    // 跳出 switch，将返回 UNKNOWN
    }

    return OIIO::TypeDesc::UNKNOWN;  // 默认或错误情况
  }

  /**
   * @brief 将 Olive 的 Frame 对象数据转换为（或填充到）一个 OIIO::ImageBuf 对象。
   * 此函数负责将 `frame`中的像素数据和元数据（如尺寸、格式）设置到 `buf`中。
   * @param frame 指向源 olive::Frame 对象的常量指针。此帧应已分配并包含有效数据。
   * @param buf 指向目标 OIIO::ImageBuf 对象的指针。此缓冲区将被修改以反映frame的内容。
   */
  static void FrameToBuffer(const Frame* frame, OIIO::ImageBuf* buf);

  /**
   * @brief 将 OIIO::ImageBuf 对象的数据转换为（或填充到）一个 Olive 的 Frame 对象。
   * 此函数负责从 `buf`中读取像素数据和元数据，并设置到 `frame`中。
   * `frame` 对象在调用此函数后可能需要调用 `allocate()`（如果其尺寸或格式发生变化）。
   * @param buf 指向源 OIIO::ImageBuf 对象的指针。
   * @param frame 指向目标 olive::Frame 对象的指针。此帧将被修改以反映buf的内容。
   */
  static void BufferToFrame(OIIO::ImageBuf* buf, Frame* frame);

  /**
   * @brief 将 OIIO 基础类型 (OIIO::TypeDesc::BASETYPE) 转换为对应的 Olive PixelFormat 枚举值。
   * @param type 输入的 OIIO 基础类型。
   * @return PixelFormat 对应的 Olive 原生 PixelFormat 枚举值。
   * 如果找不到精确匹配或输入类型不被支持，则返回 PixelFormat::INVALID。
   */
  static PixelFormat GetFormatFromOIIOBasetype(OIIO::TypeDesc::BASETYPE type);

  /**
   * @brief 从 OIIO::ImageSpec 对象中提取像素宽高比 (Pixel Aspect Ratio)。
   * OIIO 的 ImageSpec 可能包含图像的像素宽高比元数据。
   * @param spec 包含图像元数据的 OIIO::ImageSpec 对象。
   * @return rational 像素宽高比的有理数表示。如果 ImageSpec 中未定义或无法解析，
   * 则可能返回一个默认值 (例如 1/1，表示方形像素)。
   */
  static rational GetPixelAspectRatioFromOIIO(const OIIO::ImageSpec& spec);
};

}  // namespace olive

#endif  // OIIOUTILS_H