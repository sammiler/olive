#ifndef ENCODER_H
#define ENCODER_H

#include <QDir>        // 用于 GetPresetPath
#include <QHash>       // 用于 video_opts_
#include <QIODevice>   // 用于 Load/Save
#include <QMatrix4x4>  // 用于 GenerateMatrix
#include <QRegularExpression>
#include <QString>
#include <QXmlStreamReader>  // 用于加载参数 (在 .cpp 中使用，但对应 Load/Save 接口)
#include <QXmlStreamWriter>  // 用于保存参数
#include <memory>            // 为了 std::shared_ptr

#include "codec/exportcodec.h"   // 包含 ExportCodec::Codec
#include "codec/exportformat.h"  // 包含 ExportFormat::Format
#include "codec/frame.h"         // 包含 olive::FramePtr, olive::SampleBuffer (假设SampleBuffer也在此或其包含文件)
#include "common/define.h"       // 包含 olive::rational, olive::PixelFormat, olive::TimeRange 等
#include "node/block/subtitle/subtitle.h"  // 包含 SubtitleBlock
#include "render/colortransform.h"         // 包含 ColorTransform
#include "render/subtitleparams.h"         // (可能包含 SubtitleParams，但未使用)
#include "render/videoparams.h"            // 包含 VideoParams, AudioParams (假设AudioParams在此或其包含文件)

namespace olive {

class Encoder;
/**
 * @brief 指向 Encoder 对象的共享指针类型定义。
 */
using EncoderPtr = std::shared_ptr<Encoder>;

/**
 * @brief 封装所有导出编码所需参数的类。
 *
 * 此类用于配置视频、音频和字幕的编码选项，包括格式、编解码器、
 * 码率、分辨率、文件名等。它还支持从XML文件加载和保存预设。
 */
class EncodingParams {
 public:
  /**
   * @brief 视频缩放（当源尺寸和目标尺寸不一致时）的方法枚举。
   */
  enum VideoScalingMethod {
    kFit,      ///< @brief 适应模式 (Letterbox/Pillarbox)：保持原始宽高比，将图像完整放入目标尺寸，多余部分填充黑边。
    kStretch,  ///< @brief 拉伸模式：忽略原始宽高比，将图像拉伸以完全填充目标尺寸。
    kCrop  ///< @brief 裁剪模式 (Pan and Scan)：保持原始宽高比，缩放图像以填满目标尺寸的一边，并裁剪另一边超出的部分。
  };

  /**
   * @brief 默认构造函数。
   * 初始化编码参数为一组合理的默认值。
   */
  EncodingParams();

  /**
   * @brief 获取存储编码预设文件的用户特定路径。
   * @return QDir 预设文件所在的目录。
   */
  static QDir GetPresetPath();
  /**
   * @brief 获取所有可用的编码预设文件名列表。
   * @return QStringList 预设文件名的列表。
   */
  static QStringList GetListOfPresets();

  /**
   * @brief 检查当前编码参数配置是否有效。
   * 有效的条件是至少启用了一种流（视频、音频或字幕）进行编码。
   * @return bool 如果参数配置有效则返回 true，否则返回 false。
   */
  [[nodiscard]] bool IsValid() const { return video_enabled_ || audio_enabled_ || subtitles_enabled_; }

  /**
   * @brief 设置输出文件名（包含路径）。
   * @param filename 要设置的完整文件名。
   */
  void SetFilename(const QString& filename) { filename_ = filename; }

  /**
   * @brief 启用视频编码并设置相关参数。
   * @param video_params 详细的视频参数，如分辨率、帧率等。
   * @param vcodec 要使用的视频编解码器。
   */
  void EnableVideo(const VideoParams& video_params, const ExportCodec::Codec& vcodec);
  /**
   * @brief 启用音频编码并设置相关参数。
   * @param audio_params 详细的音频参数，如采样率、通道布局等。
   * @param acodec 要使用的音频编解码器。
   */
  void EnableAudio(const AudioParams& audio_params, const ExportCodec::Codec& acodec);
  /**
   * @brief 启用字幕编码（通常是嵌入式字幕）。
   * @param scodec 要使用的字幕编解码器。
   */
  void EnableSubtitles(const ExportCodec::Codec& scodec);
  /**
   * @brief 启用边车 (sidecar) 字幕文件输出。
   * 边车字幕是指字幕作为单独的文件与媒体文件一起输出。
   * @param sfmt 边车字幕文件的格式。
   * @param scodec 边车字幕使用的编解码器 (或格式特定标识)。
   */
  void EnableSidecarSubtitles(const ExportFormat::Format& sfmt, const ExportCodec::Codec& scodec);

  /**
   * @brief 禁用视频编码。
   */
  void DisableVideo();
  /**
   * @brief 禁用音频编码。
   */
  void DisableAudio();
  /**
   * @brief 禁用字幕编码（包括嵌入式和边车）。
   */
  void DisableSubtitles();

  /**
   * @brief 获取输出文件的封装格式。
   * @return const ExportFormat::Format& 对当前封装格式的常量引用。
   */
  [[nodiscard]] const ExportFormat::Format& format() const { return format_; }
  /**
   * @brief 设置输出文件的封装格式。
   * @param format 要设置的封装格式。
   */
  void set_format(const ExportFormat::Format& format) { format_ = format; }

  /**
   * @brief 设置特定的视频编码器选项（键值对）。
   * @param key 选项的键名。
   * @param value 选项的值。
   */
  void set_video_option(const QString& key, const QString& value) { video_opts_.insert(key, value); }
  /**
   * @brief 设置视频目标码率 (比特率)。
   * @param rate 视频码率，单位通常是 bps (比特每秒)。
   */
  void set_video_bit_rate(const int64_t& rate) { video_bit_rate_ = rate; }
  /**
   * @brief 设置视频最小码率。
   * @param rate 视频最小码率，单位通常是 bps。
   */
  void set_video_min_bit_rate(const int64_t& rate) { video_min_bit_rate_ = rate; }
  /**
   * @brief 设置视频最大码率。
   * @param rate 视频最大码率，单位通常是 bps。
   */
  void set_video_max_bit_rate(const int64_t& rate) { video_max_bit_rate_ = rate; }
  /**
   * @brief 设置视频编码器缓冲区大小。
   * @param sz 缓冲区大小，单位通常是字节或与码率相关的单位。
   */
  void set_video_buffer_size(const int64_t& sz) { video_buffer_size_ = sz; }
  /**
   * @brief 设置视频编码使用的线程数。
   * @param threads 线程数量。
   */
  void set_video_threads(const int& threads) { video_threads_ = threads; }
  /**
   * @brief 设置视频编码时使用的像素格式名称 (例如 "yuv420p")。
   * @param s 像素格式的字符串表示。
   */
  void set_video_pix_fmt(const QString& s) { video_pix_fmt_ = s; }
  /**
   * @brief 设置视频输出是否为图像序列。
   * @param s 如果是图像序列则为 true，否则为 false。
   */
  void set_video_is_image_sequence(bool s) { video_is_image_sequence_ = s; }
  /**
   * @brief 设置视频编码时应用的颜色转换。
   * @param color_transform 颜色转换对象。
   */
  void set_color_transform(const ColorTransform& color_transform) { color_transform_ = color_transform; }

  /**
   * @brief 获取输出文件名。
   * @return const QString& 对输出文件名的常量引用。
   */
  [[nodiscard]] const QString& filename() const { return filename_; }

  /**
   * @brief 检查视频编码是否已启用。
   * @return bool 如果视频编码启用则返回 true，否则返回 false。
   */
  [[nodiscard]] bool video_enabled() const { return video_enabled_; }
  /**
   * @brief 获取视频编解码器。
   * @return const ExportCodec::Codec& 对视频编解码器的常量引用。
   */
  [[nodiscard]] const ExportCodec::Codec& video_codec() const { return video_codec_; }
  /**
   * @brief 获取视频参数。
   * @return const VideoParams& 对视频参数的常量引用。
   */
  [[nodiscard]] const VideoParams& video_params() const { return video_params_; }
  /**
   * @brief 获取所有自定义视频编码器选项。
   * @return const QHash<QString, QString>& 对视频选项哈希表的常量引用。
   */
  [[nodiscard]] const QHash<QString, QString>& video_opts() const { return video_opts_; }
  /**
   * @brief 获取指定键的视频编码器选项值。
   * @param key 选项的键名。
   * @return QString 选项的值，如果键不存在则返回空字符串。
   */
  [[nodiscard]] QString video_option(const QString& key) const { return video_opts_.value(key); }
  /**
   * @brief 检查是否存在指定的视频编码器选项。
   * @param key 选项的键名。
   * @return bool 如果选项存在则返回 true，否则返回 false。
   */
  [[nodiscard]] bool has_video_opt(const QString& key) const { return video_opts_.contains(key); }
  /**
   * @brief 获取视频目标码率。
   * @return const int64_t& 视频码率。
   */
  [[nodiscard]] const int64_t& video_bit_rate() const { return video_bit_rate_; }
  /**
   * @brief 获取视频最小码率。
   * @return const int64_t& 视频最小码率。
   */
  [[nodiscard]] const int64_t& video_min_bit_rate() const { return video_min_bit_rate_; }
  /**
   * @brief 获取视频最大码率。
   * @return const int64_t& 视频最大码率。
   */
  [[nodiscard]] const int64_t& video_max_bit_rate() const { return video_max_bit_rate_; }
  /**
   * @brief 获取视频编码器缓冲区大小。
   * @return const int64_t& 视频缓冲区大小。
   */
  [[nodiscard]] const int64_t& video_buffer_size() const { return video_buffer_size_; }
  /**
   * @brief 获取视频编码使用的线程数。
   * @return const int& 线程数。
   */
  [[nodiscard]] const int& video_threads() const { return video_threads_; }
  /**
   * @brief 获取视频像素格式的字符串表示。
   * @return const QString& 像素格式字符串。
   */
  [[nodiscard]] const QString& video_pix_fmt() const { return video_pix_fmt_; }
  /**
   * @brief 检查视频输出是否为图像序列。
   * @return bool 如果是图像序列则返回 true。
   */
  [[nodiscard]] bool video_is_image_sequence() const { return video_is_image_sequence_; }
  /**
   * @brief 获取应用的颜色转换。
   * @return const ColorTransform& 对颜色转换对象的常量引用。
   */
  [[nodiscard]] const ColorTransform& color_transform() const { return color_transform_; }

  /**
   * @brief 检查音频编码是否已启用。
   * @return bool 如果音频编码启用则返回 true，否则返回 false。
   */
  [[nodiscard]] bool audio_enabled() const { return audio_enabled_; }
  /**
   * @brief 获取音频编解码器。
   * @return const ExportCodec::Codec& 对音频编解码器的常量引用。
   */
  [[nodiscard]] const ExportCodec::Codec& audio_codec() const { return audio_codec_; }
  /**
   * @brief 获取音频参数。
   * @return const AudioParams& 对音频参数的常量引用。
   */
  [[nodiscard]] const AudioParams& audio_params() const { return audio_params_; }
  /**
   * @brief 获取音频目标码率。
   * @return const int64_t& 音频码率。
   */
  [[nodiscard]] const int64_t& audio_bit_rate() const { return audio_bit_rate_; }

  /**
   * @brief 设置音频目标码率。
   * @param b 音频码率，单位通常是 bps。
   */
  void set_audio_bit_rate(const int64_t& b) { audio_bit_rate_ = b; }

  /**
   * @brief 检查字幕编码是否已启用。
   * @return bool 如果字幕编码启用则返回 true，否则返回 false。
   */
  [[nodiscard]] bool subtitles_enabled() const { return subtitles_enabled_; }
  /**
   * @brief 检查字幕是否配置为边车文件输出。
   * @return bool 如果是边车字幕则返回 true，否则（嵌入式）返回 false。
   */
  [[nodiscard]] bool subtitles_are_sidecar() const { return subtitles_are_sidecar_; }
  /**
   * @brief 获取边车字幕文件的封装格式。
   * @return ExportFormat::Format 边车字幕格式。
   */
  [[nodiscard]] ExportFormat::Format subtitle_sidecar_fmt() const { return subtitle_sidecar_fmt_; }
  /**
   * @brief 获取字幕编解码器（或格式标识）。
   * @return ExportCodec::Codec 字幕编解码器。
   */
  [[nodiscard]] ExportCodec::Codec subtitles_codec() const { return subtitles_codec_; }

  /**
   * @brief 获取导出范围的总长度。
   * @return const rational& 导出长度。
   */
  [[nodiscard]] const rational& GetExportLength() const { return export_length_; }
  /**
   * @brief 设置导出范围的总长度。
   * @param export_length 要设置的导出长度。
   */
  void SetExportLength(const rational& export_length) { export_length_ = export_length; }

  /**
   * @brief 从 QIODevice 设备加载编码参数（通常是XML格式）。
   * @param device 指向包含参数数据的 QIODevice 设备的指针。
   * @return bool 如果加载成功则返回 true，否则返回 false。
   */
  bool Load(QIODevice* device);
  /**
   * @brief 从 QXmlStreamReader 加载编码参数。
   * @param reader 指向 QXmlStreamReader 的指针，用于读取XML数据。
   * @return bool 如果加载成功则返回 true，否则返回 false。
   */
  bool Load(QXmlStreamReader* reader);

  /**
   * @brief 将当前编码参数保存到 QIODevice 设备（通常是XML格式）。
   * @param device 指向用于写入参数数据的 QIODevice 设备的指针。
   */
  void Save(QIODevice* device) const;
  /**
   * @brief 将当前编码参数使用 QXmlStreamWriter 保存。
   * @param writer 指向 QXmlStreamWriter 的指针，用于写入XML数据。
   */
  void Save(QXmlStreamWriter* writer) const;

  /**
   * @brief 检查是否设置了自定义导出时间范围。
   * @return bool 如果设置了自定义范围则返回 true，否则返回 false。
   */
  [[nodiscard]] bool has_custom_range() const { return has_custom_range_; }
  /**
   * @brief 获取自定义导出时间范围。
   * 只有在 has_custom_range() 返回 true 时此值才有意义。
   * @return const TimeRange& 对自定义时间范围的常量引用。
   */
  [[nodiscard]] const TimeRange& custom_range() const { return custom_range_; }
  /**
   * @brief 设置自定义导出时间范围。
   * @param custom_range 要设置的自定义时间范围。
   */
  void set_custom_range(const TimeRange& custom_range) {
    has_custom_range_ = true;
    custom_range_ = custom_range;
  }

  /**
   * @brief 获取视频缩放方法。
   * @return const VideoScalingMethod& 对当前视频缩放方法的常量引用。
   */
  [[nodiscard]] const VideoScalingMethod& video_scaling_method() const { return video_scaling_method_; }
  /**
   * @brief 设置视频缩放方法。
   * @param video_scaling_method 要设置的视频缩放方法。
   */
  void set_video_scaling_method(const VideoScalingMethod& video_scaling_method) {
    video_scaling_method_ = video_scaling_method;
  }

  /**
   * @brief 根据指定的缩放方法和源/目标尺寸生成一个4x4变换矩阵。
   * 此矩阵可用于例如OpenGL渲染时的顶点变换。
   * @param method 使用的视频缩放方法。
   * @param source_width 源图像的宽度。
   * @param source_height 源图像的高度。
   * @param dest_width 目标区域的宽度。
   * @param dest_height 目标区域的高度。
   * @return QMatrix4x4 计算得到的变换矩阵。
   */
  static QMatrix4x4 GenerateMatrix(VideoScalingMethod method, int source_width, int source_height, int dest_width,
                                   int dest_height);

 private:
  /**
   * @brief 编码参数序列化格式的版本号。
   */
  static const int kEncoderParamsVersion = 1;

  /**
   * @brief 加载版本为1的编码参数。
   * @param reader 指向 QXmlStreamReader 的指针。
   * @return bool 如果加载成功则返回 true。
   */
  bool LoadV1(QXmlStreamReader* reader);

  /**
   * @brief 输出文件名及路径。
   */
  QString filename_;
  /**
   * @brief 输出文件的封装格式。
   */
  ExportFormat::Format format_;

  /**
   * @brief 是否启用视频编码。
   */
  bool video_enabled_;
  /**
   * @brief 视频编码器类型。
   */
  ExportCodec::Codec video_codec_;
  /**
   * @brief 视频参数 (分辨率、帧率等)。
   */
  VideoParams video_params_;
  /**
   * @brief 视频编码器特定选项。
   */
  QHash<QString, QString> video_opts_;
  /**
   * @brief 视频目标码率。
   */
  int64_t video_bit_rate_;
  /**
   * @brief 视频最小码率。
   */
  int64_t video_min_bit_rate_;
  /**
   * @brief 视频最大码率。
   */
  int64_t video_max_bit_rate_;
  /**
   * @brief 视频编码缓冲区大小。
   */
  int64_t video_buffer_size_;
  /**
   * @brief 视频编码线程数。
   */
  int video_threads_;
  /**
   * @brief 视频目标像素格式 (字符串表示)。
   */
  QString video_pix_fmt_;
  /**
   * @brief 视频是否作为图像序列输出。
   */
  bool video_is_image_sequence_;
  /**
   * @brief 应用于视频的颜色转换。
   */
  ColorTransform color_transform_;

  /**
   * @brief 是否启用音频编码。
   */
  bool audio_enabled_;
  /**
   * @brief 音频编码器类型。
   */
  ExportCodec::Codec audio_codec_;
  /**
   * @brief 音频参数 (采样率、通道等)。
   */
  AudioParams audio_params_;
  /**
   * @brief 音频目标码率。
   */
  int64_t audio_bit_rate_;

  /**
   * @brief 是否启用字幕编码。
   */
  bool subtitles_enabled_;
  /**
   * @brief 字幕是否作为边车文件输出。
   */
  bool subtitles_are_sidecar_;
  /**
   * @brief 边车字幕文件的封装格式。
   */
  ExportFormat::Format subtitle_sidecar_fmt_;
  /**
   * @brief 字幕编码器类型。
   */
  ExportCodec::Codec subtitles_codec_;

  /**
   * @brief 导出的总长度。
   */
  rational export_length_;
  /**
   * @brief 视频缩放方法。
   */
  VideoScalingMethod video_scaling_method_;

  /**
   * @brief 是否使用自定义导出范围。
   */
  bool has_custom_range_;
  /**
   * @brief 自定义导出时间范围。
   */
  TimeRange custom_range_;
};

/**
 * @brief 编码器基类，定义了所有具体编码器实现的通用接口。
 *
 * Encoder 类是视频/音频/字幕编码功能的抽象。派生类将实现
 * 特定编解码库（如FFmpeg、OIIO）的编码逻辑。
 * 它负责接收帧数据并将其写入到输出文件。
 */
class Encoder : public QObject {
  Q_OBJECT
 public:
  /**
   * @brief 使用指定的编码参数构造一个 Encoder 对象。
   * @param params 编码所需的参数。
   */
  explicit Encoder(EncodingParams params);

  // 默认析构函数通常是虚函数，以允许派生类正确清理资源
  virtual ~Encoder() = default;

  /**
   * @brief 编码器类型的枚举。
   */
  enum Type {
    kEncoderTypeNone = -1,  ///< @brief 未指定或无效的编码器类型。
    kEncoderTypeFFmpeg,     ///< @brief 基于 FFmpeg 的编码器。
    kEncoderTypeOIIO        ///< @brief 基于 OpenImageIO 的编码器 (通常用于图像序列)。
  };

  /**
   * @brief 根据编码器类型ID和编码参数创建 Encoder 实例。
   * @param id 要创建的编码器类型。
   * @param params 编码所需的参数。
   * @return Encoder* 指向创建的 Encoder 实例的指针；如果ID无效或创建失败，则返回 nullptr。
   */
  static Encoder* CreateFromID(Type id, const EncodingParams& params);

  /**
   * @brief 根据输出文件格式获取推荐的编码器类型。
   * @param f 输出文件的封装格式。
   * @return Type 推荐的编码器类型。
   */
  static Type GetTypeFromFormat(ExportFormat::Format f);

  /**
   * @brief 根据输出文件格式和编码参数创建 Encoder 实例。
   * @param f 输出文件的封装格式。
   * @param params 编码所需的参数。
   * @return Encoder* 指向创建的 Encoder 实例的指针；如果格式不支持或创建失败，则返回 nullptr。
   */
  static Encoder* CreateFromFormat(ExportFormat::Format f, const EncodingParams& params);

  /**
   * @brief 根据编码参数自动选择并创建最合适的 Encoder 实例。
   * @param params 编码所需的参数。
   * @return Encoder* 指向创建的 Encoder 实例的指针；如果无法确定合适的编码器或创建失败，则返回 nullptr。
   */
  static Encoder* CreateFromParams(const EncodingParams& params);

  /**
   * @brief 获取指定编解码器支持的像素格式列表。
   * 子类应覆盖此方法以提供特定于其后端库支持的格式。
   * @param c 要查询的导出视频编解码器。
   * @return QStringList 支持的像素格式名称列表。默认实现返回空列表。
   */
  [[nodiscard]] virtual QStringList GetPixelFormatsForCodec(ExportCodec::Codec c) const;
  /**
   * @brief 获取指定编解码器支持的音频采样格式列表。
   * 子类应覆盖此方法以提供特定于其后端库支持的格式。
   * @param c 要查询的导出音频编解码器。
   * @return std::vector<SampleFormat> 支持的音频采样格式列表。默认实现返回空向量。
   */
  [[nodiscard]] virtual std::vector<SampleFormat> GetSampleFormatsForCodec(ExportCodec::Codec c) const;

  /**
   * @brief 获取当前编码器使用的编码参数。
   * @return const EncodingParams& 对编码参数对象的常量引用。
   */
  [[nodiscard]] const EncodingParams& params() const;

  /**
   * @brief 获取编码器期望的输入像素格式。
   * 编码器可能需要特定的输入像素格式才能高效工作，或者在内部进行转换。
   * @return PixelFormat 期望的像素格式。默认返回 PixelFormat::INVALID。
   */
  [[nodiscard]] virtual PixelFormat GetDesiredPixelFormat() const { return PixelFormat(PixelFormat::INVALID); }

  /**
   * @brief 获取最近一次操作发生的错误信息。
   * @return const QString& 错误描述字符串。如果没有错误，则为空字符串。
   */
  [[nodiscard]] const QString& GetError() const { return error_; }

  /**
   * @brief 为图像序列的特定帧号生成完整的文件名。
   * 使用 params_ 中的文件名作为模板。
   * @param frame 帧的编号或时间戳（将转换为帧号）。
   * @return QString 生成的完整文件名。
   */
  QString GetFilenameForFrame(const rational& frame);

  /**
   * @brief 获取图像序列文件名中数字占位符的位数。
   * 例如 "frame_####.png" 中的 '#' 位数为 4。
   * @param filename 包含占位符的文件名模板。
   * @return int 数字占位符的位数；如果未找到，则返回0或一个默认值。
   */
  static int GetImageSequencePlaceholderDigitCount(const QString& filename);

  /**
   * @brief 检查文件名是否包含图像序列的数字占位符 (如 '#' 或 '%0Nd')。
   * @param filename 要检查的文件名。
   * @return bool 如果包含占位符则返回 true，否则返回 false。
   */
  static bool FilenameContainsDigitPlaceholder(const QString& filename);
  /**
   * @brief 从文件名中移除图像序列的数字占位符。
   * @param filename 原始文件名。
   * @return QString 移除了占位符的文件名。
   */
  static QString FilenameRemoveDigitPlaceholder(QString filename);

  /**
   * @brief 用于匹配文件名中是否包含图像序列数字占位符 (如 %04d 或 ####) 的正则表达式。
   */
  static const QRegularExpression kImageSequenceContainsDigits;
  /**
   * @brief 用于从文件名中移除图像序列数字占位符的正则表达式。
   */
  static const QRegularExpression kImageSequenceRemoveDigits;

 public slots:
  /**
   * @brief 打开编码器，准备开始写入数据。
   * 纯虚函数，必须由派生类实现。
   * @return bool 如果成功打开则返回 true，否则返回 false。
   */
  virtual bool Open() = 0;

  /**
   * @brief 将视频帧写入编码器。
   * 纯虚函数，必须由派生类实现。
   * @param frame 指向视频帧数据的共享指针。
   * @param time 帧的时间戳。
   * @return bool 如果写入成功则返回 true，否则返回 false。
   */
  virtual bool WriteFrame(olive::FramePtr frame, olive::core::rational time) = 0;
  /**
   * @brief 将音频样本缓冲区写入编码器。
   * 纯虚函数，必须由派生类实现。
   * @param audio 包含音频样本数据的 SampleBuffer 对象。
   * @return bool 如果写入成功则返回 true，否则返回 false。
   */
  virtual bool WriteAudio(const olive::SampleBuffer& audio) = 0;
  /**
   * @brief 将字幕块写入编码器。
   * 纯虚函数，必须由派生类实现。
   * @param sub_block 指向字幕块数据的指针。
   * @return bool 如果写入成功则返回 true，否则返回 false。
   */
  virtual bool WriteSubtitle(const SubtitleBlock* sub_block) = 0;

  /**
   * @brief 关闭编码器，刷新所有挂起的数据并释放资源。
   * 纯虚函数，必须由派生类实现。
   */
  virtual void Close() = 0;

 protected:
  /**
   * @brief 设置当前错误信息。
   * 派生类在发生错误时应调用此方法。
   * @param err 错误描述字符串。
   */
  void SetError(const QString& err) { error_ = err; }

 private:
  /**
   * @brief 存储当前编码器使用的编码参数。
   */
  EncodingParams params_;

  /**
   * @brief 存储最近一次操作发生的错误信息。
   */
  QString error_;
};

}  // namespace olive

#endif  // ENCODER_H