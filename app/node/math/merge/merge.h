#ifndef MERGENODE_H // 防止头文件被多次包含的宏定义开始
#define MERGENODE_H

#include "node/node.h" // 引入基类 Node 的定义

// 可能需要的前向声明
// struct ShaderRequest; // 假设
// struct NodeValueRow; // 假设
// struct NodeGlobals; // 假设
// class NodeValueTable; // 假设
// class NodeInput; // 假设

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 代表一个“合并”或“混合”节点。
 * 该节点用于将两个输入图像（通常称为“基础层”和“混合层”）根据某种混合模式（如叠加、相乘、滤色等）合并成一个输出图像。
 * 这个类定义比较基础，具体的混合模式逻辑可能在着色器代码中或通过参数控制。
 */
class MergeNode : public Node {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief MergeNode 构造函数。
   *  通常会在这里初始化两个输入端口（基础层和混合层）。
   */
  MergeNode();

  NODE_DEFAULT_FUNCTIONS(MergeNode) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此合并节点的名称。
   * @return QString 类型的节点名称 (例如 "合并" 或 "Merge")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此合并节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点属于 "混合" (Blending/Merge) 或 "合成" (Compositing) 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /**
   * @brief 获取此合并节点的描述信息。
   * @return QString 类型的节点描述。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述、参数名)。
   */
  void Retranslate() override;

  /**
   * @brief 获取用于执行此合并操作的着色器代码。
   * @param request 包含着色器请求参数的对象 (例如混合模式、不透明度等)。
   * @return ShaderCode 对象，包含顶点和片段着色器代码。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest& request) const override;
  /**
   * @brief 计算并输出节点在特定时间点的值（合并后的图像数据）。
   * @param value 当前输入行数据 (包含基础层和混合层图像，以及可能的混合参数)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含输出的合并后图像)。
   */
  void Value(const NodeValueRow& value, const NodeGlobals& globals, NodeValueTable* table) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kBaseIn;  ///< "Base" - 基础层输入纹理（图像）的参数键名。
  static const QString kBlendIn; ///< "Blend" 或 "Top" - 混合层（要叠加在基础层之上）输入纹理的参数键名。

 private:
  NodeInput* base_in_{};  ///< 指向基础层输入端口的 NodeInput 对象的指针 (初始化为 nullptr)。
  NodeInput* blend_in_{}; ///< 指向混合层输入端口的 NodeInput 对象的指针 (初始化为 nullptr)。
};

}  // namespace olive

#endif  // MERGENODE_H // 头文件宏定义结束