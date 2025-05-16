#ifndef SOLIDGENERATOR_H  // 防止头文件被多次包含的宏定义开始
#define SOLIDGENERATOR_H

#include "node/node.h"  // 引入基类 Node 的定义

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 代表“纯色生成器”的节点。
 * 该节点生成一个指定颜色的纯色图像（纹理）。
 * 它通常用于创建背景、颜色遮罩或作为其他效果的基础。
 */
class SolidGenerator : public Node {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief SolidGenerator 构造函数。
      *  通常会在这里初始化颜色输入参数。
      */
     SolidGenerator();

  NODE_DEFAULT_FUNCTIONS(SolidGenerator)  // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点的名称。
   * @return QString 类型的节点名称 (例如 "纯色" 或 "纯色生成器")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点属于 "生成器" (Generator) 分类。
   */
  [[nodiscard]] QVector<CategoryID> Category() const override;
  /**
   * @brief 获取此节点的描述信息。
   * @return QString 类型的节点描述。
   */
  [[nodiscard]] QString Description() const override;

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述、参数名)。
   */
  void Retranslate() override;

  /**
   * @brief 计算并输出节点在特定时间点的值（生成的纯色图像）。
   * @param value 当前输入行数据 (主要包含颜色参数)。
   * @param globals 全局节点处理参数 (例如输出分辨率)。
   * @param table 用于存储输出值的表 (包含输出的纯色图像)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;
  /**
   * @brief 获取用于生成此纯色图像的着色器代码。
   * @param request 包含着色器请求参数的对象 (主要包含颜色信息)。
   * @return ShaderCode 对象，包含顶点和片段着色器代码 (片段着色器会输出指定的颜色)。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kColorInput;  ///< "Color" - 选择纯色颜色的参数键名。
};

}  // namespace olive

#endif  // SOLIDGENERATOR_H // 头文件宏定义结束