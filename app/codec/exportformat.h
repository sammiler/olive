#ifndef EXPORTFORMAT_H
#define EXPORTFORMAT_H

#include <QList>
#include <QString>
#include <QStringList>  // 为了 GetPixelFormatsForCodec
#include <vector>       // 为了 GetSampleFormatsForCodec

#include "common/define.h"  // 可能包含 SampleFormat 等定义
#include "exportcodec.h"    // 包含 ExportCodec::Codec 枚举

namespace olive {

/**
 * @brief 提供导出文件封装格式相关信息和功能的类。
 *
 * 此类主要包含一个文件封装格式的枚举类型 `Format`，以及用于获取格式名称、
 * 文件扩展名、特定格式下支持的视频/音频/字幕编解码器列表，
 * 以及特定格式和编解码器组合下支持的像素/采样格式的静态方法。
 * 注意：枚举值的顺序很重要，因为它们的索引可能用于序列化文件。
 */
class ExportFormat : public QObject {
  Q_OBJECT
 public:
  /**
   * @brief 支持的导出文件封装格式枚举。
   * @warning 只能在此列表末尾追加新的格式，切勿在中间插入，因为索引值被用于序列化文件。
   */
  enum Format {
    kFormatDNxHD,       ///< @brief DNxHD 封装格式 (通常是 MXF OP1a 或 QuickTime MOV，具体取决于实现)。
    kFormatMatroska,    ///< @brief Matroska (MKV) 封装格式。
    kFormatMPEG4Video,  ///< @brief MPEG-4 Part 14 (MP4) 视频封装格式。
    kFormatOpenEXR,     ///< @brief OpenEXR 图像序列格式 (每帧一个 .exr 文件)。
    kFormatQuickTime,   ///< @brief Apple QuickTime (MOV) 封装格式。
    kFormatPNG,         ///< @brief PNG 图像序列格式 (每帧一个 .png 文件)。
    kFormatTIFF,        ///< @brief TIFF 图像序列格式 (每帧一个 .tif 或 .tiff 文件)。
    kFormatWAV,         ///< @brief Waveform Audio File Format (WAV) 音频封装格式。
    kFormatAIFF,        ///< @brief Audio Interchange File Format (AIFF) 音频封装格式。
    kFormatMP3,         ///< @brief MP3 音频文件格式 (通常指原始MP3流或封装在ID3标签的MP3文件中)。
    kFormatFLAC,        ///< @brief Free Lossless Audio Codec (FLAC) 文件格式 (原始FLAC流或封装在FLAC文件中)。
    kFormatOgg,         ///< @brief Ogg 封装格式 (可包含Vorbis音频, Theora视频等)。
    kFormatWebM,        ///< @brief WebM 封装格式 (通常包含VP8/VP9视频和Vorbis/Opus音频)。
    kFormatSRT,         ///< @brief SubRip Text (SRT) 字幕文件格式。
    kFormatMPEG4Audio,  ///< @brief MPEG-4 Part 14 (MP4) 音频封装格式 (例如M4A)。

    kFormatCount  ///< @brief 封装格式总数，用于迭代或数组大小。必须始终是最后一个枚举值。
  };

  /**
   * @brief 获取指定封装格式枚举值对应的可读名称。
   * @param f 要查询的封装格式枚举值。
   * @return QString 封装格式的名称字符串 (例如 "Matroska", "QuickTime")；如果枚举值无效，则返回空字符串或错误提示。
   */
  static QString GetName(Format f);
  /**
   * @brief 获取指定封装格式的典型文件扩展名。
   * @param f 要查询的封装格式枚举值。
   * @return QString 文件扩展名 (例如 ".mkv", ".mov", ".png")，不包含点号前缀，除非特定格式需要。
   */
  static QString GetExtension(Format f);
  /**
   * @brief 获取指定封装格式通常支持的视频编解码器列表。
   * @param f 要查询的封装格式枚举值。
   * @return QList<ExportCodec::Codec> 支持的视频编解码器枚举值列表。
   */
  static QList<ExportCodec::Codec> GetVideoCodecs(ExportFormat::Format f);
  /**
   * @brief 获取指定封装格式通常支持的音频编解码器列表。
   * @param f 要查询的封装格式枚举值。
   * @return QList<ExportCodec::Codec> 支持的音频编解码器枚举值列表。
   */
  static QList<ExportCodec::Codec> GetAudioCodecs(ExportFormat::Format f);
  /**
   * @brief 获取指定封装格式通常支持的字幕编解码器（或字幕流类型）列表。
   * @param f 要查询的封装格式枚举值。
   * @return QList<ExportCodec::Codec> 支持的字幕编解码器枚举值列表。
   */
  static QList<ExportCodec::Codec> GetSubtitleCodecs(ExportFormat::Format f);

  /**
   * @brief 获取特定封装格式和视频编解码器组合下，推荐或支持的像素格式列表。
   * @param f 封装格式枚举值。
   * @param c 视频编解码器枚举值。
   * @return QStringList 支持的像素格式名称字符串列表。
   */
  static QStringList GetPixelFormatsForCodec(Format f, ExportCodec::Codec c);
  /**
   * @brief 获取特定封装格式和音频编解码器组合下，推荐或支持的音频采样格式列表。
   * @param f 封装格式枚举值。
   * @param c 音频编解码器枚举值。
   * @return std::vector<SampleFormat> 支持的音频采样格式枚举值向量。
   */
  static std::vector<SampleFormat> GetSampleFormatsForCodec(Format f, ExportCodec::Codec c);
};

}  // namespace olive

#endif  // EXPORTFORMAT_H