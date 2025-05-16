#ifndef SHADERJOB_H  // 防止头文件被重复包含的宏
#define SHADERJOB_H  // 定义 SHADERJOB_H 宏

#include <QMatrix4x4>  // Qt 4x4 矩阵类 (虽然在此文件中未直接使用，但Shader可能用到)
#include <QVector>     // Qt 动态数组容器

#include "acceleratedjob.h"  // 包含 AcceleratedJob 基类的定义
#include "render/texture.h"  // 包含 Texture (纹理) 相关的定义，特别是 Texture::Interpolation 枚举

// 假设 NodeInput 的定义 (来自 "node/param.h" 或类似文件) 已被间接包含，
// 因为 SetInterpolation(const NodeInput& ...) 和 SetIterations(int, const NodeInput& ...) 使用它。
// 如果没有，通常会显式 #include "node/param.h"。
// 但我将严格按照您提供的代码进行注释。

namespace olive {  // olive 项目的命名空间

/**
 * @brief ShaderJob 类代表一个执行自定义着色器 (Shader) 的加速任务。
 *
 * 它继承自 AcceleratedJob，因此可以携带执行任务所需的参数值 (通过 `value_map_`)，
 * 这些参数通常会作为 uniforms 传递给着色器。
 *
 * ShaderJob 特有的属性包括：
 * - 着色器的ID (`shader_id_`)，用于从着色器缓存或管理器中获取编译好的着色器程序。
 * - 迭代次数 (`iterations_`) 和迭代输入 (`iterative_input_`)，用于实现需要多次运行着色器的效果 (例如某些模糊效果)。
 * - 纹理插值模式 (`interpolation_`)，可以为不同的输入纹理指定不同的插值方法。
 * - 顶点坐标覆盖 (`vertex_overrides_`)，允许任务提供自定义的顶点数据，而不是使用默认的四边形。
 *
 * 当 NodeTraverser 处理一个应用了GLSL效果的节点时，会创建一个 ShaderJob。
 * 渲染器 (例如 NodeTraverser 的 ProcessShader 方法) 会使用这个 Job 中的信息来设置和运行着色器。
 */
class ShaderJob : public AcceleratedJob {  // ShaderJob 继承自 AcceleratedJob
 public:
  // 默认构造函数，初始化迭代次数为1，迭代输入为空
  ShaderJob() {
    iterations_ = 1;             // 默认迭代次数为1
    iterative_input_ = nullptr;  // 迭代输入默认为空 (QString 默认构造为空)
                                 // shader_id_, interpolation_, vertex_overrides_ 会进行默认初始化
  }

  /**
   * @brief 构造函数，使用一个 NodeValueRow 初始化任务参数。
   * NodeValueRow 中包含了着色器执行所需的 uniform 变量值。
   * @param row 包含任务参数的 NodeValueRow。
   */
  explicit ShaderJob(const NodeValueRow& row) : ShaderJob() { Insert(row); }  // 调用默认构造函数后，插入参数

  /**
   * @brief 获取着色器的ID。
   * @return 返回着色器ID字符串的常量引用。
   */
  [[nodiscard]] const QString& GetShaderID() const { return shader_id_; }

  /**
   * @brief 设置着色器的ID。
   * @param id 要设置的着色器ID。
   */
  void SetShaderID(const QString& id) { shader_id_ = id; }

  /**
   * @brief 设置迭代次数和用于迭代的输入端口。
   * 某些效果可能需要多次运行着色器，每次迭代时，`iterative_input` 的值可能会被更新
   * (例如，前一次迭代的输出作为下一次迭代的输入)。
   * @param iterations 迭代次数。
   * @param iterative_input 标记为迭代输入的 NodeInput 对象 (主要使用其输入ID)。
   */
  void SetIterations(int iterations, const NodeInput& iterative_input) {
    SetIterations(iterations, iterative_input.input());  // 调用另一个重载版本
  }

  /**
   * @brief 设置迭代次数和用于迭代的输入端口ID。
   * @param iterations 迭代次数。
   * @param iterative_input 标记为迭代输入的输入端口ID。
   */
  void SetIterations(int iterations, const QString& iterative_input) {
    iterations_ = iterations;
    iterative_input_ = iterative_input;
  }

  /**
   * @brief 获取着色器的迭代次数。
   * @return 返回迭代次数。
   */
  [[nodiscard]] int GetIterationCount() const { return iterations_; }

  /**
   * @brief 获取标记为迭代输入的输入端口ID。
   * @return 返回迭代输入ID字符串的常量引用。
   */
  [[nodiscard]] const QString& GetIterativeInput() const { return iterative_input_; }

  /**
   * @brief 获取指定输入ID的纹理插值模式。
   * @param id 输入端口的ID (通常是一个纹理输入)。
   * @return 返回 Texture::Interpolation 枚举值。如果未指定，则返回 Texture::kDefaultInterpolation。
   */
  [[nodiscard]] Texture::Interpolation GetInterpolation(const QString& id) const {
    return interpolation_.value(id, Texture::kDefaultInterpolation);  // 从哈希表中获取，如果不存在则返回默认值
  }

  /**
   * @brief 获取所有指定了插值模式的输入的映射。
   * @return 返回一个从输入ID到 Texture::Interpolation 的 QHash 的常量引用。
   */
  [[nodiscard]] const QHash<QString, Texture::Interpolation>& GetInterpolationMap() const { return interpolation_; }

  /**
   * @brief 为指定的输入端口设置纹理插值模式。
   * @param input NodeInput 对象，指定了输入端口。
   * @param interp 要设置的纹理插值模式。
   */
  void SetInterpolation(const NodeInput& input, Texture::Interpolation interp) {
    interpolation_.insert(input.input(), interp);  // 使用 NodeInput 的输入ID作为键
  }

  /**
   * @brief 为指定的输入端口ID设置纹理插值模式。
   * @param id 输入端口的ID。
   * @param interp 要设置的纹理插值模式。
   */
  void SetInterpolation(const QString& id, Texture::Interpolation interp) { interpolation_.insert(id, interp); }

  /**
   * @brief 设置自定义的顶点坐标。
   * 如果设置了这个，着色器在渲染时将使用这些顶点坐标而不是默认的全屏四边形。
   * @param vertex_coords 包含顶点坐标 (通常是 x, y, z, u, v 等交错排列) 的 QVector<float>。
   */
  void SetVertexCoordinates(const QVector<float>& vertex_coords) { vertex_overrides_ = vertex_coords; }

  /**
   * @brief 获取自定义的顶点坐标。
   * @return 返回包含顶点坐标的 QVector<float> 的引用。
   */
  const QVector<float>& GetVertexCoordinates() { return vertex_overrides_; }

 private:
  QString shader_id_;  // 着色器的唯一ID

  int iterations_;  // 着色器执行的迭代次数

  QString iterative_input_;  // 在多次迭代中，哪个输入端口的值会被更新 (通常是前一次迭代的输出)

  // 存储特定输入纹理的插值模式 (输入ID -> 插值模式)
  QHash<QString, Texture::Interpolation> interpolation_;

  // 存储覆盖默认渲染四边形的自定义顶点坐标
  QVector<float> vertex_overrides_;
};

}  // namespace olive

#endif  // SHADERJOB_H