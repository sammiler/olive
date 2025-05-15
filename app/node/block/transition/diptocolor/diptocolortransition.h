#ifndef DIPTOCOLORTRANSITION_H
#define DIPTOCOLORTRANSITION_H

#include "node/block/transition/transition.h" // 引入基类 TransitionBlock 的定义

namespace olive {

/**
 * @brief 代表“浸入颜色”转场效果的节点。
 * 效果通常是：第一个片段淡化至指定颜色，然后从该颜色淡化至第二个片段。
 */
class DipToColorTransition : public TransitionBlock {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief DipToColorTransition 构造函数。
   */
  DipToColorTransition();

  NODE_DEFAULT_FUNCTIONS(DipToColorTransition) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此“浸入颜色”转场的名称。
   * @return QString 类型的转场名称 (例如 "浸入颜色")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此“浸入颜色”转场的唯一标识符。
   * @return QString 类型的转场 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此转场所属的分类 ID 列表。
   * @return CategoryID 的 QVector，用于组织和归类转场效果。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /**
   * @brief 获取此“浸入颜色”转场的描述信息。
   * @return QString 类型的转场描述。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 获取用于渲染此转场效果的着色器代码。
   * @param request 包含着色器请求参数的对象 (例如目标格式、时间等)。
   * @return ShaderCode 对象，包含顶点和片段着色器代码。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本。
   * 例如，更新节点名称、描述或参数名称的翻译。
   */
  void Retranslate() override;

  static const QString kColorInput; ///< "Color" - 用于指定浸入颜色的输入参数的键名。

protected:
  /**
   * @brief 在着色器作业执行前设置相关的参数（例如浸入的颜色）。
   * 此方法会在渲染转场效果的每一帧（或需要时）被调用，以更新着色器的 uniform 变量。
   * @param value 当前输入行数据，可用于获取如颜色等参数值。
   * @param job 指向 ShaderJob 对象的指针，允许设置着色器参数。
   */
  void ShaderJobEvent(const NodeValueRow &value, ShaderJob *job) const override;
};

}  // namespace olive

#endif  // DIPTOCOLORTRANSITION_H