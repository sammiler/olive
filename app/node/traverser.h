#ifndef NODETRAVERSER_H // 防止头文件被重复包含的宏
#define NODETRAVERSER_H // 定义 NODETRAVERSER_H 宏

#include <QVector2D> // Qt 二维向量类

#include "codec/decoder.h"         // 解码器相关定义 (可能包含 VideoParams, AudioParams)
#include "common/cancelableobject.h" // 可取消对象基类 (NodeTraverser 可能间接使用)
#include "node/output/track/track.h" // 轨道节点类 (可能为了 LoopMode 枚举)
#include "render/cancelatom.h"       // 取消原子操作类，用于控制渲染过程的取消
#include "render/job/cachejob.h"       // 缓存任务定义
#include "render/job/colortransformjob.h" // 颜色转换任务定义
#include "render/job/footagejob.h"     // 素材处理任务定义
#include "value.h"                   // 值类型定义 (可能包含 NodeValue, NodeValueTable, NodeValueDatabase, TimeRange 等)

namespace olive { // olive 项目的命名空间

class Node; // 向前声明 Node 类，避免循环依赖

/**
 * @brief NodeTraverser 类负责遍历节点图并计算节点在特定时间或时间范围内的值。
 *
 * 它递归地处理节点的输入，处理连接、关键帧和静态值，
 * 并最终调用具体节点的处理函数 (例如 Value() 或 GenerateFrame())。
 * NodeTraverser 也可能负责管理和调度渲染任务 (Jobs) 以及处理缓存。
 *
 * 这个类是 Olive 渲染引擎的核心部分，用于按需生成帧数据或音频数据。
 * 它可以被子类化以实现不同的遍历策略或处理特定类型的渲染任务 (例如，CPU渲染与GPU渲染)。
 */
class NodeTraverser {
 public:
  // 构造函数
  NodeTraverser();

  /**
   * @brief 为指定节点在给定时间范围内生成一个值表 (NodeValueTable)。
   * 这个表通常包含该节点所有输入在指定时间范围内的计算结果。
   * @param n 要计算其值表的目标节点。
   * @param range 计算的时间范围。
   * @param next_node (可选) 遍历路径上的下一个节点，可能用于优化或特定逻辑。
   * @return 返回计算得到的 NodeValueTable。
   */
  NodeValueTable GenerateTable(const Node *n, const TimeRange &range, const Node *next_node = nullptr);

  /**
   * @brief 为指定节点在给定时间范围内生成一个值数据库 (NodeValueDatabase)。
   * 值数据库可能包含更复杂或多时间点的值信息。
   * 这是一个虚函数，允许子类提供不同的实现。
   * @param node 目标节点。
   * @param range 计算的时间范围。
   * @return 返回计算得到的 NodeValueDatabase。
   */
  virtual NodeValueDatabase GenerateDatabase(const Node *node, const TimeRange &range);

  /**
   * @brief 从给定的值数据库中为指定节点在特定时间范围生成一行值 (NodeValueRow)。
   * NodeValueRow 代表节点在某个特定时刻 (或极小时间片) 的所有输入值。
   * @param database 包含预计算值的数据库。
   * @param node 目标节点。
   * @param range 时间范围 (通常代表一个特定时间点)。
   * @return 返回 NodeValueRow。
   */
  NodeValueRow GenerateRow(NodeValueDatabase *database, const Node *node, const TimeRange &range);
  /**
   * @brief 直接为指定节点在特定时间范围生成一行值 (NodeValueRow)。
   * 这个版本可能会在内部调用 GenerateDatabase 或其他机制。
   * @param node 目标节点。
   * @param range 时间范围。
   * @return 返回 NodeValueRow。
   */
  NodeValueRow GenerateRow(const Node *node, const TimeRange &range);

  /**
   * @brief 为指定节点的特定输入，在给定时间点从值表中生成其值 (NodeValue)。
   * @param node 目标节点。
   * @param input 目标输入的ID。
   * @param table 包含预计算值的表。
   * @param time 时间点。
   * @return 返回该输入的 NodeValue。
   */
  NodeValue GenerateRowValue(const Node *node, const QString &input, NodeValueTable *table, const TimeRange &time);
  /**
   * @brief 为指定节点的特定输入元素的特定元素，在给定时间点从值表中生成其值 (NodeValue)。
   * @param node 目标节点。
   * @param input 目标输入的ID。
   * @param element 输入的元素索引 (用于数组类型输入)。
   * @param table 包含预计算值的表。
   * @param time 时间点。
   * @return 返回该输入元素的 NodeValue。
   */
  NodeValue GenerateRowValueElement(const Node *node, const QString &input, int element, NodeValueTable *table,
                                    const TimeRange &time);
  /**
   * @brief (静态) 根据值提示和首选类型，从值表中确定合适的元素索引。
   * 这可能用于处理多通道数据或根据上下文选择特定分量。
   * @param hint 输入的值提示。
   * @param preferred_type 首选的数据类型。
   * @param table 值表。
   * @return 返回合适的元素索引。
   */
  static int GenerateRowValueElementIndex(const Node::ValueHint &hint, NodeValue::Type preferred_type,
                                          const NodeValueTable *table);
  /**
   * @brief (静态) 根据节点输入和元素，从值表中确定合适的元素索引。
   * @param node 目标节点。
   * @param input 输入ID。
   * @param element 元素索引。
   * @param table 值表。
   * @return 返回合适的元素索引。
   */
  static int GenerateRowValueElementIndex(const Node *node, const QString &input, int element,
                                          const NodeValueTable *table);

  /**
   * @brief 计算从开始节点到结束节点之间，在指定时间范围内的累积变换。
   * 这个变换通常是几何变换 (如位置、旋转、缩放)。
   * @param transform 指向 QTransform 对象的指针，用于累积变换结果。
   * @param start 变换路径的起始节点。
   * @param end 变换路径的结束节点。
   * @param range 时间范围。
   */
  void Transform(QTransform *transform, const Node *start, const Node *end, const TimeRange &range);

  // 获取用于缓存的视频参数 (如分辨率、帧率、像素格式)
  [[nodiscard]] const VideoParams &GetCacheVideoParams() const { return video_params_; }

  // 设置用于缓存的视频参数
  void SetCacheVideoParams(const VideoParams &params) { video_params_ = params; }

  // 获取用于缓存的音频参数 (如采样率、通道数、样本格式)
  [[nodiscard]] const AudioParams &GetCacheAudioParams() const { return audio_params_; }

  // 设置用于缓存的音频参数
  void SetCacheAudioParams(const AudioParams &params) { audio_params_ = params; }

 protected:
  /**
   * @brief 处理指定节点的特定输入，在给定时间范围内生成其值表。
   * 这是递归计算节点输入值的核心函数。
   * @param node 目标节点。
   * @param input 要处理的输入端口的ID。
   * @param range 时间范围。
   * @return 返回该输入的值表 (NodeValueTable)。
   */
  NodeValueTable ProcessInput(const Node *node, const QString &input, const TimeRange &range);

  /**
   * @brief 处理指定节点的特定输入数组的特定元素，在给定时间范围内计算其值，并存入数组值表。
   * @param array_tbl 存储数组元素值的值表数组 (NodeValueTableArray)。
   * @param node 目标节点。
   * @param input 输入端口的ID (必须是数组类型)。
   * @param element 要处理的元素索引。
   * @param range 时间范围。
   */
  void ProcessInputElement(NodeValueTableArray &array_tbl, const Node *node, const QString &input, int element,
                           const TimeRange &range);

  // 以下 Process* 系列函数是虚函数，允许子类重写以实现特定的任务处理逻辑 (例如，CPU vs GPU)。
  // 它们通常在 ResolveJobs 函数中被调用，用于处理节点计算后产生的渲染任务 (Job)。

  /**
   * @brief (虚) 处理视频素材任务。
   * @param destination 目标纹理，用于存储处理结果。
   * @param stream 包含素材信息的 FootageJob。
   * @param input_time 需要处理的素材时间点。
   */
  virtual void ProcessVideoFootage(TexturePtr destination, const FootageJob *stream, const rational &input_time) {}

  /**
   * @brief (虚) 处理音频素材任务。
   * @param destination 目标音频样本缓冲区。
   * @param stream 包含素材信息的 FootageJob。
   * @param input_time 需要处理的素材时间范围。
   */
  virtual void ProcessAudioFootage(SampleBuffer &destination, const FootageJob *stream, const TimeRange &input_time) {}

  /**
   * @brief (虚) 处理着色器任务。
   * @param destination 目标纹理。
   * @param node 执行着色器的节点。
   * @param job 包含着色器信息的 ShaderJob。
   */
  virtual void ProcessShader(TexturePtr destination, const Node *node, const ShaderJob *job) {}

  /**
   * @brief (虚) 处理颜色转换任务。
   * @param destination 目标纹理。
   * @param node 执行颜色转换的节点。
   * @param job 包含颜色转换信息的 ColorTransformJob。
   */
  virtual void ProcessColorTransform(TexturePtr destination, const Node *node, const ColorTransformJob *job) {}

  /**
   * @brief (虚) 处理音频样本处理任务。
   * @param destination 目标音频样本缓冲区。
   * @param node 执行样本处理的节点。
   * @param range 时间范围。
   * @param job 包含样本处理信息的 SampleJob。
   */
  virtual void ProcessSamples(SampleBuffer &destination, const Node *node, const TimeRange &range,
                              const SampleJob &job) {}

  /**
   * @brief (虚) 处理帧生成任务 (例如，生成纯色、噪点等)。
   * @param destination 目标纹理。
   * @param node 执行帧生成的节点。
   * @param job 包含生成信息的 GenerateJob。
   */
  virtual void ProcessFrameGeneration(TexturePtr destination, const Node *node, const GenerateJob *job) {}

  /**
   * @brief (虚) 将源纹理从指定的输入色彩空间转换到参考色彩空间。
   * @param destination 目标纹理 (转换结果)。
   * @param source 源纹理。
   * @param input_cs 源纹理的输入色彩空间名称。
   */
  virtual void ConvertToReferenceSpace(TexturePtr destination, TexturePtr source, const QString &input_cs) {}

  /**
   * @brief (虚) 处理视频缓存任务，通常是从缓存中读取数据或将数据写入缓存。
   * @param val 缓存任务信息。
   * @return 返回从缓存中获取或新创建的纹理指针。
   */
  virtual TexturePtr ProcessVideoCacheJob(const CacheJob *val);

  /**
   * @brief (虚) 根据视频参数创建一个新的纹理。
   * @param p 视频参数。
   * @return 返回创建的纹理指针 (TexturePtr)。默认实现返回一个虚拟纹理。
   */
  virtual TexturePtr CreateTexture(const VideoParams &p) { return CreateDummyTexture(p); }

  /**
   * @brief (虚) 根据音频参数和样本数量创建一个新的音频样本缓冲区。
   * @param params 音频参数。
   * @param sample_count 样本数量。
   * @return 返回创建的 SampleBuffer。默认实现返回一个空的 SampleBuffer。
   */
  virtual SampleBuffer CreateSampleBuffer(const AudioParams &ms, int sample_count) {
    // 默认返回一个空的 SampleBuffer
    return {};
  }

  /**
   * @brief 根据音频参数和时长创建一个新的音频样本缓冲区。
   * @param params 音频参数。
   * @param length 时长。
   * @return 返回创建的 SampleBuffer。
   */
  SampleBuffer CreateSampleBuffer(const AudioParams &params, const rational &length) {
    if (params.is_valid()) { // 确保音频参数有效
      return CreateSampleBuffer(params, params.time_to_samples(length)); // 调用另一个重载版本
    } else {
      return {}; // 无效参数则返回空缓冲区
    }
  }

  // 生成当前上下文期望的渲染分辨率
  [[nodiscard]] QVector2D GenerateResolution() const;

  // 检查遍历/渲染过程是否已被取消
  bool IsCancelled() { return cancel_ && cancel_->IsCancelled(); }

  // 检查是否已“听到”取消信号 (可能用于更早地停止某些预处理)
  [[nodiscard]] bool HeardCancel() const { return cancel_ && cancel_->HeardCancel(); }

  // 获取取消原子对象的指针
  [[nodiscard]] CancelAtom *GetCancelPointer() const { return cancel_; }
  // 设置取消原子对象的指针
  void SetCancelPointer(CancelAtom *cancel) { cancel_ = cancel; }

  /**
   * @brief 解析 NodeValue 中包含的视频渲染任务 (Jobs)。
   * 这个函数会检查 NodeValue 是否包含需要进一步处理的 Job (如 FootageJob, ShaderJob)，
   * 并调用相应的 Process* 虚函数来执行这些任务，用实际的纹理数据替换 Job。
   * @param value 包含潜在渲染任务的 NodeValue (通常通过引用修改)。
   */
  void ResolveJobs(NodeValue &value);
  /**
   * @brief 解析 NodeValue 中包含的音频渲染任务 (Jobs)。
   * 类似于 ResolveJobs，但针对音频。
   * @param value 包含潜在音频渲染任务的 NodeValue。
   */
  void ResolveAudioJobs(NodeValue &value);

  // 获取当前所在的 Block 节点 (例如，时间轴上的片段或合成块)
  // Block 节点可能会影响其内部子节点的行为或时间上下文。
  [[nodiscard]] Block *GetCurrentBlock() const { return block_stack_.empty() ? nullptr : block_stack_.back(); }

  // 获取当前的循环模式 (例如，从 Track 节点获取)
  [[nodiscard]] LoopMode loop_mode() const { return loop_mode_; }

  // (虚) 检查当前遍历是否应该使用缓存
  [[nodiscard]] virtual bool UseCache() const { return false; } // 默认不使用缓存

 private:
  // (静态) 创建一个虚拟/占位纹理 (用于默认实现或错误情况)
  static TexturePtr CreateDummyTexture(const VideoParams &p);

  VideoParams video_params_; // 当前遍历上下文的视频参数 (分辨率、帧率等)
  AudioParams audio_params_; // 当前遍历上下文的音频参数 (采样率、通道等)

  CancelAtom *cancel_; // 指向取消原子对象的指针，用于中断渲染

  // 用于计算累积变换的成员变量
  const Node *transform_start_{}; // 变换路径的起始节点
  const Node *transform_now_{};   // 变换路径的当前节点
  QTransform *transform_;         // 指向外部传入的 QTransform 对象，用于累积结果

  // Block 节点栈，用于处理嵌套的 Block 上下文
  std::list<Block *> block_stack_;

  LoopMode loop_mode_; // 当前的循环模式

  // 内部缓存，用于存储已计算的节点值表，避免重复计算
  // 外层 QHash: 节点指针 -> 内层 QHash
  // 内层 QHash: 时间范围 -> 该节点在该时间范围的值表 (NodeValueTable)
  QHash<const Node *, QHash<TimeRange, NodeValueTable> > value_cache_;

  // 内部缓存，用于存储已解析 (实际渲染出来) 的纹理，避免重复渲染相同的Job
  // QHash: 某个 Job 产生的临时纹理标识 (或 Job 指针) -> 实际的 TexturePtr
  QHash<Texture *, TexturePtr> resolved_texture_cache_;
};

}  // namespace olive

#endif  // NODETRAVERSER_H