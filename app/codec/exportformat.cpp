#include "exportformat.h"

#include "encoder.h"

namespace olive {

QString ExportFormat::GetName(olive::ExportFormat::Format f) {
  switch (f) {
    case kFormatDNxHD:
      return tr("DNxHD");
    case kFormatMatroska:
      return tr("Matroska Video");
    case kFormatMPEG4Video:
      return tr("MPEG-4 Video");
    case kFormatMPEG4Audio:
      return tr("MPEG-4 Audio");
    case kFormatOpenEXR:
      return tr("OpenEXR");
    case kFormatPNG:
      return tr("PNG");
    case kFormatTIFF:
      return tr("TIFF");
    case kFormatQuickTime:
      return tr("QuickTime");
    case kFormatWAV:
      return tr("Wave Audio");
    case kFormatAIFF:
      return tr("AIFF");
    case kFormatMP3:
      return tr("MP3");
    case kFormatFLAC:
      return tr("FLAC");
    case kFormatOgg:
      return tr("Ogg");
    case kFormatWebM:
      return tr("WebM");
    case kFormatSRT:
      return tr("SubRip SRT");

    case kFormatCount:
      break;
  }

  return tr("Unknown");
}

QString ExportFormat::GetExtension(ExportFormat::Format f) {
  switch (f) {
    case kFormatDNxHD:
      return QStringLiteral("mxf");
    case kFormatMatroska:
      return QStringLiteral("mkv");
    case kFormatMPEG4Video:
      return QStringLiteral("mp4");
    case kFormatMPEG4Audio:
      return QStringLiteral("m4a");
    case kFormatOpenEXR:
      return QStringLiteral("exr");
    case kFormatPNG:
      return QStringLiteral("png");
    case kFormatTIFF:
      return QStringLiteral("tiff");
    case kFormatQuickTime:
      return QStringLiteral("mov");
    case kFormatWAV:
      return QStringLiteral("wav");
    case kFormatAIFF:
      return QStringLiteral("aiff");
    case kFormatMP3:
      return QStringLiteral("mp3");
    case kFormatFLAC:
      return QStringLiteral("flac");
    case kFormatOgg:
      return QStringLiteral("ogg");
    case kFormatWebM:
      return QStringLiteral("webm");
    case kFormatSRT:
      return QStringLiteral("srt");
    case kFormatCount:
      break;
  }

  return {};
}

QList<ExportCodec::Codec> ExportFormat::GetVideoCodecs(ExportFormat::Format f) {
  switch (f) {
    case kFormatDNxHD:
      return {ExportCodec::kCodecDNxHD};
    case kFormatMatroska:
      return {ExportCodec::kCodecH264, ExportCodec::kCodecH264rgb, ExportCodec::kCodecH265, ExportCodec::kCodecVP9};
    case kFormatMPEG4Video:
      return {ExportCodec::kCodecH264, ExportCodec::kCodecH264rgb, ExportCodec::kCodecH265};
    case kFormatOpenEXR:
      return {ExportCodec::kCodecOpenEXR};
    case kFormatPNG:
      return {ExportCodec::kCodecPNG};
    case kFormatTIFF:
      return {ExportCodec::kCodecTIFF};
    case kFormatQuickTime:
      return {ExportCodec::kCodecH264, ExportCodec::kCodecH264rgb, ExportCodec::kCodecH265, ExportCodec::kCodecProRes,
              ExportCodec::kCodecCineform};
    case kFormatWebM:
      return {ExportCodec::kCodecAV1, ExportCodec::kCodecVP9};
    case kFormatOgg:
    case kFormatWAV:
    case kFormatMPEG4Audio:
    case kFormatAIFF:
    case kFormatMP3:
    case kFormatFLAC:
    case kFormatSRT:
    case kFormatCount:
      break;
  }

  return {};
}

QList<ExportCodec::Codec> ExportFormat::GetAudioCodecs(ExportFormat::Format f) {
  switch (f) {
    // Video/audio formats
    case kFormatDNxHD:
      return {ExportCodec::kCodecPCM};
    case kFormatMatroska:
      return {ExportCodec::kCodecAAC,    ExportCodec::kCodecMP2,  ExportCodec::kCodecMP3, ExportCodec::kCodecPCM,
              ExportCodec::kCodecVorbis, ExportCodec::kCodecOpus, ExportCodec::kCodecFLAC};
    case kFormatMPEG4Video:
    case kFormatMPEG4Audio:
      return {ExportCodec::kCodecAAC, ExportCodec::kCodecMP2, ExportCodec::kCodecMP3};
    case kFormatQuickTime:
      return {ExportCodec::kCodecAAC, ExportCodec::kCodecMP2, ExportCodec::kCodecMP3, ExportCodec::kCodecPCM};
    case kFormatWebM:
      return {ExportCodec::kCodecOpus, ExportCodec::kCodecAAC, ExportCodec::kCodecMP2,
              ExportCodec::kCodecMP3,  ExportCodec::kCodecPCM, ExportCodec::kCodecVorbis};

    // Audio only formats
    case kFormatWAV:
      return {ExportCodec::kCodecPCM};
    case kFormatAIFF:
      return {ExportCodec::kCodecPCM};
    case kFormatMP3:
      return {ExportCodec::kCodecMP3};
    case kFormatFLAC:
      return {ExportCodec::kCodecFLAC};
    case kFormatOgg:
      return {ExportCodec::kCodecOpus, ExportCodec::kCodecVorbis, ExportCodec::kCodecPCM};

    // Video only formats
    case kFormatOpenEXR:
    case kFormatPNG:
    case kFormatTIFF:
    case kFormatSRT:
    case kFormatCount:
      break;
  }

  return {};
}

QList<ExportCodec::Codec> ExportFormat::GetSubtitleCodecs(Format f) {
  switch (f) {
    case kFormatDNxHD:
    case kFormatMPEG4Video:
    case kFormatMPEG4Audio:
    case kFormatOpenEXR:
    case kFormatQuickTime:
    case kFormatPNG:
    case kFormatTIFF:
    case kFormatWAV:
    case kFormatAIFF:
    case kFormatMP3:
    case kFormatFLAC:
    case kFormatOgg:
    case kFormatWebM:
    case kFormatCount:
      break;
    case kFormatMatroska:
    case kFormatSRT:
      return {ExportCodec::kCodecSRT};
  }

  return {};
}

QStringList ExportFormat::GetPixelFormatsForCodec(ExportFormat::Format f, ExportCodec::Codec c) {
  Encoder* e = Encoder::CreateFromFormat(f, EncodingParams());
  QStringList list;

  if (e) {
    list = e->GetPixelFormatsForCodec(c);
    delete e;
  }

  return list;
}

std::vector<SampleFormat> ExportFormat::GetSampleFormatsForCodec(Format format, ExportCodec::Codec c) {
  std::vector<SampleFormat> f;
  Encoder* e = Encoder::CreateFromFormat(format, EncodingParams());

  if (e) {
    f = e->GetSampleFormatsForCodec(c);
    delete e;
  }

  return f;
}

}  // namespace olive
