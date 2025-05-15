

#include "oiioutils.h"

#include <QDebug>

namespace olive {

void OIIOUtils::FrameToBuffer(const Frame *frame, OIIO::ImageBuf *buf) {
  buf->set_pixels(OIIO::ROI(), buf->spec().format, frame->const_data(), OIIO::AutoStride, frame->linesize_bytes());
}

void OIIOUtils::BufferToFrame(OIIO::ImageBuf *buf, Frame *frame) {
  buf->get_pixels(OIIO::ROI(), buf->spec().format, frame->data(), OIIO::AutoStride, frame->linesize_bytes());
}

rational OIIOUtils::GetPixelAspectRatioFromOIIO(const OIIO::ImageSpec &spec) {
  return rational::fromDouble(spec.get_float_attribute("PixelAspectRatio", 1));
}

PixelFormat OIIOUtils::GetFormatFromOIIOBasetype(OIIO::TypeDesc::BASETYPE type) {
  switch (type) {
    case OIIO::TypeDesc::UNKNOWN:
    case OIIO::TypeDesc::NONE:
      break;

    case OIIO::TypeDesc::INT8:
    case OIIO::TypeDesc::INT16:
    case OIIO::TypeDesc::INT32:
    case OIIO::TypeDesc::UINT32:
    case OIIO::TypeDesc::INT64:
    case OIIO::TypeDesc::UINT64:
    case OIIO::TypeDesc::STRING:
    case OIIO::TypeDesc::PTR:
    case OIIO::TypeDesc::LASTBASE:
    case OIIO::TypeDesc::DOUBLE:
      qDebug() << "Tried to use unknown OIIO base type";
      break;

    case OIIO::TypeDesc::UINT8:
      return PixelFormat(PixelFormat::U8);
    case OIIO::TypeDesc::UINT16:
      return PixelFormat(PixelFormat::U16);
    case OIIO::TypeDesc::HALF:
      return PixelFormat(PixelFormat::F16);
    case OIIO::TypeDesc::FLOAT:
      return PixelFormat(PixelFormat::F32);
  }

  return PixelFormat(PixelFormat::INVALID);
}

}  // namespace olive
