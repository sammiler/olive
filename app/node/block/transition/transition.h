#ifndef TRANSITIONBLOCK_H
#define TRANSITIONBLOCK_H

#include "node/block/block.h"  // 引入基类 Block 的定义

namespace olive {

class ClipBlock;  // 前向声明 ClipBlock 类，转场通常发生在 ClipBlock 之间

/**
 * @brief 转场效果的基类节点。
 * 定义了在两个媒体片段 (Block，通常是 ClipBlock) 之间进行过渡所需的核心功能。
 */
class TransitionBlock : public Block {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief TransitionBlock 构造函数。
      */
     TransitionBlock();

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本。
   * 例如，更新节点名称、描述或参数名称的翻译。
   */
  void Retranslate() override;

  /**
   * @brief 获取转场效果在入点片段（前一个片段）上的偏移量。
   * 表示转场从前一个片段结束点之前多少时间开始。
   * @return rational 类型的入点偏移量。
   */
  [[nodiscard]] rational in_offset() const;
  /**
   * @brief 获取转场效果在出点片段（后一个片段）上的偏移量。
   * 表示转场在后一个片段开始点之后多少时间结束。
   * @return rational 类型的出点偏移量。
   */
  [[nodiscard]] rational out_offset() const;

  /**
   * @brief 返回转场的“中点”，相对于转场自身。
   * 用于计算入点和出点偏移量。
   * 值为 0 表示转场的中心点恰好在中间，此时入点和出点偏移量相等。
   * 值为 -1 表示转场完全在前一个片段的末尾（例如，淡出）。
   * 值为 +1 表示转场完全在后一个片段的开头（例如，淡入）。
   * @return rational 类型的中心偏移值。
   */
  [[nodiscard]] rational offset_center() const;
  /**
   * @brief 设置转场的“中点”。
   * @param r rational 类型的中心偏移值。
   */
  void set_offset_center(const rational &r);

  /**
   * @brief 设置转场的入点偏移、出点偏移，并由此确定转场长度。
   * @param in_offset 入点偏移量。
   * @param out_offset 出点偏移量。
   */
  void set_offsets_and_length(const rational &in_offset, const rational &out_offset);

  /**
   * @brief 检查这是否是一个双边转场（即连接了前后两个片段）。
   * @return 如果同时连接了出片段和入片段，则返回 true；否则返回 false（例如，仅淡入或仅淡出）。
   */
  [[nodiscard]] bool is_dual_transition() const { return connected_out_block() && connected_in_block(); }

  /**
   * @brief 获取连接到此转场的“出点”片段（即转场开始时正在播放的片段）。
   * @return 指向 Block 对象的指针，如果没有连接则为 nullptr。
   */
  [[nodiscard]] Block *connected_out_block() const;
  /**
   * @brief 获取连接到此转场的“入点”片段（即转场结束时将要播放的片段）。
   * @return 指向 Block 对象的指针，如果没有连接则为 nullptr。
   */
  [[nodiscard]] Block *connected_in_block() const;

  /**
   * @brief 获取在给定时间点转场的总进度。
   * @param time 相对于转场开始的时间。
   * @return double 类型的进度值 (通常在 0.0 到 1.0 之间)。
   */
  [[nodiscard]] double GetTotalProgress(const double &time) const;
  /**
   * @brief 获取在给定时间点，出点片段（前一个片段）的淡出进度。
   * @param time 相对于转场开始的时间。
   * @return double 类型的进度值。
   */
  [[nodiscard]] double GetOutProgress(const double &time) const;
  /**
   * @brief 获取在给定时间点，入点片段（后一个片段）的淡入进度。
   * @param time 相对于转场开始的时间。
   * @return double 类型的进度值。
   */
  [[nodiscard]] double GetInProgress(const double &time) const;

  /**
   * @brief 计算并输出转场节点在特定时间点的值（例如混合后的视频帧或音频样本）。
   * @param value 当前输入行数据。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  /**
   * @brief 使指定时间范围内的缓存无效。
   * @param range 需要作废的时间范围。
   * @param from 触发作废的源头标识。
   * @param element 相关的元素索引，默认为 -1。
   * @param options 作废缓存的选项，默认为 InvalidateCacheOptions()。
   */
  void InvalidateCache(const TimeRange &range, const QString &from, int element = -1,
                       InvalidateCacheOptions options = InvalidateCacheOptions()) override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kOutBlockInput;  ///< "OutBlock" - 连接前一个（淡出）片段的输入端口键名。
  static const QString kInBlockInput;   ///< "InBlock" - 连接后一个（淡入）片段的输入端口键名。
  static const QString kCurveInput;     ///< "Curve" - 转场缓动曲线类型参数的键名。
  static const QString kCenterInput;    ///< "Center" - 转场中心偏移参数的键名。

 protected:
  /**
   * @brief 虚函数，供派生类在着色器作业执行前设置特定参数。
   * 派生类应重写此方法以传递转场特有的数据给着色器（例如，特定颜色、纹理等）。
   * @param value 当前输入行数据。
   * @param job 指向 ShaderJob 对象的指针，允许设置着色器参数。
   */
  virtual void ShaderJobEvent(const NodeValueRow &value, ShaderJob *job) const {}

  /**
   * @brief 虚函数，供派生类处理音频采样的转场。
   * 派生类应重写此方法以实现音频的混合或过渡效果。
   * @param from_samples 来自前一个片段的音频采样缓冲区。
   * @param to_samples 来自后一个片段的音频采样缓冲区。
   * @param out_samples 用于存储混合后输出音频采样的缓冲区。
   * @param time_in 当前转场进度的时间点（通常是 0.0 到 1.0 之间的归一化值）。
   */
  virtual void SampleJobEvent(const SampleBuffer &from_samples, const SampleBuffer &to_samples,
                              SampleBuffer &out_samples, double time_in) const {}

  /**
   * @brief 根据当前设置的缓动曲线类型，转换线性的进度值。
   * @param linear 线性的进度值 (0.0 到 1.0)。
   * @return double 类型的经过缓动曲线调整后的进度值。
   */
  [[nodiscard]] double TransformCurve(double linear) const;

  /**
   * @brief 当节点的某个输入端口连接上一个输出节点时调用的事件处理函数。
   * @param input 连接的输入端口名称。
   * @param element 输入端口的元素索引。
   * @param output 连接到的输出节点。
   */
  void InputConnectedEvent(const QString &input, int element, Node *output) override;

  /**
   * @brief 当节点的某个输入端口断开连接时调用的事件处理函数。
   * @param input 断开连接的输入端口名称。
   * @param element 输入端口的元素索引。
   * @param output 之前连接的输出节点。
   */
  void InputDisconnectedEvent(const QString &input, int element, Node *output) override;

  /**
   * @brief 根据输入调整时间范围。
   * @param input 输入端口的名称。
   * @param element 输入端口的元素索引。
   * @param input_time 输入的时间范围。
   * @param clamp 是否将时间限制在有效范围内。
   * @return 调整后的时间范围。
   */
  [[nodiscard]] TimeRange InputTimeAdjustment(const QString &input, int element, const TimeRange &input_time,
                                              bool clamp) const override;

  /**
   * @brief 根据输出调整时间范围。
   * @param input 输入端口的名称（通常指引向此输出的逻辑输入）。
   * @param element 输入端口的元素索引。
   * @param input_time 内部处理的时间范围。
   * @return 调整后用于输出的时间范围。
   */
  [[nodiscard]] TimeRange OutputTimeAdjustment(const QString &input, int element,
                                               const TimeRange &input_time) const override;

 private:
  /**
   * @brief 定义转场缓动曲线的类型。
   */
  enum CurveType {
    kLinear,       ///< 线性曲线 (匀速变化)。
    kExponential,  ///< 指数曲线 (先慢后快或先快后慢，取决于具体实现)。
    kLogarithmic   ///< 对数曲线 (与指数相反的效果)。
  };

  /**
   * @brief 获取内部使用的转场时间表示。
   * @param time 外部传入的时间。
   * @return double 类型的内部转场时间。
   */
  [[nodiscard]] static double GetInternalTransitionTime(const double &time);

  /**
   * @brief 将转场相关的时间信息插入到加速作业（如 GPU 作业）中。
   * @param job 指向 AcceleratedJob 对象的指针。
   * @param time 当前时间。
   */
  void InsertTransitionTimes(AcceleratedJob *job, const double &time) const;

  // --- 私有成员变量 ---
  ClipBlock *connected_out_block_;  ///< 指向连接的“出点”片段 (前一个片段) 的指针。
                                    ///< 注意: 公共接口返回 Block*, 这里是 ClipBlock*，可能是内部优化或特定假设。

  ClipBlock *connected_in_block_;  ///< 指向连接的“入点”片段 (后一个片段) 的指针。
                                   ///< 注意: 公共接口返回 Block*, 这里是 ClipBlock*。
};

}  // namespace olive

#endif  // TRANSITIONBLOCK_H