#ifndef SAMPLEJOB_H  // 防止头文件被重复包含的宏
#define SAMPLEJOB_H  // 定义 SAMPLEJOB_H 宏

#include "acceleratedjob.h"  // 包含 AcceleratedJob 基类的定义
                             // 也可能间接包含 NodeValue, SampleBuffer, TimeRange (通过 AcceleratedJob 或其他常用头文件)
// 为了明确，SampleJob 使用 SampleBuffer 和 TimeRange，这些通常定义在与 value.h 或 render/texture.h 相关的地方。
// AcceleratedJob 本身可能不直接包含它们，但使用 AcceleratedJob 的上下文通常会确保这些类型可用。

namespace olive {  // olive 项目的命名空间

/**
 * @brief SampleJob 类代表一个处理音频样本 (Audio Samples) 的加速任务。
 *
 * 它继承自 AcceleratedJob，但与 FootageJob 类似，它主要通过自身的成员变量
 * (`samples_` 和 `time_`) 来携带任务的核心数据，而不是依赖 AcceleratedJob 的 `value_map_`。
 * AcceleratedJob 的 `value_map_` 仍可用于传递此音频处理任务可能需要的其他参数。
 *
 * `samples_`: 存储了要处理的音频样本数据 (SampleBuffer)。
 * `time_`: 指定了这些音频样本对应的时间范围。
 *
 * 当 NodeTraverser遇到需要对已有音频数据进行进一步处理 (例如应用效果、混音等) 的节点时，
 * 可能会创建一个 SampleJob。渲染器 (例如 NodeTraverser 的 ProcessSamples 方法，
 * 或具体节点的 ProcessSamples 方法) 会使用这个 Job 中的音频数据和时间信息来进行处理。
 */
class SampleJob : public AcceleratedJob {  // SampleJob 继承自 AcceleratedJob
 public:
  // 默认构造函数
  SampleJob() = default;

  /**
   * @brief 构造函数，使用给定的时间和 NodeValue (从中提取音频样本) 初始化任务。
   * @param time 音频样本对应的时间范围。
   * @param value 包含音频样本数据的 NodeValue (其类型应为 NodeValue::kSamples)。
   */
  SampleJob(const TimeRange &time, const NodeValue &value) {
    samples_ = value.toSamples();  // 从 NodeValue 中提取 SampleBuffer
    time_ = time;                  // 设置时间范围
  }

  /**
   * @brief 构造函数，使用给定的时间、输入ID和 NodeValueRow (从中提取音频样本) 初始化任务。
   * @param time 音频样本对应的时间范围。
   * @param from NodeValueRow 中包含音频样本的键 (输入端口ID)。
   * @param row 包含输入参数的 NodeValueRow。
   */
  SampleJob(const TimeRange &time, const QString &from, const NodeValueRow &row) {
    samples_ = row[from].toSamples();  // 从 NodeValueRow 的指定输入中提取 SampleBuffer
    time_ = time;                      // 设置时间范围
    // 注意: AcceleratedJob 的 value_map_ 仍可通过 Insert(row) 等方法填充其他参数
  }

  /**
   * @brief 获取音频样本数据 (SampleBuffer)。
   * @return 返回 SampleBuffer 的常量引用。
   */
  [[nodiscard]] const SampleBuffer &samples() const { return samples_; }

  /**
   * @brief 检查此任务是否包含已分配的音频样本数据。
   * @return 如果 SampleBuffer 已分配内存，则返回 true。
   */
  [[nodiscard]] bool HasSamples() const { return samples_.is_allocated(); }

  /**
   * @brief 获取音频样本对应的时间范围。
   * @return 返回 TimeRange 的常量引用。
   */
  [[nodiscard]] const TimeRange &time() const { return time_; }

 private:
  SampleBuffer samples_;  // 存储音频样本数据

  TimeRange time_;  // 音频样本对应的时间范围
};

}  // namespace olive

// 声明 SampleJob 类型为元类型，以便在 QVariant 中使用或在信号槽中传递
Q_DECLARE_METATYPE(olive::SampleJob)

#endif  // SAMPLEJOB_H