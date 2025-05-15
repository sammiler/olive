

#ifndef OCIOUTILS_H
#define OCIOUTILS_H

#include <OpenColorIO/OpenColorIO.h> // 包含 OpenColorIO 核心头文件
// 为 OpenColorIO 的命名空间 OCIO_NAMESPACE 定义一个更短的别名 OCIO
// OCIO_NAMESPACE 通常是 OpenColorIO_v2_x (x是版本号)
namespace OCIO = OCIO_NAMESPACE;

#include "render/videoparams.h" // 包含 olive::PixelFormat 定义 (假设在此文件中或其包含的文件中)
#include "common/define.h"      // 可能包含 olive::PixelFormat (如果 VideoParams 未包含所有PixelFormat相关定义)

namespace olive {

/**
 * @brief 提供 OpenColorIO (OCIO) 相关转换和工具函数的静态工具类。
 *
 * 此类封装了与 OpenColorIO 库交互时所需的一些辅助功能，
 * 例如将 Olive 内部的像素格式转换为 OCIO 使用的位深度表示。
 */
class OCIOUtils {
public:
  /**
   * @brief 将 Olive 的 PixelFormat 枚举值转换为对应的 OpenColorIO (OCIO) BitDepth 枚举值。
   *
   * 不同的像素格式有不同的位深度（例如，8位整数，16位浮点数等），
   * 此函数用于在 Olive 的表示和 OCIO 的表示之间进行映射。
   * @param format 输入的 Olive 原生 PixelFormat 枚举值。
   * @return OCIO::BitDepth 对应的 OCIO BitDepth 枚举值。
   * 如果找不到精确匹配或输入格式无效，可能会返回 OCIO::BIT_DEPTH_UNKNOWN 或其他默认值。
   */
  static OCIO::BitDepth GetOCIOBitDepthFromPixelFormat(PixelFormat format);
};

}  // namespace olive

#endif  // OCIOUTILS_H