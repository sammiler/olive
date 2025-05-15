

#include "ocioutils.h"

namespace olive {

OCIO::BitDepth OCIOUtils::GetOCIOBitDepthFromPixelFormat(PixelFormat format) {
  switch (static_cast<PixelFormat::Format>(format)) {
    case PixelFormat::U8:
      return OCIO::BIT_DEPTH_UINT8;
    case PixelFormat::U16:
      return OCIO::BIT_DEPTH_UINT16;
      break;
    case PixelFormat::F16:
      return OCIO::BIT_DEPTH_F16;
      break;
    case PixelFormat::F32:
      return OCIO::BIT_DEPTH_F32;
      break;
    case PixelFormat::INVALID:
    case PixelFormat::COUNT:
      break;
  }

  return OCIO::BIT_DEPTH_UNKNOWN;
}

}  // namespace olive
