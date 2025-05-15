

#include "exportcodec.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/pixdesc.h>
}

namespace olive {

QString ExportCodec::GetCodecName(ExportCodec::Codec c) {
  switch (c) {
    case kCodecDNxHD:
      return tr("DNxHD");
    case kCodecH264:
      return tr("H.264");
    case kCodecH264rgb:
      return tr("H.264 RGB");
    case kCodecH265:
      return tr("H.265");
    case kCodecOpenEXR:
      return tr("OpenEXR");
    case kCodecPNG:
      return tr("PNG");
    case kCodecProRes:
      return tr("ProRes");
    case kCodecCineform:
      return tr("Cineform");
    case kCodecTIFF:
      return tr("TIFF");
    case kCodecMP2:
      return tr("MP2");
    case kCodecMP3:
      return tr("MP3");
    case kCodecAAC:
      return tr("AAC");
    case kCodecPCM:
      return tr("PCM (Uncompressed)");
    case kCodecFLAC:
      return tr("FLAC");
    case kCodecOpus:
      return tr("Opus");
    case kCodecVorbis:
      return tr("Vorbis");
    case kCodecVP9:
      return tr("VP9");
    case kCodecAV1:
      return tr("AV1");
    case kCodecSRT:
      return tr("SubRip SRT");
    case kCodecCount:
      break;
  }

  return tr("Unknown");
}

bool ExportCodec::IsCodecAStillImage(ExportCodec::Codec c) {
  switch (c) {
    case kCodecDNxHD:
    case kCodecH264:
    case kCodecH264rgb:
    case kCodecH265:
    case kCodecProRes:
    case kCodecCineform:
    case kCodecMP2:
    case kCodecMP3:
    case kCodecAAC:
    case kCodecPCM:
    case kCodecVorbis:
    case kCodecOpus:
    case kCodecFLAC:
    case kCodecVP9:
    case kCodecAV1:
    case kCodecSRT:
      return false;
    case kCodecOpenEXR:
    case kCodecPNG:
    case kCodecTIFF:
      return true;
    case kCodecCount:
      break;
  }

  return false;
}

bool ExportCodec::IsCodecLossless(Codec c) {
  switch (c) {
    case kCodecPCM:
    case kCodecFLAC:
      return true;
    case kCodecDNxHD:
    case kCodecH264:
    case kCodecH264rgb:
    case kCodecH265:
    case kCodecProRes:
    case kCodecCineform:
    case kCodecMP2:
    case kCodecMP3:
    case kCodecAAC:
    case kCodecVorbis:
    case kCodecOpus:
    case kCodecVP9:
    case kCodecAV1:
    case kCodecSRT:
    case kCodecOpenEXR:
    case kCodecPNG:
    case kCodecTIFF:
    case kCodecCount:
      break;
  }

  return false;
}

}  // namespace olive
