/***

  Olive - Non-Linear Video Editor
  Copyright (C) 2022 Olive Team

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

***/

#ifndef VIDEOPARAMS_H
#define VIDEOPARAMS_H

#include <olive/core/core.h>
#include <QVector2D>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

namespace olive {

using namespace core;

class VideoParams {
 public:
  enum Interlacing { kInterlaceNone, kInterlacedTopFirst, kInterlacedBottomFirst };

  enum Type { kVideoTypeVideo, kVideoTypeStill, kVideoTypeImageSequence };
  enum ColorRange {
    kColorRangeLimited,  // 16_235
    kColorRangeFull,     // 0-255

    kColorRangeDefault = kColorRangeLimited
  };

  VideoParams();
  VideoParams(int width, int height, PixelFormat format, int nb_channels,
              const rational& pixel_aspect_ratio = rational(1), Interlacing interlacing = kInterlaceNone,
              int divider = 1);
  VideoParams(int width, int height, int depth, PixelFormat format, int nb_channels,
              const rational& pixel_aspect_ratio = rational(1), Interlacing interlacing = kInterlaceNone,
              int divider = 1);
  VideoParams(int width, int height, const rational& time_base, PixelFormat format, int nb_channels,
              const rational& pixel_aspect_ratio = rational(1), Interlacing interlacing = kInterlaceNone,
              int divider = 1);

  [[nodiscard]] int width() const { return width_; }

  void set_width(int width) {
    width_ = width;
    calculate_effective_size();
  }

  /**
   * @brief Returns width multiplied by pixel aspect ratio where applicable
   */
  [[nodiscard]] int square_pixel_width() const { return par_width_; }

  [[nodiscard]] QVector2D resolution() const { return {static_cast<float>(width_), static_cast<float>(height_)}; }

  [[nodiscard]] QVector2D square_resolution() const {
    return {static_cast<float>(par_width_), static_cast<float>(height_)};
  }

  [[nodiscard]] int height() const { return height_; }

  void set_height(int height) {
    height_ = height;
    calculate_effective_size();
  }

  [[nodiscard]] int depth() const { return depth_; }

  void set_depth(int depth) {
    depth_ = depth;
    calculate_effective_size();
  }

  [[nodiscard]] bool is_3d() const { return depth_ > 1; }

  [[nodiscard]] const rational& time_base() const { return time_base_; }

  void set_time_base(const rational& r) { time_base_ = r; }

  [[nodiscard]] rational frame_rate_as_time_base() const { return frame_rate_.flipped(); }

  [[nodiscard]] int divider() const { return divider_; }

  void set_divider(int d) {
    divider_ = d;
    calculate_effective_size();
  }

  [[nodiscard]] int effective_width() const { return effective_width_; }

  [[nodiscard]] int effective_height() const { return effective_height_; }

  [[nodiscard]] int effective_depth() const { return effective_depth_; }

  [[nodiscard]] PixelFormat format() const { return format_; }

  void set_format(PixelFormat f) { format_ = f; }

  [[nodiscard]] int channel_count() const { return channel_count_; }

  void set_channel_count(int c) { channel_count_ = c; }

  [[nodiscard]] const rational& pixel_aspect_ratio() const { return pixel_aspect_ratio_; }

  void set_pixel_aspect_ratio(const rational& r) {
    pixel_aspect_ratio_ = r;
    validate_pixel_aspect_ratio();
  }

  [[nodiscard]] Interlacing interlacing() const { return interlacing_; }

  void set_interlacing(Interlacing i) { interlacing_ = i; }

  static int generate_auto_divider(qint64 width, qint64 height);

  [[nodiscard]] bool is_valid() const;

  bool operator==(const VideoParams& rhs) const;
  bool operator!=(const VideoParams& rhs) const;

  static int GetBytesPerChannel(PixelFormat format);
  [[nodiscard]] int GetBytesPerChannel() const { return GetBytesPerChannel(format_); }

  static int GetBytesPerPixel(PixelFormat format, int channels);
  [[nodiscard]] int GetBytesPerPixel() const { return GetBytesPerPixel(format_, channel_count_); }

  static int GetBufferSize(int width, int height, PixelFormat format, int channels) {
    return width * height * GetBytesPerPixel(format, channels);
  }
  [[nodiscard]] int GetBufferSize() const { return GetBufferSize(width_, height_, format_, channel_count_); }

  static QString GetNameForDivider(int div);

  static bool FormatIsFloat(PixelFormat format) { return format.is_float(); }

  static QString GetFormatName(PixelFormat format);

  static int GetDividerForTargetResolution(int src_width, int src_height, int dst_width, int dst_height);

  static const int kInternalChannelCount;

  static const rational kPixelAspectSquare;
  static const rational kPixelAspectNTSCStandard;
  static const rational kPixelAspectNTSCWidescreen;
  static const rational kPixelAspectPALStandard;
  static const rational kPixelAspectPALWidescreen;
  static const rational kPixelAspect1080Anamorphic;

  static const QVector<rational> kSupportedFrameRates;
  static const QVector<rational> kStandardPixelAspects;
  static const QVector<int> kSupportedDividers;

  static const int kHSVChannelCount = 3;
  static const int kRGBChannelCount = 3;
  static const int kRGBAChannelCount = 4;

  /**
   * @brief Convert rational frame rate (i.e. flipped timebase) to a user-friendly string
   */
  static QString FrameRateToString(const rational& frame_rate);

  static QStringList GetStandardPixelAspectRatioNames();
  static QString FormatPixelAspectRatioString(const QString& format, const rational& ratio);

  static int GetScaledDimension(int dim, int divider);

  [[nodiscard]] bool enabled() const { return enabled_; }

  void set_enabled(bool e) { enabled_ = e; }

  [[nodiscard]] float x() const { return x_; }
  void set_x(float x) { x_ = x; }
  [[nodiscard]] float y() const { return y_; }
  void set_y(float y) { y_ = y; }
  [[nodiscard]] QVector2D offset() const { return {x_, y_}; }

  [[nodiscard]] int stream_index() const { return stream_index_; }

  void set_stream_index(int s) { stream_index_ = s; }

  [[nodiscard]] Type video_type() const { return video_type_; }

  void set_video_type(Type t) { video_type_ = t; }

  [[nodiscard]] const rational& frame_rate() const { return frame_rate_; }

  void set_frame_rate(const rational& frame_rate) { frame_rate_ = frame_rate; }

  [[nodiscard]] int64_t start_time() const { return start_time_; }

  void set_start_time(int64_t start_time) { start_time_ = start_time; }

  [[nodiscard]] int64_t duration() const { return duration_; }

  void set_duration(int64_t duration) { duration_ = duration; }

  [[nodiscard]] bool premultiplied_alpha() const { return premultiplied_alpha_; }

  void set_premultiplied_alpha(bool premultiplied_alpha) { premultiplied_alpha_ = premultiplied_alpha; }

  [[nodiscard]] const QString& colorspace() const { return colorspace_; }

  void set_colorspace(const QString& c) { colorspace_ = c; }

  [[nodiscard]] const ColorRange& color_range() const { return color_range_; }
  void set_color_range(const ColorRange& color_range) { color_range_ = color_range; }

  [[nodiscard]] int64_t get_time_in_timebase_units(const rational& time) const;

  void Load(QXmlStreamReader* reader);

  void Save(QXmlStreamWriter* writer) const;

 private:
  void calculate_effective_size();

  void validate_pixel_aspect_ratio();

  void set_defaults_for_footage();

  void calculate_square_pixel_width();

  int width_;
  int height_;
  int depth_;
  rational time_base_;

  PixelFormat format_;

  int channel_count_;

  rational pixel_aspect_ratio_;

  Interlacing interlacing_;

  int divider_;

  // Cached values
  int effective_width_{};
  int effective_height_{};
  int effective_depth_{};
  int par_width_{};

  bool enabled_{};
  int stream_index_{};
  Type video_type_;
  rational frame_rate_;
  int64_t start_time_{};
  int64_t duration_{};
  bool premultiplied_alpha_{};
  QString colorspace_;
  float x_{};
  float y_{};
  ColorRange color_range_;
};

}  // namespace olive

Q_DECLARE_METATYPE(olive::VideoParams)
Q_DECLARE_METATYPE(olive::VideoParams::Interlacing)

#endif  // VIDEOPARAMS_H
