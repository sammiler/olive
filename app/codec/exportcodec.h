#ifndef EXPORTCODEC_H
#define EXPORTCODEC_H

#include <QObject>
#include <QString>

#include "common/define.h"       // 可能包含一些通用定义或类型
#include "render/subtitleparams.h" // 可能包含字幕相关的参数定义 (虽然在此文件中未直接使用其成员)

namespace olive {

/**
 * @brief 提供导出编解码器相关信息和功能的类。
 *
 *此类主要包含一个编解码器的枚举类型 `Codec`，以及用于获取编解码器名称、
 * 判断编解码器特性（如是否为静态图像、是否无损）的静态方法。
 * 注意：枚举值的顺序很重要，因为它们的索引可能用于序列化文件。
 */
class ExportCodec : public QObject {
  Q_OBJECT
 public:
  /**
   * @brief 支持的导出编解码器枚举。
   * @warning 只能在此列表末尾追加新的编解码器，切勿在中间插入，因为索引值被用于序列化文件。
   */
  enum Codec {
    kCodecDNxHD,            ///< @brief DNxHD 视频编解码器。
    kCodecH264,             ///< @brief H.264 (AVC) 视频编解码器。
    kCodecH264rgb,          ///< @brief H.264 RGB 视频编解码器 (通常用于无损或高质量场景)。
    kCodecH265,             ///< @brief H.265 (HEVC) 视频编解码器。
    kCodecOpenEXR,          ///< @brief OpenEXR 图像编解码器 (通常用于高动态范围图像序列)。
    kCodecPNG,              ///< @brief PNG 图像编解码器 (无损)。
    kCodecProRes,           ///< @brief Apple ProRes 视频编解码器。
    kCodecCineform,         ///< @brief GoPro CineForm 视频编解码器。
    kCodecTIFF,             ///< @brief TIFF 图像编解码器。
    kCodecVP9,              ///< @brief VP9 视频编解码器。
    kCodecMP2,              ///< @brief MPEG-1 Audio Layer II (MP2) 音频编解码器。
    kCodecMP3,              ///< @brief MPEG-1 Audio Layer III (MP3) 音频编解码器。
    kCodecAAC,              ///< @brief Advanced Audio Coding (AAC) 音频编解码器。
    kCodecPCM,              ///< @brief Pulse-Code Modulation (PCM) 音频编解码器 (无损原始音频)。
    kCodecOpus,             ///< @brief Opus 音频编解码器 (有损，但通常质量很高)。
    kCodecVorbis,           ///< @brief Vorbis 音频编解码器 (有损)。
    kCodecFLAC,             ///< @brief Free Lossless Audio Codec (FLAC) 音频编解码器 (无损)。
    kCodecSRT,              ///< @brief SubRip Text (SRT) 字幕格式。
    kCodecAV1,              ///< @brief AV1 视频编解码器。

    kCodecCount             ///< @brief 编解码器总数，用于迭代或数组大小。必须始终是最后一个枚举值。
  };

  /**
   * @brief 获取指定编解码器枚举值对应的可读名称。
   * @param c 要查询的编解码器枚举值。
   * @return QString 编解码器的名称字符串 (例如 "H.264", "PNG")；如果枚举值无效，则返回空字符串或错误提示。
   */
  static QString GetCodecName(Codec c);

  /**
   * @brief 判断指定的编解码器是否为静态图像格式。
   * @param c 要检查的编解码器枚举值。
   * @return bool 如果是静态图像编解码器 (如 PNG, TIFF, OpenEXR) 则返回 true，否则返回 false。
   */
  static bool IsCodecAStillImage(Codec c);

  /**
   * @brief 判断指定的编解码器是否为无损压缩格式。
   * @param c 要检查的编解码器枚举值。
   * @return bool 如果是无损编解码器 (如 PNG, FLAC, PCM, H264rgb的部分配置) 则返回 true，否则返回 false。
   */
  static bool IsCodecLossless(Codec c);
};

}  // namespace olive

#endif  // EXPORTCODEC_H