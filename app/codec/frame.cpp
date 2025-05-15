

#include "frame.h"

#include <OpenImageIO/imagebuf.h>
#include <QDebug>
#include <QtGlobal>
#include <QtMath>

#include "common/oiioutils.h"
#include "render/framemanager.h"

namespace olive {

Frame::Frame() : data_(nullptr), data_size_(0), timestamp_(0) {}

Frame::~Frame() { destroy(); }

FramePtr Frame::Create() { return std::make_shared<Frame>(); }

const VideoParams &Frame::video_params() const { return params_; }

void Frame::set_video_params(const VideoParams &params) {
  params_ = params;

  linesize_ = generate_linesize_bytes(width(), params_.format(), params_.channel_count());
  linesize_pixels_ = linesize_ / params_.GetBytesPerPixel();
}

FramePtr Frame::Interlace(const FramePtr &top, const FramePtr &bottom) {
  if (top->video_params() != bottom->video_params()) {
    qCritical() << "Tried to interlace two frames that had incompatible parameters";
    return nullptr;
  }

  FramePtr interlaced = Frame::Create();
  interlaced->set_video_params(top->video_params());
  interlaced->allocate();

  int linesize = interlaced->linesize_bytes();

  for (int i = 0; i < interlaced->height(); i++) {
    FramePtr which = (i % 2 == 0) ? top : bottom;

    memcpy(interlaced->data() + i * linesize, which->const_data() + i * linesize, linesize);
  }

  return interlaced;
}

int Frame::generate_linesize_bytes(int width, PixelFormat format, int channel_count) {
  // Align to 32 bytes (not sure if this is necessary?)
  return VideoParams::GetBytesPerPixel(format, channel_count) * ((width + 31) & ~31);
}

Color Frame::get_pixel(int x, int y) const {
  if (!contains_pixel(x, y)) {
    return {};
  }

  int byte_offset = y * linesize_bytes() + x * video_params().GetBytesPerPixel();

  return Color(reinterpret_cast<const char *>(data_ + byte_offset), video_params().format(),
               video_params().channel_count());
}

bool Frame::contains_pixel(int x, int y) const {
  return (is_allocated() && x >= 0 && x < width() && y >= 0 && y < height());
}

void Frame::set_pixel(int x, int y, const Color &c) {
  if (!contains_pixel(x, y)) {
    return;
  }

  int byte_offset = y * linesize_bytes() + x * video_params().GetBytesPerPixel();

  c.toData(reinterpret_cast<char *>(data_ + byte_offset), video_params().format(), video_params().channel_count());
}

bool Frame::allocate() {
  // Assume this frame is intended to be a video frame
  if (!params_.is_valid()) {
    qWarning() << "Tried to allocate a frame with invalid parameters";
    return false;
  }

  if (is_allocated()) {
    // Already allocated
    return true;
  }

  data_size_ = linesize_ * height();
  data_ = FrameManager::Allocate(data_size_);

  return true;
}

void Frame::destroy() {
  if (is_allocated()) {
    FrameManager::Deallocate(data_size_, data_);

    data_size_ = 0;
    data_ = nullptr;
  }
}

FramePtr Frame::convert(PixelFormat format) const {
  // Create new params with destination format
  VideoParams params = params_;
  params.set_format(format);

  // Create new frame
  FramePtr converted = Frame::Create();
  converted->set_video_params(params);
  converted->set_timestamp(timestamp_);
  converted->allocate();

  // Do the conversion through OIIO for convenience
  OIIO::ImageBuf src(
      OIIO::ImageSpec(width(), height(), channel_count(), OIIOUtils::GetOIIOBaseTypeFromFormat(this->format())));

  OIIOUtils::FrameToBuffer(this, &src);

  OIIO::ImageBuf dst(OIIO::ImageSpec(converted->width(), converted->height(), channel_count(),
                                     OIIOUtils::GetOIIOBaseTypeFromFormat(format)));

  if (dst.copy_pixels(src)) {
    OIIOUtils::BufferToFrame(&dst, converted.get());
    return converted;
  } else {
    return nullptr;
  }
}

}  // namespace olive
