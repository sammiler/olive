#ifndef CROSSDISSOLVETRANSITION_H
#define CROSSDISSOLVETRANSITION_H

#include "node/block/transition/transition.h" // 引入基类 TransitionBlock 的定义

namespace olive {

/**
 * @brief 代表交叉溶解转场效果的节点。
 * 当一个片段淡出而另一个片段同时淡入时，使用此转场。
 */
class CrossDissolveTransition : public TransitionBlock {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief CrossDissolveTransition 构造函数。
   */
  CrossDissolveTransition();

  NODE_DEFAULT_FUNCTIONS(CrossDissolveTransition) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此交叉溶解转场的名称。
   * @return QString 类型的转场名称 (例如 "交叉溶解")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此交叉溶解转场的唯一标识符。
   * @return QString 类型的转场 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此转场所属的分类 ID 列表。
   * @return CategoryID 的 QVector，用于组织和归类转场效果。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /**
   * @brief 获取此交叉溶解转场的描述信息。
   * @return QString 类型的转场描述。
   */
  [[nodiscard]] QString Description() const override;

  // virtual void Retranslate() override; // 该行被注释掉了，如果需要国际化节点名称/描述等，可能需要取消注释并实现

  /**
   * @brief 获取用于渲染此转场效果的着色器代码。
   * @param request 包含着色器请求参数的对象 (例如目标格式、时间等)。
   * @return ShaderCode 对象，包含顶点和片段着色器代码。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

protected:
  /**
   * @brief 处理音频采样的转场事件。
   * 用于在两个音频片段之间应用交叉溶解（通常是音频的交叉淡化）。
   * @param from_samples 来自前一个片段的音频采样缓冲区。
   * @param to_samples 来自后一个片段的音频采样缓冲区。
   * @param out_samples 用于存储混合后输出音频采样的缓冲区。
   * @param time_in 当前转场进度的时间点（通常是 0.0 到 1.0 之间的归一化值）。
   */
  void SampleJobEvent(const SampleBuffer &from_samples, const SampleBuffer &to_samples, SampleBuffer &out_samples,
                      double time_in) const override;
};

}  // namespace olive

#endif  // CROSSDISSOLVETRANSITION_H