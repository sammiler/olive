#ifndef GENERATORWITHMERGE_H  // 防止头文件被多次包含的宏定义开始
#define GENERATORWITHMERGE_H

#include "node/node.h"  // 引入基类 Node 的定义

// 前向声明 TexturePtr，它通常是 std::shared_ptr<Texture> 或类似的智能指针类型
// namespace olive { // 如果 TexturePtr 在 olive 命名空间内定义
// class Texture;
// using TexturePtr = std::shared_ptr<Texture>;
// }

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 代表一类特殊生成器节点的基类，这类生成器能够将其输出与一个“基础”输入进行合并。
 * 例如，一个形状生成器（如圆形、矩形）可以生成一个形状，然后这个形状可以与一个背景图像（基础输入）合并。
 * 这个类提供了处理这种合并逻辑的通用功能。
 */
class GeneratorWithMerge : public Node {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief GeneratorWithMerge 构造函数。
      *  可能会初始化与合并操作相关的参数或状态。
      */
     GeneratorWithMerge();

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述、参数名)。
   */
  void Retranslate() override;

  /**
   * @brief 获取用于渲染此生成器（可能包括合并逻辑）的着色器代码。
   * @param request 包含着色器请求参数的对象。
   * @return ShaderCode 对象，包含顶点和片段着色器代码。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

  static const QString kBaseInput;  ///< "Base" - 基础输入纹理（图像）的参数键名。生成器的输出将与此基础输入进行合并。

 protected:
  /**
   * @brief 将一个可合并的作业（通常是已生成的形状或图案的纹理）推送到输出表，并处理与基础输入的合并。
   *  此方法封装了将生成器自身输出与 `kBaseInput` 提供的纹理进行混合的逻辑。
   * @param value 当前节点的参数值，可能包含合并模式等信息。
   * @param job 指向已生成的纹理 (TexturePtr) 的智能指针，这是生成器自身产生的内容。
   * @param table 用于存储最终输出结果（合并后的纹理）的 NodeValueTable。
   */
  void PushMergableJob(const NodeValueRow &value, const TexturePtr &job, NodeValueTable *table) const;
};

}  // namespace olive

#endif  // GENERATORWITHMERGE_H // 头文件宏定义结束