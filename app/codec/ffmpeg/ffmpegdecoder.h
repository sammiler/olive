#ifndef FFMPEGDECODER_H
#define FFMPEGDECODER_H

// 修复包含 <avfilter.h> 时出现的奇怪宏定义问题
#include <inttypes.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

#include <QTimer>
#include <QVector>
#include <QWaitCondition>  // 虽然包含但在此头文件中未直接使用
#include <list>            // 为了 std::list

#include "codec/decoder.h"
#include "common/ffmpegutils.h"  // 包含 FFmpeg 相关的工具函数和类型定义，如 AVFramePtr

namespace olive {

/**
 * @brief 基于 FFmpeg 的解码器实现。
 *
 * 此类继承自 Decoder，并封装了 FFmpeg 的解码功能，
 * 使其能够作为 Olive 项目中的一个标准解码器使用。
 * 它支持视频和音频的解码，并处理与 FFmpeg 库的底层交互。
 */
class FFmpegDecoder : public Decoder {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 FFmpegDecoder 对象。
   */
  FFmpegDecoder();

  /**
   * @brief FFmpegDecoder 的默认析构函数。
   * 使用 DECODER_DEFAULT_DESTRUCTOR 宏来定义。
   */
  DECODER_DEFAULT_DESTRUCTOR(FFmpegDecoder)

  /**
   * @brief 获取解码器的唯一标识符。
   * @return QString 解码器的ID，对于 FFmpegDecoder 通常是 "FFmpegDecoder"。
   */
  [[nodiscard]] QString id() const override;

  /**
   * @brief 检查此解码器是否支持视频解码。
   * @return bool 总是返回 true，因为 FFmpegDecoder 支持视频。
   */
  bool SupportsVideo() override { return true; }
  /**
   * @brief 检查此解码器是否支持音频解码。
   * @return bool 总是返回 true，因为 FFmpegDecoder 支持音频。
   */
  bool SupportsAudio() override { return true; }

  /**
   * @brief 探测指定文件的素材信息。
   * @param filename 要探测的文件名。
   * @param cancelled 指向 CancelAtom 的指针，用于在操作过程中检查是否已请求取消。
   * @return FootageDescription 包含探测到的素材信息的对象。
   */
  FootageDescription Probe(const QString& filename, CancelAtom* cancelled) const override;

 protected:
  /**
   * @brief 内部打开解码器的实现。
   * @return bool 如果成功打开则返回 true，否则返回 false。
   */
  bool OpenInternal() override;

  /**
   * @brief 内部检索视频帧的实现。
   * @param p 检索视频帧所需的参数。
   * @return TexturePtr 指向解码后视频帧纹理的指针。
   */
  TexturePtr RetrieveVideoInternal(const RetrieveVideoParams& p) override;

  /**
   * @brief 内部处理音频适配（转码/重采样）的实现。
   * @param filenames （此参数在 FFmpeg 解码器中可能未使用，通常单个文件包含所有流）。
   * @param params 目标音频参数。
   * @param cancelled 指向 CancelAtom 的指针，用于检查是否已请求取消。
   * @return bool 如果成功适配音频则返回 true，否则返回 false。
   */
  bool ConformAudioInternal(const QVector<QString>& filenames, const AudioParams& params,
                            CancelAtom* cancelled) override;
  /**
   * @brief 内部关闭解码器的实现。
   */
  void CloseInternal() override;

  /**
   * @brief 获取音频流的起始偏移量。
   * @return rational 音频流的起始偏移。
   */
  [[nodiscard]] rational GetAudioStartOffset() const override;

 private:
  /**
   * @brief 内部类，封装了单个 FFmpeg 流实例的打开、关闭、读取和定位操作。
   */
  class Instance {
   public:
    /**
     * @brief 构造一个 Instance 对象。
     */
    Instance();

    /**
     * @brief 析构 Instance 对象，确保调用 Close()。
     */
    ~Instance() { Close(); }

    /**
     * @brief 打开指定的媒体文件和流索引。
     * @param filename 要打开的媒体文件名。
     * @param stream_index 要打开的流的索引。
     * @return bool 如果成功打开则返回 true，否则返回 false。
     */
    bool Open(const char* filename, int stream_index);

    /**
     * @brief 检查当前实例是否已成功打开。
     * @return bool 如果 AVFormatContext 已初始化则返回 true，否则返回 false。
     */
    [[nodiscard]] bool IsOpen() const { return fmt_ctx_; }

    /**
     * @brief 关闭当前打开的媒体文件和流，释放相关资源。
     */
    void Close();

    /**
     * @brief 使用 FFmpeg API 检索一个数据包 (存储在 pkt) 并解码它 (存储在 frame)。
     * @param pkt 指向 AVPacket 的指针，用于存储读取到的数据包。
     * @param frame 指向 AVFrame 的指针，用于存储解码后的帧。
     * @return int FFmpeg 错误码，成功时返回 >= 0 的值。
     */
    int GetFrame(AVPacket* pkt, AVFrame* frame);

    /**
     * @brief 获取字幕流的头部信息。
     * @return const char* 指向字幕头部信息的C字符串指针，如果不存在则为 nullptr。
     */
    [[nodiscard]] const char* GetSubtitleHeader() const;

    /**
     * @brief 获取一个字幕包。
     * @param pkt 指向 AVPacket 的指针，用于存储读取到的数据包。
     * @param sub 指向 AVSubtitle 的指针，用于存储解码后的字幕数据。
     * @return int FFmpeg 错误码，成功时返回 >= 0 的值。
     */
    int GetSubtitle(AVPacket* pkt, AVSubtitle* sub);

    /**
     * @brief 从流中读取下一个数据包。
     * @param pkt 指向 AVPacket 的指针，用于存储读取到的数据包。
     * @return int FFmpeg 错误码，成功时返回 >= 0 的值。
     */
    int GetPacket(AVPacket* pkt);

    /**
     * @brief 定位到指定的时间戳。
     * @param timestamp 要定位到的时间戳（以 AVStream 的 time_base 为单位）。
     */
    void Seek(int64_t timestamp);

    /**
     * @brief 获取 AVFormatContext。
     * @return AVFormatContext* 指向当前实例的 AVFormatContext 的指针。
     */
    [[nodiscard]] AVFormatContext* fmt_ctx() const { return fmt_ctx_; }

    /**
     * @brief 获取 AVStream。
     * @return AVStream* 指向当前实例关联的 AVStream 的指针。
     */
    [[nodiscard]] AVStream* avstream() const { return avstream_; }

   private:
    /**
     * @brief FFmpeg 格式上下文。
     */
    AVFormatContext* fmt_ctx_{nullptr};
    /**
     * @brief FFmpeg 编解码器上下文。
     */
    AVCodecContext* codec_ctx_{nullptr};
    /**
     * @brief FFmpeg 流对象。
     */
    AVStream* avstream_{nullptr};
    /**
     * @brief FFmpeg 字典选项，用于打开编解码器等。
     */
    AVDictionary* opts_{nullptr};
  };

  /**
   * @brief 处理 FFmpeg 错误码。
   *
   * 使用 FFmpeg API 获取此错误码的描述性字符串，并将其发送到 Error()。
   * 因此，此函数也会自动关闭解码器。
   * @param error_code FFmpeg 错误码。
   * @return QString 对应错误码的描述字符串。
   */
  static QString FFmpegError(int error_code);

  /**
   * @brief 释放图像缩放上下文 (SwsContext)。
   */
  void FreeScaler();

  /**
   * @brief 将 FFmpeg 的 AVPixelFormat 转换为 Olive 的原生 PixelFormat。
   * @param pix_fmt FFmpeg 的像素格式。
   * @return PixelFormat Olive 的像素格式。
   */
  static PixelFormat GetNativePixelFormat(AVPixelFormat pix_fmt);

  /**
   * @brief 根据 FFmpeg 的 AVPixelFormat 获取 Olive 的原生通道数。
   * @param pix_fmt FFmpeg 的像素格式。
   * @return int 通道数。
   */
  static int GetNativeChannelCount(AVPixelFormat pix_fmt);

  /**
   * @brief 验证并获取流的通道布局。
   * @param stream 指向 AVStream 的指针。
   * @return uint64_t 通道布局掩码。
   */
  static uint64_t ValidateChannelLayout(AVStream* stream);

  /**
   * @brief 将 Olive 的隔行扫描模式转换为 FFmpeg 使用的字符串表示。
   * @param interlacing Olive 的 VideoParams::Interlacing 枚举值。
   * @return const char* FFmpeg 使用的隔行扫描模式字符串。
   */
  static const char* GetInterlacingModeInFFmpeg(VideoParams::Interlacing interlacing);

  /**
   * @brief 检查给定的 AVPixelFormat 是否与 GLSL 兼容。
   * @param f 要检查的 AVPixelFormat。
   * @return bool 如果兼容则返回 true，否则返回 false。
   */
  static bool IsPixelFormatGLSLCompatible(AVPixelFormat f);

  /**
   * @brief 从缓存中根据时间戳获取 AVFrame。
   * @param t 时间戳。
   * @return AVFramePtr 指向缓存中 AVFrame 的智能指针，如果未找到则为空。
   */
  [[nodiscard]] AVFramePtr GetFrameFromCache(const int64_t& t) const;

  /**
   * @brief 清空帧缓存。
   */
  void ClearFrameCache();

  /**
   * @brief 对解码后的帧进行预处理。
   * @param f 原始解码帧。
   * @param p 视频检索参数。
   * @return AVFramePtr 预处理后的帧。
   */
  AVFramePtr PreProcessFrame(AVFramePtr f, const RetrieveVideoParams& p);

  /**
   * @brief 将 AVFrame 处理并转换为 TexturePtr。
   * @param f 要处理的 AVFrame。
   * @param p 视频检索参数。
   * @param original 原始解码帧（可能用于某些处理）。
   * @return TexturePtr 转换后的纹理。
   */
  TexturePtr ProcessFrameIntoTexture(const AVFramePtr& f, const RetrieveVideoParams& p, const AVFramePtr& original);

  /**
   * @brief 根据指定时间检索视频帧。
   * @param time 要检索帧的时间点。
   * @param cancelled 指向 CancelAtom 的指针，用于检查是否已请求取消。
   * @return AVFramePtr 指向检索到的 AVFrame 的智能指针，失败或取消则为空。
   */
  AVFramePtr RetrieveFrame(const rational& time, CancelAtom* cancelled);

  /**
   * @brief 从帧缓存中移除第一个（最早的）帧。
   */
  void RemoveFirstFrame();

  /**
   * @brief 获取帧缓存队列的最大允许大小。
   * @return int 队列最大大小。
   */
  static int MaximumQueueSize();

  /**
   * @brief FFmpeg 图像缩放上下文。
   */
  SwsContext* sws_ctx_{nullptr};
  /**
   * @brief 图像缩放源宽度。
   */
  int sws_src_width_{};
  /**
   * @brief 图像缩放源高度。
   */
  int sws_src_height_{};
  /**
   * @brief 图像缩放源像素格式。
   */
  AVPixelFormat sws_src_format_;
  /**
   * @brief 图像缩放目标宽度。
   */
  int sws_dst_width_{};
  /**
   * @brief 图像缩放目标高度。
   */
  int sws_dst_height_{};
  /**
   * @brief 图像缩放目标像素格式。
   */
  AVPixelFormat sws_dst_format_;
  /**
   * @brief 图像缩放颜色范围。
   */
  AVColorRange sws_colrange_;
  /**
   * @brief 图像缩放颜色空间。
   */
  AVColorSpace sws_colspace_;

  /**
   * @brief 工作数据包，用于存储从 FFmpeg 读取的原始数据。
   */
  AVPacket* working_packet_{nullptr};

  /**
   * @brief 用于计算帧率或处理非单调时间戳的第二个时间戳。
   */
  int64_t second_ts_{};

  /**
   * @brief 存储解码后帧的缓存列表。
   */
  std::list<AVFramePtr> cached_frames_;

  /**
   * @brief 标记是否在时间点 0 缓存了帧。
   */
  bool cache_at_zero_{false};
  /**
   * @brief 标记是否已缓存到文件末尾 (EOF)。
   */
  bool cache_at_eof_{false};

  /**
   * @brief FFmpeg 流实例对象，用于实际的解码操作。
   */
  Instance instance_;
};

}  // namespace olive

#endif  // FFMPEGDECODER_H