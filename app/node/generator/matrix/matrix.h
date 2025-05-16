#ifndef MATRIXGENERATOR_H  // 防止头文件被多次包含的宏定义开始
#define MATRIXGENERATOR_H

#include <QVector2D>  // Qt 二维向量类，用于表示位置、缩放、锚点等

#include "node/inputdragger.h"  // 引入输入拖动器相关定义，可能用于 Gizmo 交互
#include "node/node.h"          // 引入基类 Node 的定义

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 代表一个生成变换矩阵的节点的基类。
 * 该类提供了通过位置、旋转、缩放和锚点等参数生成一个 4x4 变换矩阵 (QMatrix4x4) 的通用功能。
 * 具体的变换节点（如 TransformDistortNode）会继承此类。
 */
class MatrixGenerator : public Node {
 Q_OBJECT  // Qt 对象宏，用于支持信号和槽机制以及元对象系统
     public :
     /**
      * @brief MatrixGenerator 构造函数。
      *  通常会在这里初始化节点的输入参数（如位置、旋转、缩放等）。
      */
     MatrixGenerator();

  NODE_DEFAULT_FUNCTIONS(MatrixGenerator)  // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点的名称。
   * @return QString 类型的节点名称 (例如，对于特定子类可能是 "变换", 此基类可能是 "矩阵生成器" 或 "正交" 的默认名)。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此节点的短名称。
   * @return QString 类型的节点短名称 (例如 "Ortho" 可能是一个默认值)。
   */
  [[nodiscard]] QString ShortName() const override;
  /**
   * @brief 获取此节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点可能属于 "生成器" 或 "变换" 相关分类。
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
   * @brief 计算并输出节点在特定时间点的值。
   *  对于 MatrixGenerator 及其子类，这个值通常是生成的 4x4 变换矩阵。
   * @param value 当前输入行数据 (包含生成矩阵所需的参数，如位置、旋转、缩放等)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (输出生成的 QMatrix4x4 变换矩阵)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kPositionInput;      ///< "Position" - 位置 (平移) 参数的键名 (通常是 QVector2D 或类似类型)。
  static const QString kRotationInput;      ///< "Rotation" - 旋转角度参数的键名 (通常是 float 或 double)。
  static const QString kScaleInput;         ///< "Scale" - 缩放比例参数的键名 (通常是 QVector2D，表示 X 和 Y 轴的缩放)。
  static const QString kUniformScaleInput;  ///< "UniformScale" - 是否启用等比缩放的布尔参数键名。如果为
                                            ///< true，kScaleInput 的 Y 分量可能被忽略，X 分量用于双轴。
  static const QString kAnchorInput;  ///< "Anchor" - 变换锚点（旋转和缩放的中心点）参数的键名 (通常是 QVector2D)。

 protected:
  /**
   * @brief (静态工具函数) 根据节点当前的参数值生成一个 4x4 变换矩阵。
   * @param value 包含所有相关参数 (位置、旋转、缩放、锚点等) 的 NodeValueRow。
   * @param ignore_anchor 是否在生成矩阵时忽略锚点参数。
   * @param ignore_position 是否在生成矩阵时忽略位置参数。
   * @param ignore_scale 是否在生成矩阵时忽略缩放参数。
   * @param mat 初始矩阵，新的变换将在此矩阵基础上累积。通常是单位矩阵开始。
   * @return QMatrix4x4 生成的变换矩阵。
   */
  [[nodiscard]] static QMatrix4x4 GenerateMatrix(const NodeValueRow &value, bool ignore_anchor, bool ignore_position,
                                                 bool ignore_scale, const QMatrix4x4 &mat);
  /**
   * @brief (静态工具函数) 根据具体的位置、旋转、缩放和锚点值生成一个 4x4 变换矩阵。
   * @param pos 二维位置向量。
   * @param rot 旋转角度 (通常为度)。
   * @param scale 二维缩放向量。
   * @param uniform_scale 是否进行等比缩放。
   * @param anchor 二维锚点向量。
   * @param mat 初始矩阵，新的变换将在此矩阵基础上累积。
   * @return QMatrix4x4 生成的变换矩阵。
   */
  static QMatrix4x4 GenerateMatrix(const QVector2D &pos, const float &rot, const QVector2D &scale, bool uniform_scale,
                                   const QVector2D &anchor, QMatrix4x4 mat);

  /**
   * @brief 当节点的某个输入参数值发生变化时调用的事件处理函数。
   *  例如，当位置、旋转或缩放参数改变时，可能需要触发 Gizmo 更新或其他依赖此矩阵的节点的重新计算。
   * @param input 值发生变化的输入端口/参数名称。
   * @param element 相关元素的索引 (例如，对于向量参数，指明是哪个分量变化了)。
   */
  void InputValueChangedEvent(const QString &input, int element) override;
};

}  // namespace olive

#endif  // TRANSFORMDISTORT_H // 头文件宏定义结束, 注意：此处的宏名称 TRANSFORMDISTORT_H 与文件名 MATRIXGENERATOR_H
        // 不匹配，可能是笔误。