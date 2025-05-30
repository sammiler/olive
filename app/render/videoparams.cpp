#include "videoparams.h"

extern "C" {
#include <libavutil/avutil.h>
}

#include <QCoreApplication>
#include <QtMath>

#include "core.h"

namespace olive {

const int VideoParams::kInternalChannelCount = kRGBAChannelCount;

const rational VideoParams::kPixelAspectSquare(1);
const rational VideoParams::kPixelAspectNTSCStandard(8, 9);
const rational VideoParams::kPixelAspectNTSCWidescreen(32, 27);
const rational VideoParams::kPixelAspectPALStandard(16, 15);
const rational VideoParams::kPixelAspectPALWidescreen(64, 45);
const rational VideoParams::kPixelAspect1080Anamorphic(4, 3);

const QVector<rational> VideoParams::kSupportedFrameRates = {
    rational(10, 1),        // 10 FPS
    rational(15, 1),        // 15 FPS
    rational(24000, 1001),  // 23.976 FPS
    rational(24, 1),        // 24 FPS
    rational(25, 1),        // 25 FPS
    rational(30000, 1001),  // 29.97 FPS
    rational(30, 1),        // 30 FPS
    rational(48000, 1001),  // 47.952 FPS
    rational(48, 1),        // 48 FPS
    rational(50, 1),        // 50 FPS
    rational(60000, 1001),  // 59.94 FPS
    rational(60, 1)         // 60 FPS
};

const QVector<int> VideoParams::kSupportedDividers = {1, 2, 3, 4, 6, 8, 12, 16};

const QVector<rational> VideoParams::kStandardPixelAspects = {
    VideoParams::kPixelAspectSquare,         VideoParams::kPixelAspectNTSCStandard,
    VideoParams::kPixelAspectNTSCWidescreen, VideoParams::kPixelAspectPALStandard,
    VideoParams::kPixelAspectPALWidescreen,  VideoParams::kPixelAspect1080Anamorphic};

VideoParams::VideoParams()
    : width_(0),
      height_(0),
      depth_(0),
      format_(PixelFormat::INVALID),
      channel_count_(0),
      interlacing_(Interlacing::kInterlaceNone),
      divider_(1) {
  set_defaults_for_footage();
}

VideoParams::VideoParams(int width, int height, PixelFormat format, int nb_channels, const rational &pixel_aspect_ratio,
                         Interlacing interlacing, int divider)
    : width_(width),
      height_(height),
      depth_(1),
      format_(format),
      channel_count_(nb_channels),
      pixel_aspect_ratio_(pixel_aspect_ratio),
      interlacing_(interlacing),
      divider_(divider) {
  calculate_effective_size();
  validate_pixel_aspect_ratio();
  set_defaults_for_footage();
}

VideoParams::VideoParams(int width, int height, int depth, PixelFormat format, int nb_channels,
                         const rational &pixel_aspect_ratio, VideoParams::Interlacing interlacing, int divider)
    : width_(width),
      height_(height),
      depth_(depth),
      format_(format),
      channel_count_(nb_channels),
      pixel_aspect_ratio_(pixel_aspect_ratio),
      interlacing_(interlacing),
      divider_(divider) {
  calculate_effective_size();
  validate_pixel_aspect_ratio();
  set_defaults_for_footage();
}

VideoParams::VideoParams(int width, int height, const rational &time_base, PixelFormat format, int nb_channels,
                         const rational &pixel_aspect_ratio, Interlacing interlacing, int divider)
    : width_(width),
      height_(height),
      depth_(1),
      time_base_(time_base),
      format_(format),
      channel_count_(nb_channels),
      pixel_aspect_ratio_(pixel_aspect_ratio),
      interlacing_(interlacing),
      divider_(divider),
      frame_rate_(time_base.flipped()) {
  calculate_effective_size();
  validate_pixel_aspect_ratio();
  set_defaults_for_footage();
}

int VideoParams::generate_auto_divider(qint64 width, qint64 height) {
  const int target_res = 1280 * 720;

  qint64 megapixels = width * height;

  double squared_divider = double(megapixels) / double(target_res);
  double divider = qSqrt(squared_divider);

  if (divider <= kSupportedDividers.first()) {
    return kSupportedDividers.first();
  } else if (divider >= kSupportedDividers.last()) {
    return kSupportedDividers.last();
  } else {
    for (int i = 1; i < kSupportedDividers.size(); i++) {
      int prev_divider = kSupportedDividers.at(i - 1);
      int next_divider = kSupportedDividers.at(i);

      if (divider >= prev_divider && divider <= next_divider) {
        double prev_diff = qAbs(prev_divider - divider);
        double next_diff = qAbs(next_divider - divider);

        if (prev_diff < next_diff) {
          return prev_divider;
        } else {
          return next_divider;
        }
      }
    }

    // Fallback
    return 1;
  }
}

bool VideoParams::operator==(const VideoParams &rhs) const {
  return width() == rhs.width() && height() == rhs.height() && depth() == rhs.depth() &&
         interlacing() == rhs.interlacing() && time_base() == rhs.time_base() &&
         static_cast<PixelFormat::Format>(format()) == static_cast<PixelFormat::Format>(rhs.format()) &&
         pixel_aspect_ratio() == rhs.pixel_aspect_ratio() && divider() == rhs.divider() &&
         channel_count() == rhs.channel_count();
}

bool VideoParams::operator!=(const VideoParams &rhs) const { return !(*this == rhs); }

int VideoParams::GetBytesPerChannel(PixelFormat format) {
  switch (static_cast<PixelFormat::Format>(format)) {
    case PixelFormat::INVALID:
    case PixelFormat::COUNT:
      break;
    case PixelFormat::U8:
      return 1;
    case PixelFormat::U16:
    case PixelFormat::F16:
      return 2;
    case PixelFormat::F32:
      return 4;
  }

  return 0;
}

int VideoParams::GetBytesPerPixel(PixelFormat format, int channels) { return GetBytesPerChannel(format) * channels; }

QString VideoParams::GetNameForDivider(int div) {
  if (div == 1) {
    return QCoreApplication::translate("VideoParams", "Full");
  } else {
    return QCoreApplication::translate("VideoParams", "1/%1").arg(div);
  }
}

QString VideoParams::GetFormatName(PixelFormat format) {
  switch (static_cast<PixelFormat::Format>(format)) {
    case PixelFormat::U8:
      return QCoreApplication::translate("VideoParams", "8-bit");
    case PixelFormat::U16:
      return QCoreApplication::translate("VideoParams", "16-bit Integer");
    case PixelFormat::F16:
      return QCoreApplication::translate("VideoParams", "Half-Float (16-bit)");
    case PixelFormat::F32:
      return QCoreApplication::translate("VideoParams", "Full-Float (32-bit)");
    case PixelFormat::INVALID:
    case PixelFormat::COUNT:
      break;
  }

  return QCoreApplication::translate("VideoParams", "Unknown (0x%1)")
      .arg(static_cast<PixelFormat::Format>(format), 0, 16);
}

int VideoParams::GetDividerForTargetResolution(int src_width, int src_height, int dst_width, int dst_height) {
  int divider = 0;
  int test_width, test_height;

  do {
    divider++;

    test_width = VideoParams::GetScaledDimension(src_width, divider);
    test_height = VideoParams::GetScaledDimension(src_height, divider);
  } while (test_width > dst_width || test_height > dst_height);

  return divider;
}

void VideoParams::calculate_effective_size() {
  effective_width_ = GetScaledDimension(width(), divider_);
  effective_height_ = GetScaledDimension(height(), divider_);
  effective_depth_ = (depth() == 1) ? depth() : GetScaledDimension(depth(), divider_);
  calculate_square_pixel_width();
}

void VideoParams::validate_pixel_aspect_ratio() {
  if (pixel_aspect_ratio_.isNull()) {
    pixel_aspect_ratio_ = rational(1);
  }
  calculate_square_pixel_width();
}

void VideoParams::set_defaults_for_footage() {
  enabled_ = true;
  stream_index_ = 0;
  video_type_ = kVideoTypeVideo;
  start_time_ = 0;
  duration_ = 0;
  premultiplied_alpha_ = false;
  x_ = 0;
  y_ = 0;
  color_range_ = kColorRangeDefault;
}

void VideoParams::calculate_square_pixel_width() {
  if (pixel_aspect_ratio_.denominator() != 0) {
    par_width_ = qRound(width_ * pixel_aspect_ratio_.toDouble());
  } else {
    par_width_ = width_;
  }
}

bool VideoParams::is_valid() const {
  return (width() > 0 && height() > 0 && !pixel_aspect_ratio_.isNull() &&
          static_cast<PixelFormat::Format>(format_) > PixelFormat::INVALID &&
          static_cast<PixelFormat::Format>(format_) < PixelFormat::COUNT && channel_count_ > 0);
}

QString VideoParams::FrameRateToString(const rational &frame_rate) {
  return QCoreApplication::translate("VideoParams", "%1 FPS").arg(frame_rate.toDouble());
}

QStringList VideoParams::GetStandardPixelAspectRatioNames() {
  QStringList strings = {QCoreApplication::translate("VideoParams", "Square Pixels (%1)"),
                         QCoreApplication::translate("VideoParams", "NTSC Standard (%1)"),
                         QCoreApplication::translate("VideoParams", "NTSC Widescreen (%1)"),
                         QCoreApplication::translate("VideoParams", "PAL Standard (%1)"),
                         QCoreApplication::translate("VideoParams", "PAL Widescreen (%1)"),
                         QCoreApplication::translate("VideoParams", "HD Anamorphic 1080 (%1)")};

  // Format each
  for (int i = 0; i < strings.size(); i++) {
    strings.replace(i, FormatPixelAspectRatioString(strings.at(i), kStandardPixelAspects.at(i)));
  }

  return strings;
}

QString VideoParams::FormatPixelAspectRatioString(const QString &format, const rational &ratio) {
  return format.arg(QString::number(ratio.toDouble(), 'f', 4));
}

int VideoParams::GetScaledDimension(int dim, int divider) { return dim / divider; }

int64_t VideoParams::get_time_in_timebase_units(const rational &time) const {
  if (time_base_.isNull()) {
    return AV_NOPTS_VALUE;
  }

  return Timecode::time_to_timestamp(time, time_base_) + start_time_;
}

void VideoParams::Load(QXmlStreamReader *reader) {
  while (XMLReadNextStartElement(reader)) {
    if (reader->name() == QStringLiteral("width")) {
      set_width(reader->readElementText().toInt());
    } else if (reader->name() == QStringLiteral("height")) {
      set_height(reader->readElementText().toInt());
    } else if (reader->name() == QStringLiteral("depth")) {
      set_depth(reader->readElementText().toInt());
    } else if (reader->name() == QStringLiteral("timebase")) {
      set_time_base(rational::fromString(reader->readElementText().toUtf8().constData()));
    } else if (reader->name() == QStringLiteral("format")) {
      set_format(PixelFormat(static_cast<PixelFormat::Format>(reader->readElementText().toInt())));
    } else if (reader->name() == QStringLiteral("channelcount")) {
      set_channel_count(reader->readElementText().toInt());
    } else if (reader->name() == QStringLiteral("pixelaspectratio")) {
      set_pixel_aspect_ratio(rational::fromString(reader->readElementText().toUtf8().constData()));
    } else if (reader->name() == QStringLiteral("interlacing")) {
      set_interlacing(static_cast<VideoParams::Interlacing>(reader->readElementText().toInt()));
    } else if (reader->name() == QStringLiteral("divider")) {
      set_divider(reader->readElementText().toInt());
    } else if (reader->name() == QStringLiteral("enabled")) {
      set_enabled(reader->readElementText().toInt());
    } else if (reader->name() == QStringLiteral("x")) {
      set_x(reader->readElementText().toFloat());
    } else if (reader->name() == QStringLiteral("y")) {
      set_y(reader->readElementText().toFloat());
    } else if (reader->name() == QStringLiteral("streamindex")) {
      set_stream_index(reader->readElementText().toInt());
    } else if (reader->name() == QStringLiteral("videotype")) {
      set_video_type(static_cast<VideoParams::Type>(reader->readElementText().toInt()));
    } else if (reader->name() == QStringLiteral("framerate")) {
      set_frame_rate(rational::fromString(reader->readElementText().toUtf8().constData()));
    } else if (reader->name() == QStringLiteral("starttime")) {
      set_start_time(reader->readElementText().toLongLong());
    } else if (reader->name() == QStringLiteral("duration")) {
      set_duration(reader->readElementText().toLongLong());
    } else if (reader->name() == QStringLiteral("premultipliedalpha")) {
      set_premultiplied_alpha(reader->readElementText().toInt());
    } else if (reader->name() == QStringLiteral("colorspace")) {
      set_colorspace(reader->readElementText());
    } else if (reader->name() == QStringLiteral("colorrange")) {
      set_color_range(static_cast<ColorRange>(reader->readElementText().toInt()));
    } else {
      reader->skipCurrentElement();
    }
  }
}

void VideoParams::Save(QXmlStreamWriter *writer) const {
  writer->writeTextElement(QStringLiteral("width"), QString::number(width_));
  writer->writeTextElement(QStringLiteral("height"), QString::number(height_));
  writer->writeTextElement(QStringLiteral("depth"), QString::number(depth_));
  writer->writeTextElement(QStringLiteral("timebase"), QString::fromStdString(time_base_.toString()));
  writer->writeTextElement(QStringLiteral("format"), QString::number(static_cast<PixelFormat::Format>(format_)));
  writer->writeTextElement(QStringLiteral("channelcount"), QString::number(channel_count_));
  writer->writeTextElement(QStringLiteral("pixelaspectratio"), QString::fromStdString(pixel_aspect_ratio_.toString()));
  writer->writeTextElement(QStringLiteral("interlacing"), QString::number(interlacing_));
  writer->writeTextElement(QStringLiteral("divider"), QString::number(divider_));
  writer->writeTextElement(QStringLiteral("enabled"), QString::number(enabled_));
  writer->writeTextElement(QStringLiteral("x"), QString::number(x_));
  writer->writeTextElement(QStringLiteral("y"), QString::number(y_));
  writer->writeTextElement(QStringLiteral("streamindex"), QString::number(stream_index_));
  writer->writeTextElement(QStringLiteral("videotype"), QString::number(video_type_));
  writer->writeTextElement(QStringLiteral("framerate"), QString::fromStdString(frame_rate_.toString()));
  writer->writeTextElement(QStringLiteral("starttime"), QString::number(start_time_));
  writer->writeTextElement(QStringLiteral("duration"), QString::number(duration_));
  writer->writeTextElement(QStringLiteral("premultipliedalpha"), QString::number(premultiplied_alpha_));
  writer->writeTextElement(QStringLiteral("colorspace"), colorspace_);
  writer->writeTextElement(QStringLiteral("colorrange"), QString::number(color_range_));
}

}  // namespace olive
