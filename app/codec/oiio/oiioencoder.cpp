

#include "oiioencoder.h"

#include "common/oiioutils.h"

namespace olive {

OIIOEncoder::OIIOEncoder(const EncodingParams &params) : Encoder(params) {}

bool OIIOEncoder::Open() { return true; }

bool OIIOEncoder::WriteFrame(FramePtr frame, rational time) {
  std::string filename = GetFilenameForFrame(time).toUtf8().constData();

  auto output = OIIO::ImageOutput::create(filename);
  if (!output) {
    return false;
  }

  OIIO::TypeDesc type = OIIOUtils::GetOIIOBaseTypeFromFormat(frame->format());
  OIIO::ImageSpec spec(frame->width(), frame->height(), frame->channel_count(), type);

  if (!output->open(filename, spec)) {
    return false;
  }

  if (!output->write_image(type, frame->data(), OIIO::AutoStride, frame->linesize_bytes())) {
    return false;
  }

  if (!output->close()) {
    return false;
  }

  return true;
}

bool OIIOEncoder::WriteAudio(const SampleBuffer &audio) {
  // Do nothing
  return false;
}

bool OIIOEncoder::WriteSubtitle(const SubtitleBlock *sub_block) { return false; }

void OIIOEncoder::Close() {
  // Do nothing
}

}  // namespace olive
