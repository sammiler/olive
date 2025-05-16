#ifndef FFMPEGENCODER_H
#define FFMPEGENCODER_H

// 修复包含 <avfilter.h> 时出现的奇怪宏定义问题
#include <inttypes.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}

#include "codec/encoder.h"  // 基类 Encoder 和其他相关定义
#include "common/define.h"  // 包含 olive::FramePtr, olive::SampleBuffer 等类型定义

// Forward declarations (如果需要)
// namespace olive {
// class Frame; // 假设 FramePtr 是 std::shared_ptr<Frame>
// class SampleBuffer;
// struct EncodingParams;
// struct AudioParams;
// struct SubtitleBlock;
// enum class PixelFormat;
// enum class SampleFormat;
// namespace core { class rational; }
// namespace ExportCodec { enum class Codec; }
// }

namespace olive {

/**
 * @brief 基于 FFmpeg 的编码器实现。
 *
 *此类继承自 Encoder，并封装了 FFmpeg 的编码功能，
 * 用于将 Olive 项目中的媒体数据（视频、音频、字幕）编码为各种格式。
 */
class FFmpegEncoder : public Encoder {
  Q_OBJECT
 public:
  /**
   * @brief 构造一个新的 FFmpegEncoder 对象。
   * @param params 编码所需的参数，定义了输出格式、编解码器、码率等。
   */
  explicit FFmpegEncoder(const EncodingParams &params);

  // 析构函数由基类 Encoder 通过宏提供默认实现

  /**
   * @brief 获取指定编解码器支持的像素格式列表。
   * @param c 要查询的导出编解码器类型。
   * @return QStringList 支持的像素格式名称列表。
   */
  [[nodiscard]] QStringList GetPixelFormatsForCodec(ExportCodec::Codec c) const override;

  /**
   * @brief 获取指定编解码器支持的音频采样格式列表。
   * @param c 要查询的导出编解码器类型。
   * @return std::vector<SampleFormat> 支持的音频采样格式列表。
   */
  [[nodiscard]] std::vector<SampleFormat> GetSampleFormatsForCodec(ExportCodec::Codec c) const override;

  /**
   * @brief 打开编码器并初始化所有必要的 FFmpeg 上下文和流。
   * @return bool 如果编码器成功打开并准备好写入数据，则返回 true，否则返回 false。
   */
  bool Open() override;

  /**
   * @brief 写入一个视频帧到编码器。
   * @param frame 指向包含视频帧数据的 olive::FramePtr 智能指针。
   * @param time 当前帧的时间戳。
   * @return bool 如果帧成功写入则返回 true，否则返回 false。
   */
  bool WriteFrame(olive::FramePtr frame, olive::core::rational time) override;

  /**
   * @brief 写入一个音频样本缓冲区到编码器。
   * @param audio 包含音频样本数据的 olive::SampleBuffer 对象。
   * @return bool 如果音频数据成功写入则返回 true，否则返回 false。
   */
  bool WriteAudio(const olive::SampleBuffer &audio) override;

  /**
   * @brief 写入原始音频数据到编码器。
   * @param audio_params 输入音频数据的参数。
   * @param data 指向音频数据平面数组的指针。
   * @param input_sample_count 每个通道的输入样本数。
   * @return bool 如果音频数据成功写入则返回 true，否则返回 false。
   */
  bool WriteAudioData(const AudioParams &audio_params, const uint8_t **data, int input_sample_count);

  /**
   * @brief 写入一个字幕块到编码器。
   * @param sub_block 指向包含字幕数据的 SubtitleBlock 对象的指针。
   * @return bool 如果字幕成功写入则返回 true，否则返回 false。
   */
  bool WriteSubtitle(const SubtitleBlock *sub_block) override;

  /**
   * @brief 关闭编码器，刷新所有挂起的帧并释放 FFmpeg 资源。
   */
  void Close() override;

  /**
   * @brief 获取视频编码时内部转换所需的目标像素格式。
   * @return PixelFormat 目标像素格式。
   */
  [[nodiscard]] PixelFormat GetDesiredPixelFormat() const override { return video_conversion_fmt_; }

 private:
  /**
   * @brief 处理 FFmpeg API 调用返回的错误码。
   *
   * 此函数会使用 FFmpeg API 获取错误码对应的描述性字符串，
   * 并通过基类的 Error() 方法报告错误。调用此函数通常意味着编码过程发生严重错误，
   * 编码器可能会被自动关闭。
   * @param context 发生错误的上下文描述，例如正在执行的操作。
   * @param error_code FFmpeg 返回的错误码。
   */
  void FFmpegError(const QString &context, int error_code);

  /**
   * @brief 将一个 AVFrame 写入到指定的 FFmpeg 编解码器上下文和流。
   * @param frame 指向要写入的 AVFrame 的指针。
   * @param codec_ctx 目标流的 AVCodecContext。
   * @param stream 目标 AVStream。
   * @return bool 如果帧成功写入则返回 true，否则返回 false。
   */
  bool WriteAVFrame(AVFrame *frame, AVCodecContext *codec_ctx, AVStream *stream);

  /**
   * @brief 初始化指定媒体类型的流和编解码器上下文。
   * @param type 要初始化的媒体类型 (例如 AVMediaType::AVMEDIA_TYPE_VIDEO)。
   * @param stream 指向 AVStream* 的指针，用于存储创建的流对象。
   * @param codec_ctx 指向 AVCodecContext* 的指针，用于存储创建的编解码器上下文。
   * @param codec 要使用的导出编解码器类型。
   * @return bool 如果初始化成功则返回 true，否则返回 false。
   */
  bool InitializeStream(enum AVMediaType type, AVStream **stream, AVCodecContext **codec_ctx,
                        const ExportCodec::Codec &codec);
  /**
   * @brief 初始化编解码器上下文。
   * @param stream 指向 AVStream* 的指针，关联的流。
   * @param codec_ctx 指向 AVCodecContext* 的指针，要初始化的编解码器上下文。
   * @param codec 要使用的 AVCodec 编码器。
   * @return bool 如果初始化成功则返回 true，否则返回 false。
   */
  bool InitializeCodecContext(AVStream **stream, AVCodecContext **codec_ctx, const AVCodec *codec);
  /**
   * @brief 设置编解码器上下文的参数。
   * @param stream 关联的 AVStream。
   * @param codec_ctx 要设置的 AVCodecContext。
   * @param codec 使用的 AVCodec 编码器。
   * @return bool 如果设置成功则返回 true，否则返回 false。
   */
  bool SetupCodecContext(AVStream *stream, AVCodecContext *codec_ctx, const AVCodec *codec);

  /**
   * @brief 刷新所有编码器中缓存的数据。
   */
  void FlushEncoders();
  /**
   * @brief 刷新指定编解码器上下文中缓存的数据。
   * @param codec_ctx 要刷新的 AVCodecContext。
   * @param stream 关联的 AVStream。
   */
  void FlushCodecCtx(AVCodecContext *codec_ctx, AVStream *stream);

  /**
   * @brief 初始化音频重采样上下文 (SwrContext)。
   * @param audio 输入音频的参数，用于确定是否需要重采样以及如何重采样。
   * @return bool 如果重采样上下文成功初始化或不需要重采样，则返回 true，否则返回 false。
   */
  bool InitializeResampleContext(const AudioParams &audio);

  /**
   * @brief 根据指定的导出编解码器类型和音频采样格式获取 FFmpeg 编码器。
   * @param c 导出编解码器类型。
   * @param aformat 音频采样格式 (如果适用)。
   * @return const AVCodec* 指向找到的 AVCodec 的指针，如果未找到则为 nullptr。
   */
  static const AVCodec *GetEncoder(ExportCodec::Codec c, SampleFormat aformat);

  /**
   * @brief FFmpeg 格式上下文，用于处理输出文件的封装。
   */
  AVFormatContext *fmt_ctx_{nullptr};

  /**
   * @brief 指向视频流的 AVStream 对象。
   */
  AVStream *video_stream_{nullptr};
  /**
   * @brief 视频流的 AVCodecContext。
   */
  AVCodecContext *video_codec_ctx_{nullptr};
  /**
   * @brief 用于视频缩放/像素格式转换的 FFmpeg 滤镜图。
   */
  AVFilterGraph *video_scale_ctx_{nullptr};
  /**
   * @brief 视频滤镜图中的源缓冲滤镜上下文。
   */
  AVFilterContext *video_buffersrc_ctx_{nullptr};
  /**
   * @brief 视频滤镜图中的接收缓冲滤镜上下文。
   */
  AVFilterContext *video_buffersink_ctx_{nullptr};
  /**
   * @brief 视频编码前内部转换的目标像素格式。
   */
  PixelFormat video_conversion_fmt_;

  /**
   * @brief 指向音频流的 AVStream 对象。
   */
  AVStream *audio_stream_{nullptr};
  /**
   * @brief 音频流的 AVCodecContext。
   */
  AVCodecContext *audio_codec_ctx_{nullptr};
  /**
   * @brief 用于音频重采样的 SwrContext。
   */
  SwrContext *audio_resample_ctx_{nullptr};
  /**
   * @brief 用于存储待编码音频数据的 AVFrame。
   */
  AVFrame *audio_frame_{nullptr};
  /**
   * @brief 音频帧能容纳的最大样本数。
   */
  int audio_max_samples_{};
  /**
   * @brief 当前在 audio_frame_ 中的样本偏移量。
   */
  int audio_frame_offset_{};
  /**
   * @brief 已写入的音频样本总数（用于调试或同步）。
   */
  int audio_write_count_{};

  /**
   * @brief 指向字幕流的 AVStream 对象。
   */
  AVStream *subtitle_stream_{nullptr};
  /**
   * @brief 字幕流的 AVCodecContext。
   */
  AVCodecContext *subtitle_codec_ctx_{nullptr};

  /**
   * @brief 标记编码器是否已成功打开。
   */
  bool open_{false};
};

}  // namespace olive

#endif  // FFMPEGENCODER_H