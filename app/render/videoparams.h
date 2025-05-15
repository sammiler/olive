#ifndef VIDEOPARAMS_H // 防止头文件被重复包含的宏
#define VIDEOPARAMS_H // 定义 VIDEOPARAMS_H 宏

#include <olive/core/core.h> // 包含 Olive 核心定义 (可能包含 PixelFormat, rational)
#include <QVector2D>         // Qt 二维向量类
#include <QXmlStreamReader>  // Qt XML 流读取器
#include <QXmlStreamWriter>  // Qt XML 流写入器

// 假设 PixelFormat 和 rational 类型在 <olive/core/core.h> 中定义。

namespace olive { // olive 项目的命名空间

using namespace core; // 使用 olive::core 命名空间中的类型

/**
 * @brief VideoParams 类封装了描述视频流或图像序列的各种参数。
 *
 * 这些参数包括但不限于：
 * - 尺寸 (宽度、高度、深度)
 * - 时间基准 (time_base_) 和帧率 (frame_rate_)
 * - 像素格式 (format_) 和通道数 (channel_count_)
 * - 像素宽高比 (pixel_aspect_ratio_)
 * - 隔行扫描方式 (interlacing_)
 * - 图像缩放因子 (divider_)，用于处理代理或低分辨率版本
 * - 视频类型 (视频、静止图像、图像序列)
 * - Alpha预乘状态、色彩空间、色彩范围等。
 *
 * 这个类还提供了一些便捷方法来计算有效尺寸、缓冲区大小，以及进行参数的序列化和反序列化。
 * VideoParams 是 Olive 中处理视频数据时传递和存储格式信息的标准方式。
 */
class VideoParams {
 public:
  // 定义隔行扫描方式的枚举
  enum Interlacing {
    kInterlaceNone,         // 无隔行扫描 (逐行扫描)
    kInterlacedTopFirst,    // 隔行扫描，顶场优先 (TFF)
    kInterlacedBottomFirst  // 隔行扫描，底场优先 (BFF)
  };

  // 定义视频内容类型的枚举
  enum Type {
    kVideoTypeVideo,        // 视频类型 (动态影像)
    kVideoTypeStill,        // 静止图像类型
    kVideoTypeImageSequence // 图像序列类型
  };

  // 定义色彩范围的枚举
  enum ColorRange {
    kColorRangeLimited,  // 有限范围 (例如视频中常见的 16-235 for 8-bit YUV)
    kColorRangeFull,     // 完全范围 (例如计算机图形中常见的 0-255 for 8-bit RGB)

    kColorRangeDefault = kColorRangeLimited // 默认色彩范围为有限范围
  };

  // 默认构造函数，会设置一些初始值
  VideoParams();
  /**
   * @brief 构造函数 (2D视频/图像)。
   * @param width, height 宽度和高度。
   * @param format 像素格式。
   * @param nb_channels 通道数量。
   * @param pixel_aspect_ratio (可选) 像素宽高比，默认为1 (方形像素)。
   * @param interlacing (可选) 隔行扫描方式，默认为无。
   * @param divider (可选) 图像缩放因子 (用于代理)，默认为1 (原始尺寸)。
   */
  VideoParams(int width, int height, PixelFormat format, int nb_channels,
              const rational& pixel_aspect_ratio = rational(1), Interlacing interlacing = kInterlaceNone,
              int divider = 1);
  /**
   * @brief 构造函数 (3D视频/图像，包含深度)。
   */
  VideoParams(int width, int height, int depth, PixelFormat format, int nb_channels,
              const rational& pixel_aspect_ratio = rational(1), Interlacing interlacing = kInterlaceNone,
              int divider = 1);
  /**
   * @brief 构造函数 (包含时间基准)。
   */
  VideoParams(int width, int height, const rational& time_base, PixelFormat format, int nb_channels,
              const rational& pixel_aspect_ratio = rational(1), Interlacing interlacing = kInterlaceNone,
              int divider = 1);

  /**
   * @brief 获取原始宽度。
   */
  [[nodiscard]] int width() const { return width_; }

  /**
   * @brief 设置原始宽度，并重新计算有效尺寸和方形像素宽度。
   */
  void set_width(int width) {
    width_ = width;
    calculate_effective_size(); // 重新计算有效尺寸
    calculate_square_pixel_width(); // 重新计算方形像素宽度
  }

  /**
   * @brief 返回考虑了像素宽高比的宽度 (用于在方形像素显示设备上正确显示)。
   * 例如，对于720x576的PAL视频 (PAR 16:15)，方形像素宽度可能是 720 * (16/15) = 768。
   */
  [[nodiscard]] int square_pixel_width() const { return par_width_; }

  /**
   * @brief 获取原始分辨率 (宽度, 高度) 作为 QVector2D。
   */
  [[nodiscard]] QVector2D resolution() const { return {static_cast<float>(width_), static_cast<float>(height_)}; }

  /**
   * @brief 获取考虑了像素宽高比的“方形像素”分辨率。
   */
  [[nodiscard]] QVector2D square_resolution() const {
    return {static_cast<float>(par_width_), static_cast<float>(height_)};
  }

  /**
   * @brief 获取原始高度。
   */
  [[nodiscard]] int height() const { return height_; }

  /**
   * @brief 设置原始高度，并重新计算有效尺寸。
   */
  void set_height(int height) {
    height_ = height;
    calculate_effective_size();
  }

  /**
   * @brief 获取深度 (用于3D图像或体积数据)。
   */
  [[nodiscard]] int depth() const { return depth_; }

  /**
   * @brief 设置深度，并重新计算有效尺寸。
   */
  void set_depth(int depth) {
    depth_ = depth;
    calculate_effective_size();
  }

  /**
   * @brief 检查是否为3D图像 (深度大于1)。
   */
  [[nodiscard]] bool is_3d() const { return depth_ > 1; }

  /**
   * @brief 获取时间基准 (例如，1/25 表示25fps视频的时间单位是1/25秒)。
   * 时间基准是帧持续时间的倒数。
   */
  [[nodiscard]] const rational& time_base() const { return time_base_; }

  /**
   * @brief 设置时间基准。
   */
  void set_time_base(const rational& r) { time_base_ = r; }

  /**
   * @brief 获取以时间基准形式表示的帧率 (即 time_base 的倒数)。
   * (注意：这里是 frame_rate_ 的倒数，如果 frame_rate_ 代表每秒帧数，则正确)
   */
  [[nodiscard]] rational frame_rate_as_time_base() const { return frame_rate_.flipped(); }

  /**
   * @brief 获取图像缩放因子 (divider)。
   * 用于表示当前参数是原始尺寸的 1/divider。例如，divider=2 表示半分辨率代理。
   */
  [[nodiscard]] int divider() const { return divider_; }

  /**
   * @brief 设置图像缩放因子，并重新计算有效尺寸。
   */
  void set_divider(int d) {
    divider_ = d;
    calculate_effective_size();
  }

  /**
   * @brief 获取有效宽度 (原始宽度 / divider)。
   */
  [[nodiscard]] int effective_width() const { return effective_width_; }

  /**
   * @brief 获取有效高度 (原始高度 / divider)。
   */
  [[nodiscard]] int effective_height() const { return effective_height_; }

  /**
   * @brief 获取有效深度 (原始深度 / divider)。
   */
  [[nodiscard]] int effective_depth() const { return effective_depth_; }

  /**
   * @brief 获取像素格式。
   */
  [[nodiscard]] PixelFormat format() const { return format_; }

  /**
   * @brief 设置像素格式。
   */
  void set_format(PixelFormat f) { format_ = f; }

  /**
   * @brief 获取通道数量。
   */
  [[nodiscard]] int channel_count() const { return channel_count_; }

  /**
   * @brief 设置通道数量。
   */
  void set_channel_count(int c) { channel_count_ = c; }

  /**
   * @brief 获取像素宽高比 (PAR)。
   */
  [[nodiscard]] const rational& pixel_aspect_ratio() const { return pixel_aspect_ratio_; }

  /**
   * @brief 设置像素宽高比，并进行验证和重新计算方形像素宽度。
   */
  void set_pixel_aspect_ratio(const rational& r) {
    pixel_aspect_ratio_ = r;
    validate_pixel_aspect_ratio(); // 验证PAR是否有效
    calculate_square_pixel_width(); // 重新计算
  }

  /**
   * @brief 获取隔行扫描方式。
   */
  [[nodiscard]] Interlacing interlacing() const { return interlacing_; }

  /**
   * @brief 设置隔行扫描方式。
   */
  void set_interlacing(Interlacing i) { interlacing_ = i; }

  /**
   * @brief (静态) 根据给定的宽度和高度自动生成一个合适的缩放因子 (divider)。
   * (具体逻辑需看实现，可能用于在生成代理时选择合适的缩小比例)
   */
  static int generate_auto_divider(qint64 width, qint64 height);

  /**
   * @brief 检查当前 VideoParams 是否有效 (例如，宽度、高度、格式等是否合理)。
   */
  [[nodiscard]] bool is_valid() const;

  // 重载等于 (==) 和不等于 (!=) 运算符，用于比较两个 VideoParams 对象
  bool operator==(const VideoParams& rhs) const;
  bool operator!=(const VideoParams& rhs) const;

  // (静态) 获取指定像素格式每个通道的字节数
  static int GetBytesPerChannel(PixelFormat format);
  // 获取当前像素格式每个通道的字节数
  [[nodiscard]] int GetBytesPerChannel() const { return GetBytesPerChannel(format_); }

  // (静态) 获取指定像素格式和通道数的每个像素的总字节数
  static int GetBytesPerPixel(PixelFormat format, int channels);
  // 获取当前参数下每个像素的总字节数
  [[nodiscard]] int GetBytesPerPixel() const { return GetBytesPerPixel(format_, channel_count_); }

  // (静态) 计算存储指定尺寸、格式和通道数的图像所需的缓冲区大小 (字节)
  static int GetBufferSize(int width, int height, PixelFormat format, int channels) {
    return width * height * GetBytesPerPixel(format, channels);
  }
  // 计算存储当前参数定义的图像所需的缓冲区大小
  [[nodiscard]] int GetBufferSize() const { return GetBufferSize(width_, height_, format_, channel_count_); }

  // (静态) 根据缩放因子获取其名称 (例如，divider=2 可能返回 "1/2")
  static QString GetNameForDivider(int div);

  // (静态) 检查指定的像素格式是否为浮点类型
  static bool FormatIsFloat(PixelFormat format) { return format.is_float(); }

  // (静态) 获取指定像素格式的名称字符串
  static QString GetFormatName(PixelFormat format);

  // (静态) 根据源尺寸和目标尺寸计算合适的缩放因子 (divider)
  static int GetDividerForTargetResolution(int src_width, int src_height, int dst_width, int dst_height);

  // 内部通道计数的常量 (具体含义需看上下文，可能是某种标准或默认值)
  static const int kInternalChannelCount;

  // --- 预定义的常用像素宽高比常量 ---
  static const rational kPixelAspectSquare;       // 方形像素 (1:1)
  static const rational kPixelAspectNTSCStandard;   // NTSC 标清 (例如 10:11 for 720x480 DV)
  static const rational kPixelAspectNTSCWidescreen; // NTSC 宽屏
  static const rational kPixelAspectPALStandard;    // PAL 标清 (例如 59:54 for 720x576 DV)
  static const rational kPixelAspectPALWidescreen;  // PAL 宽屏
  static const rational kPixelAspect1080Anamorphic; // 1080P 变形宽银幕 (例如 4:3 PAR for 1440x1080 -> 1920x1080)

  // 支持的帧率和像素宽高比列表 (用于UI下拉框或验证)
  static const QVector<rational> kSupportedFrameRates;
  static const QVector<rational> kStandardPixelAspects;
  // 支持的缩放因子列表
  static const QVector<int> kSupportedDividers;

  // 常用通道数常量
  static const int kHSVChannelCount = 3; // HSV 颜色模型通道数
  static const int kRGBChannelCount = 3; // RGB 颜色模型通道数
  static const int kRGBAChannelCount = 4; // RGBA 颜色模型通道数

  /**
   * @brief (静态) 将 rational 类型的帧率 (例如 25/1) 转换为用户友好的字符串 (例如 "25 fps")。
   */
  static QString FrameRateToString(const rational& frame_rate);

  // (静态) 获取标准像素宽高比的名称列表 (用于UI)
  static QStringList GetStandardPixelAspectRatioNames();
  // (静态) 根据格式名称和宽高比值格式化像素宽高比字符串
  static QString FormatPixelAspectRatioString(const QString& format, const rational& ratio);

  // (静态) 根据原始尺寸和缩放因子计算缩放后的尺寸
  static int GetScaledDimension(int dim, int divider);

  /**
   * @brief 检查此视频参数是否启用 (例如，视频轨道是否启用)。
   */
  [[nodiscard]] bool enabled() const { return enabled_; }

  /**
   * @brief 设置启用状态。
   */
  void set_enabled(bool e) { enabled_ = e; }

  // (可能用于图像在画布上的偏移)
  [[nodiscard]] float x() const { return x_; } // X轴偏移
  void set_x(float x) { x_ = x; }
  [[nodiscard]] float y() const { return y_; } // Y轴偏移
  void set_y(float y) { y_ = y; }
  [[nodiscard]] QVector2D offset() const { return {x_, y_}; } // 获取偏移向量

  /**
   * @brief 获取视频流的索引 (在包含多个视频流的媒体文件中)。
   */
  [[nodiscard]] int stream_index() const { return stream_index_; }

  /**
   * @brief 设置视频流的索引。
   */
  void set_stream_index(int s) { stream_index_ = s; }

  /**
   * @brief 获取视频内容的类型 (视频、静止图、图像序列)。
   */
  [[nodiscard]] Type video_type() const { return video_type_; }

  /**
   * @brief 设置视频内容的类型。
   */
  void set_video_type(Type t) { video_type_ = t; }

  /**
   * @brief 获取帧率 (以每秒帧数为单位的 rational 值，例如 25/1)。
   */
  [[nodiscard]] const rational& frame_rate() const { return frame_rate_; }

  /**
   * @brief 设置帧率。
   */
  void set_frame_rate(const rational& frame_rate) { frame_rate_ = frame_rate; }

  /**
   * @brief 获取视频的开始时间 (通常以时间基准为单位的整数，例如媒体文件的起始时间码)。
   */
  [[nodiscard]] int64_t start_time() const { return start_time_; }

  /**
   * @brief 设置视频的开始时间。
   */
  void set_start_time(int64_t start_time) { start_time_ = start_time; }

  /**
   * @brief 获取视频的总时长 (通常以时间基准为单位的整数)。
   */
  [[nodiscard]] int64_t duration() const { return duration_; }

  /**
   * @brief 设置视频的总时长。
   */
  void set_duration(int64_t duration) { duration_ = duration; }

  /**
   * @brief 检查Alpha通道是否为预乘Alpha。
   */
  [[nodiscard]] bool premultiplied_alpha() const { return premultiplied_alpha_; }

  /**
   * @brief 设置Alpha通道是否为预乘Alpha。
   */
  void set_premultiplied_alpha(bool premultiplied_alpha) { premultiplied_alpha_ = premultiplied_alpha; }

  /**
   * @brief 获取视频的色彩空间名称。
   */
  [[nodiscard]] const QString& colorspace() const { return colorspace_; }

  /**
   * @brief 设置视频的色彩空间名称。
   */
  void set_colorspace(const QString& c) { colorspace_ = c; }

  /**
   * @brief 获取视频的色彩范围。
   */
  [[nodiscard]] const ColorRange& color_range() const { return color_range_; }
  /**
   * @brief 设置视频的色彩范围。
   */
  void set_color_range(const ColorRange& color_range) { color_range_ = color_range; }

  /**
   * @brief 将一个 rational 类型的时间转换为以当前时间基准为单位的整数时间值。
   * 例如，将秒转换为帧号。
   */
  [[nodiscard]] int64_t get_time_in_timebase_units(const rational& time) const;

  // 从 XML 流加载 VideoParams 数据
  void Load(QXmlStreamReader* reader);
  // 将 VideoParams 数据保存到 XML 流
  void Save(QXmlStreamWriter* writer) const;

 private:
  // 私有辅助方法：根据 width_, height_, depth_, divider_ 计算有效尺寸
  void calculate_effective_size();

  // 私有辅助方法：验证像素宽高比是否有效
  void validate_pixel_aspect_ratio();

  // 私有辅助方法：为从素材文件加载的参数设置一些默认值
  void set_defaults_for_footage();

  // 私有辅助方法：根据 width_ 和 pixel_aspect_ratio_ 计算方形像素宽度 par_width_
  void calculate_square_pixel_width();

  // --- 成员变量 ---
  int width_;          // 原始宽度
  int height_;         // 原始高度
  int depth_;          // 深度 (通常为1，对于3D数据 >1)
  rational time_base_; // 时间基准 (帧持续时间的倒数)

  PixelFormat format_; // 像素格式

  int channel_count_; // 通道数量

  rational pixel_aspect_ratio_; // 像素宽高比

  Interlacing interlacing_; // 隔行扫描方式

  int divider_; // 图像缩放因子 (1 表示原始尺寸)

  // 缓存的计算值
  int effective_width_{};  // 有效宽度 (width_ / divider_)
  int effective_height_{}; // 有效高度 (height_ / divider_)
  int effective_depth_{};  // 有效深度 (depth_ / divider_)
  int par_width_{};        // 方形像素宽度 (width_ * pixel_aspect_ratio_)

  // 更多元数据
  bool enabled_{true}; // 是否启用 (默认为true)
  int stream_index_{0}; // 流索引 (默认为0)
  Type video_type_{kVideoTypeVideo}; // 视频类型 (默认为视频)
  rational frame_rate_; // 帧率 (例如 25/1 fps)
  int64_t start_time_{0}; // 开始时间 (以时间基准为单位)
  int64_t duration_{0};   // 总时长 (以时间基准为单位)
  bool premultiplied_alpha_{false}; // Alpha是否预乘 (默认为false)
  QString colorspace_;   // 色彩空间名称
  float x_{0.0f};         // X轴偏移 (默认为0)
  float y_{0.0f};         // Y轴偏移 (默认为0)
  ColorRange color_range_{kColorRangeDefault}; // 色彩范围 (默认为有限范围)
};

}  // namespace olive

// 声明 VideoParams 类型为元类型，以便在 QVariant 中使用或在信号槽中传递
Q_DECLARE_METATYPE(olive::VideoParams)
// 声明 VideoParams::Interlacing 枚举类型为元类型
Q_DECLARE_METATYPE(olive::VideoParams::Interlacing)

#endif  // VIDEOPARAMS_H