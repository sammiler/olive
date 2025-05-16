#ifndef COLORTRANSFORMJOB_H  // 防止头文件被重复包含的宏
#define COLORTRANSFORMJOB_H  // 定义 COLORTRANSFORMJOB_H 宏

#include <QMatrix4x4>  // Qt 4x4 矩阵类
#include <QString>     // Qt 字符串类
#include <utility>     // 标准库 utility 头文件，提供 std::move

#include "acceleratedjob.h"         // 包含 AcceleratedJob 基类的定义
#include "render/alphaassoc.h"      // 包含 AlphaAssociated (Alpha关联类型) 枚举的定义
#include "render/colorprocessor.h"  // 包含 ColorProcessor (颜色处理器) 相关的定义 (例如 ColorProcessorPtr)
#include "render/texture.h"         // 包含 Texture (纹理) 相关的定义 (例如 TexturePtr)

namespace olive {  // olive 项目的命名空间

class Node;  // 向前声明 Node 类

/**
 * @brief ColorTransformJob 类代表一个颜色转换的加速任务。
 *
 * 它继承自 AcceleratedJob，因此可以携带执行任务所需的参数值 (通常通过 Insert(row) 继承)。
 * ColorTransformJob 专门用于描述对输入纹理进行颜色空间转换、LUT应用、或其他颜色操作的任务。
 *
 * 主要属性包括：
 * - 输入纹理 (`input_texture_`)
 * - 颜色处理器 (`processor_`)，定义了要应用的颜色转换逻辑。
 * - 可选的自定义着色器 (`custom_shader_src_`, `custom_shader_id_`)，允许节点提供特定的GLSL代码。
 * - Alpha 关联类型 (`input_alpha_association_`)。
 * - 变换矩阵和裁剪矩阵，用于在颜色转换前对纹理进行几何变换或裁剪。
 * - 其他控制标志，如是否清空目标、是否强制不透明等。
 */
class ColorTransformJob : public AcceleratedJob {  // ColorTransformJob 继承自 AcceleratedJob
 public:
  // 默认构造函数，初始化成员变量为默认状态
  ColorTransformJob() {
    processor_ = nullptr;                   // 颜色处理器默认为空
    custom_shader_src_ = nullptr;           // 自定义着色器源节点默认为空
    input_alpha_association_ = kAlphaNone;  // 输入Alpha关联类型默认为无
    clear_destination_ = true;              // 默认在渲染前清空目标纹理
    force_opaque_ = false;                  // 默认不强制输出为不透明
                            // id_, input_texture_, custom_shader_id_, matrix_, crop_matrix_, function_name_
                            // 会进行默认初始化 (例如 QString 为空，NodeValue 为 kNone，QMatrix4x4 为单位矩阵)
  }

  /**
   * @brief 构造函数，使用一个 NodeValueRow 初始化任务参数。
   * @param row 包含任务参数的 NodeValueRow。
   */
  explicit ColorTransformJob(const NodeValueRow &row) : ColorTransformJob() {
    Insert(row);
  }  // 调用默认构造函数后插入参数

  /**
   * @brief 获取此颜色转换任务的ID。
   * 如果设置了覆盖ID (`id_`)，则返回覆盖ID；否则返回颜色处理器 (`processor_`) 的ID。
   * @return 返回任务的ID字符串。
   */
  [[nodiscard]] QString id() const {
    if (id_.isEmpty()) {                                 // 如果覆盖ID为空
      return processor_ ? processor_->id() : QString();  // 返回颜色处理器的ID (如果处理器存在)
    } else {
      return id_;  // 返回覆盖ID
    }
  }

  /**
   * @brief 设置一个覆盖ID，用于标识此任务。
   * @param id 要设置的覆盖ID字符串。
   */
  void SetOverrideID(const QString &id) { id_ = id; }

  /**
   * @brief 获取输入纹理 (作为 NodeValue)。
   * @return 返回包含输入纹理的 NodeValue 的常量引用。
   */
  [[nodiscard]] const NodeValue &GetInputTexture() const { return input_texture_; }
  /**
   * @brief 设置输入纹理 (通过 NodeValue)。
   * @param tex 包含输入纹理的 NodeValue。
   */
  void SetInputTexture(const NodeValue &tex) { input_texture_ = tex; }
  /**
   * @brief 设置输入纹理 (通过 TexturePtr)。
   * @param tex 指向输入纹理的 TexturePtr。断言确保纹理不是虚拟的。
   */
  void SetInputTexture(const TexturePtr &tex) {
    Q_ASSERT(!tex->IsDummy());                             // 确保纹理不是一个虚拟/占位纹理
    input_texture_ = NodeValue(NodeValue::kTexture, tex);  // 将 TexturePtr 包装成 NodeValue
  }

  /**
   * @brief 获取颜色处理器。
   * @return 返回指向颜色处理器的 ColorProcessorPtr。
   */
  [[nodiscard]] ColorProcessorPtr GetColorProcessor() const { return processor_; }
  /**
   * @brief 设置颜色处理器。
   * @param p 要设置的 ColorProcessorPtr。
   */
  void SetColorProcessor(ColorProcessorPtr p) { processor_ = std::move(p); }  // 使用 std::move 转移所有权

  /**
   * @brief 获取输入纹理的Alpha关联类型。
   * @return 返回 AlphaAssociated 枚举值的常量引用。
   */
  [[nodiscard]] const AlphaAssociated &GetInputAlphaAssociation() const { return input_alpha_association_; }
  /**
   * @brief 设置输入纹理的Alpha关联类型。
   * @param e 要设置的 AlphaAssociated 枚举值。
   */
  void SetInputAlphaAssociation(const AlphaAssociated &e) { input_alpha_association_ = e; }

  /**
   * @brief 获取提供自定义着色器代码的源节点。
   * @return 返回指向源节点的 const Node* 指针。
   */
  [[nodiscard]] const Node *CustomShaderSource() const { return custom_shader_src_; }
  /**
   * @brief 获取自定义着色器的ID。
   * @return 返回自定义着色器ID字符串的常量引用。
   */
  [[nodiscard]] const QString &CustomShaderID() const { return custom_shader_id_; }
  /**
   * @brief 设置此任务需要使用自定义着色器。
   * @param node 提供自定义着色器代码的节点。
   * @param id (可选) 自定义着色器的特定ID。
   */
  void SetNeedsCustomShader(const Node *node, const QString &id = QString()) {
    custom_shader_src_ = node;
    custom_shader_id_ = id;
  }

  /**
   * @brief 检查是否启用了在渲染前清空目标纹理。
   * @return 如果启用则返回 true。
   */
  [[nodiscard]] bool IsClearDestinationEnabled() const { return clear_destination_; }
  /**
   * @brief 设置是否启用在渲染前清空目标纹理。
   * @param e 如果为 true 则启用。
   */
  void SetClearDestinationEnabled(bool e) { clear_destination_ = e; }

  /**
   * @brief 获取应用于输入纹理的变换矩阵。
   * @return 返回 QMatrix4x4 的常量引用。
   */
  [[nodiscard]] const QMatrix4x4 &GetTransformMatrix() const { return matrix_; }
  /**
   * @brief 设置变换矩阵。
   * @param m 要设置的 QMatrix4x4 矩阵。
   */
  void SetTransformMatrix(const QMatrix4x4 &m) { matrix_ = m; }

  /**
   * @brief 获取应用于输入纹理的裁剪矩阵。
   * @return 返回 QMatrix4x4 的常量引用。
   */
  [[nodiscard]] const QMatrix4x4 &GetCropMatrix() const { return crop_matrix_; }
  /**
   * @brief 设置裁剪矩阵。
   * @param m 要设置的 QMatrix4x4 矩阵。
   */
  void SetCropMatrix(const QMatrix4x4 &m) { crop_matrix_ = m; }

  /**
   * @brief 获取在自定义着色器中可能调用的特定函数名称。
   * @return 返回函数名称字符串的常量引用。
   */
  [[nodiscard]] const QString &GetFunctionName() const { return function_name_; }
  /**
   * @brief 设置在自定义着色器中调用的函数名称。
   * @param function_name (可选) 函数名称。如果为空，则可能使用默认函数。
   */
  void SetFunctionName(const QString &function_name = QString()) { function_name_ = function_name; };

  /**
   * @brief 获取是否强制输出为不透明 (忽略Alpha通道)。
   * @return 如果强制不透明则返回 true。
   */
  [[nodiscard]] bool GetForceOpaque() const { return force_opaque_; }
  /**
   * @brief 设置是否强制输出为不透明。
   * @param e 如果为 true 则强制不透明。
   */
  void SetForceOpaque(bool e) { force_opaque_ = e; }

 private:
  ColorProcessorPtr processor_;  // 指向颜色处理器对象的智能指针
  QString id_;                   // 任务的覆盖ID (可选)

  NodeValue input_texture_;  // 输入纹理 (作为 NodeValue 存储)

  const Node *custom_shader_src_;  // 提供自定义着色器代码的源节点指针 (不拥有所有权)
  QString custom_shader_id_;       // 自定义着色器的ID

  AlphaAssociated input_alpha_association_;  // 输入纹理的Alpha关联类型

  bool clear_destination_;  // 是否在渲染前清空目标纹理的标志

  QMatrix4x4 matrix_;       // 应用于输入纹理的变换矩阵
  QMatrix4x4 crop_matrix_;  // 应用于输入纹理的裁剪矩阵

  QString function_name_;  // 在自定义着色器中调用的函数名 (可选)

  bool force_opaque_;  // 是否强制输出为不透明的标志
};

}  // namespace olive

#endif  // COLORTRANSFORMJOB_H