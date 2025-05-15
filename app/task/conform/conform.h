#ifndef CONFORMTASK_H
#define CONFORMTASK_H

#include "codec/decoder.h"
#include "node/project/footage/footage.h"
#include "task/task.h"

namespace olive {

/**
 * @brief ConformTask 类定义，继承自 Task 类。
 *
 * 该类用于执行“整合”（Conform）操作的任务。
 * “整合”通常指的是将解码后的媒体流（尤其是音频流）进行处理、转换格式或参数，
 * 并将其输出到指定文件的过程。这个任务封装了执行此类操作所需的参数和逻辑。
 */
class ConformTask : public Task {
  Q_OBJECT
 public:
  /**
   * @brief ConformTask 的构造函数。
   * @param decoder_id 解码器的唯一标识符字符串。用于指定使用哪个解码器进行操作。
   * @param stream 解码器的编解码器流信息。包含了要处理的原始媒体数据流的详细信息。
   * @param params 音频参数。定义了输出音频的目标特性，如采样率、声道数等。
   * @param output_filenames 一个包含一个或多个输出文件名称的字符串向量。指定了整合后文件的存储位置和名称。
   */
  ConformTask(QString decoder_id, const Decoder::CodecStream &stream, AudioParams params,
              const QVector<QString> &output_filenames);

protected:
  /**
   * @brief 执行整合任务的核心逻辑。
   *
   * 此方法重写自基类 Task 的 Run 方法。当任务被调度执行时，此函数会被调用。
   * 它负责读取解码器流，根据提供的音频参数进行处理，并将结果写入到指定的输出文件中。
   * @return 如果任务成功完成，则返回 true；如果发生任何错误导致任务失败，则返回 false。
   */
  bool Run() override;

private:
  QString decoder_id_; ///< @brief 存储解码器的唯一标识符。

  Decoder::CodecStream stream_; ///< @brief 存储编解码器流信息，包含了要被整合处理的原始媒体数据。

  AudioParams params_; ///< @brief 存储音频参数，用于配置输出音频的格式和特性。

  QVector<QString> output_filenames_; ///< @brief 存储输出文件的名称列表。整合后的结果将写入这些文件。
};

}  // namespace olive

#endif  // CONFORMTASK_H