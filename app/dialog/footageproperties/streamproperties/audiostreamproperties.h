#ifndef AUDIOSTREAMPROPERTIES_H
#define AUDIOSTREAMPROPERTIES_H

// Olive 内部头文件
// 假设 footage.h 声明了 Footage 类
#include "node/project/footage/footage.h"
// 假设 streamproperties.h 声明了 StreamProperties 基类
#include "streamproperties.h"  // 这个头文件的实际路径可能需要确认
// 假设 undocommand.h 声明了 MultiUndoCommand 类 (虽然 Accept 方法在此头文件中未被 Q_OBJECT 标记为槽)
#include "undo/undocommand.h"
// #include "common/define.h" // 如果需要 common/define.h 中的内容

namespace olive {

/**
 * @brief 用于表示和配置特定素材中单个音频流属性的类。
 *
 *此类继承自 StreamProperties，专门用于处理音频流的特定属性。
 * 它通常会关联到一个 Footage 对象和一个音频流索引，并允许用户
 * 查看或修改该音频流的属性（例如采样率、通道数、编解码器信息等，
 * 具体属性取决于 StreamProperties 基类和此类的实现）。
 * Accept() 方法通常用于将更改应用到数据模型或生成撤销命令。
 */
class AudioStreamProperties : public StreamProperties {
  // Q_OBJECT // 如果此类需要信号槽机制，则应添加此宏
 public:
  /**
   * @brief 构造一个新的 AudioStreamProperties 对象。
   * @param footage 指向包含此音频流的 Footage 对象的指针。
   * @param audio_index 此音频流在 `footage` 对象中的索引。
   */
  AudioStreamProperties(Footage *footage, int audio_index);

  // 如果 StreamProperties 有虚析构函数，这里也应该是虚的
  // ~AudioStreamProperties() override; // 默认析构函数通常足够，除非有特定资源管理需求

  /**
   * @brief 应用或接受对音频流属性所做的更改。
   *
   * 此方法是 StreamProperties 基类中定义的虚函数的覆盖实现。
   * 它负责将用户在属性编辑器中所做的修改持久化，
   * 例如更新相关的 Footage 对象中的数据，并可能创建和提交
   * 一个或多个撤销命令到传入的 `parent` (MultiUndoCommand) 中，
   * 以支持撤销/重做功能。
   *
   * @param parent 指向 MultiUndoCommand 对象的指针，用于聚合此属性更改产生的撤销命令。
   * 如果更改不需要撤销/重做，或者由外部统一处理，则此参数可能未使用。
   */
  void Accept(MultiUndoCommand *parent) override;

 private:
  /**
   * @brief 指向包含此音频流的 Footage 对象的指针。
   */
  Footage *footage_;

  /**
   * @brief 此音频流在 `footage_` 对象中的索引。
   * 用于标识要操作的具体音频流。
   */
  int audio_index_;
};

}  // namespace olive

#endif  // AUDIOSTREAMPROPERTIES_H