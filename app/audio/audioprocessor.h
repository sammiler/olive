#ifndef AUDIOPROCESSOR_H
#define AUDIOPROCESSOR_H

#include <inttypes.h>
#include <olive/core/core.h>
#include <QByteArray>
#include <QVector>  // 为 QVector 包含

extern "C" {
#include <libavfilter/avfilter.h>
}

#include "common/define.h"

namespace olive {

using namespace core;

/**
 * @brief 处理音频处理任务，如格式转换和速度调整。
 *
 *此类利用 FFmpeg 的 libavfilter 库来创建和管理音频
 * 滤镜图。它允许将音频从一组参数（例如采样率、
 * 格式、通道布局）转换为另一组参数，并且还可以应用诸如速度变化之类的效果。
 */
class AudioProcessor {
 public:
  /**
   * @brief 构造一个 AudioProcessor 对象。
   * 将内部 FFmpeg 相关的指针初始化为 nullptr。
   */
  AudioProcessor();

  /**
   * @brief 销毁 AudioProcessor 对象。
   * 确保调用 Close() 以释放任何已分配的 FFmpeg 资源。
   */
  ~AudioProcessor();

  /**
   * @brief 禁止此类的拷贝和移动构造函数及赋值运算符。
   * 这是通过 DISABLE_COPY_MOVE 宏实现的。
   */
  DISABLE_COPY_MOVE(AudioProcessor)

  /**
   * @brief 使用指定的输入、输出参数和速度初始化音频处理器。
   * 为音频转换设置 FFmpeg 滤镜图。
   * @param from 输入音频的音频参数。
   * @param to 输出音频的目标音频参数。
   * @param tempo 要应用的速度因子（例如，1.0 表示原始速度，2.0 表示双倍速度）。
   * @return bool 如果处理器成功打开并配置，则返回 true，否则返回 false。
   */
  bool Open(const AudioParams &from, const AudioParams &to, double tempo = 1.0);

  /**
   * @brief 关闭音频处理器并释放所有相关的 FFmpeg 资源。
   * 包括释放滤镜图、上下文和帧。
   */
  void Close();

  /**
   * @brief 检查音频处理器当前是否已打开并准备好进行处理。
   * @return bool 如果滤镜图已初始化，则返回 true，否则返回 false。
   */
  [[nodiscard]] bool IsOpen() const { return filter_graph_; }

  /**
   * @brief 用于存储音频数据的缓冲区的类型别名。
   * 它是一个 QVector，其中每个 QByteArray 通常包含一个音频采样平面。
   */
  using Buffer = QVector<QByteArray>;

  /**
   * @brief 转换一块输入音频样本。
   * 输入样本通过配置的 FFmpeg 滤镜图进行处理。
   * @param in 指向浮点样本缓冲区的C风格指针数组（期望平面格式）。
   * 数组中的每个缓冲区代表一个通道。
   * @param nb_in_samples 'in' 缓冲区中每个通道的输入样本数。
   * @param output 指向 AudioProcessor::Buffer 的指针，转换后的音频数据将存储在此处。
   * 输出是平面的，QVector 中的每个 QByteArray 代表一个通道。
   * @return int 生成的每个通道的输出样本数。错误时返回负值。
   */
  int Convert(float **in, int nb_in_samples, AudioProcessor::Buffer *output);

  /**
   * @brief 从滤镜图中刷新任何剩余的缓冲帧。
   * 在所有输入样本都已传递给 Convert() 之后，应调用此方法
   * 以确保检索所有已处理的音频。
   */
  void Flush();

  /**
   * @brief 获取处理器使用的输入音频参数。
   * @return const AudioParams& 对输入音频参数的常量引用。
   */
  [[nodiscard]] const AudioParams &from() const { return from_; }

  /**
   * @brief 获取处理器针对的输出音频参数。
   * @return const AudioParams& 对目标输出音频参数的常量引用。
   */
  [[nodiscard]] const AudioParams &to() const { return to_; }

 private:
  /**
   * @brief 创建并配置一个 FFmpeg 'atempo' 滤镜用于速度调整。
   * @param graph 将要添加 atempo 滤镜的 FFmpeg 滤镜图。
   * @param link atempo 滤镜将从中接收输入的 FFmpeg 滤镜上下文。
   * @param tempo 速度因子（例如，1.0 表示无变化，0.5 表示半速，2.0 表示双倍速）。
   * @return AVFilterContext* 指向创建的 atempo 滤镜上下文的指针，失败时为 nullptr。
   */
  static AVFilterContext *CreateTempoFilter(AVFilterGraph *graph, AVFilterContext *link, const double &tempo);

  /**
   * @brief 指向 FFmpeg 滤镜图实例的指针。
   */
  AVFilterGraph *filter_graph_;

  /**
   * @brief 指向 FFmpeg buffersrc (音频缓冲源) 滤镜上下文的指针。
   */
  AVFilterContext *buffersrc_ctx_{};

  /**
   * @brief 指向 FFmpeg buffersink (音频缓冲接收器) 滤镜上下文的指针。
   */
  AVFilterContext *buffersink_ctx_{};

  /**
   * @brief 输入音频的音频参数。
   */
  AudioParams from_;
  /**
   * @brief 对应于输入音频的 FFmpeg 样本格式。
   */
  AVSampleFormat from_fmt_;

  /**
   * @brief 目标输出音频的音频参数。
   */
  AudioParams to_;
  /**
   * @brief 对应于目标输出音频的 FFmpeg 样本格式。
   */
  AVSampleFormat to_fmt_;

  /**
   * @brief 指向用于向滤镜图馈送输入样本的 FFmpeg AVFrame 的指针。
   */
  AVFrame *in_frame_;

  /**
   * @brief 指向用于从滤镜图检索输出样本的 FFmpeg AVFrame 的指针。
   */
  AVFrame *out_frame_;
};

}  // namespace olive

#endif  // AUDIOPROCESSOR_H