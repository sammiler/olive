#ifndef POLYGONGENERATOR_H // 防止头文件被多次包含的宏定义开始
#define POLYGONGENERATOR_H

#include <QPainterPath> // Qt 的 QPainterPath 类，用于表示和操作2D路径，是多边形的核心

#include "node/generator/shape/generatorwithmerge.h" // 引入基类 GeneratorWithMerge，表明此多边形生成器可能支持合并操作
#include "node/gizmo/line.h"                         // 引入线段 Gizmo (交互控件) 的定义
#include "node/gizmo/path.h"                         // 引入路径 Gizmo 的定义
#include "node/gizmo/point.h"                        // 引入点 Gizmo 的定义
#include "node/inputdragger.h"                       // 引入输入拖动器相关定义，用于 Gizmo 交互
#include "node/node.h"                               // 引入基类 Node 的定义

namespace olive { // Olive 编辑器的命名空间

/**
 * @brief 代表“多边形生成器”的节点。
 * 该节点允许用户创建和编辑一个由多个顶点（可能包含贝塞尔曲线控制点）定义的多边形形状。
 * 它继承自 GeneratorWithMerge，表明它是一个形状生成器，并可能具有合并形状的功能。
 */
class PolygonGenerator : public GeneratorWithMerge {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief PolygonGenerator 构造函数。
   *  通常在这里初始化节点的基本属性和输入参数。
   */
  PolygonGenerator();

  NODE_DEFAULT_FUNCTIONS(PolygonGenerator) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点的名称。
   * @return QString 类型的节点名称 (例如 "多边形")。
   */
  [[nodiscard]] QString Name() const override;
  /**
   * @brief 获取此节点的唯一标识符。
   * @return QString 类型的节点 ID。
   */
  [[nodiscard]] QString id() const override;
  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点属于 "生成器" (Generator) 或 "形状" (Shape) 分类。
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
   *  对于多边形生成器，这通常是生成的形状数据或渲染后的图像。
   * @param value 当前输入行数据 (包含多边形顶点、颜色等参数)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含输出的形状或图像)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  /**
   * @brief 生成一帧图像数据。
   *  这是 GeneratorWithMerge 基类或类似接口定义的方法，用于具体的帧生成逻辑。
   * @param frame 指向目标帧数据 (FramePtr) 的智能指针。
   * @param job 包含生成作业所需参数的 GenerateJob 对象。
   */
  void GenerateFrame(FramePtr frame, const GenerateJob &job) const override;

  /**
   * @brief 更新 Gizmo (交互控件，如多边形的顶点、贝塞尔控制点) 在屏幕上的位置。
   *  当节点参数变化或视图变化时调用。
   * @param row 当前节点的参数值。
   * @param globals 全局节点处理参数。
   */
  void UpdateGizmoPositions(const NodeValueRow &row, const NodeGlobals &globals) override;

  /**
   * @brief 获取用于渲染此多边形形状的着色器代码。
   * @param request 包含着色器请求参数的对象。
   * @return ShaderCode 对象，包含顶点和片段着色器代码。
   */
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kPointsInput; ///< "Points" - 定义多边形顶点（及可能的贝塞尔控制点）的参数键名。
  static const QString kColorInput;  ///< "Color" - 多边形填充颜色的参数键名。

 protected:
  /**
   * @brief 根据节点参数和视频参数获取一个生成作业 (ShaderJob)。
   *  此作业包含了渲染多边形所需的着色器和参数。
   * @param value 当前节点的参数值。
   * @param params 目标视频的参数 (如分辨率)。
   * @return ShaderJob 对象。
   */
  [[nodiscard]] ShaderJob GetGenerateJob(const NodeValueRow &value, const VideoParams & ms) const;

 protected slots:
  /**
   * @brief 当 Gizmo (例如多边形的顶点或贝塞尔控制点) 被拖动时调用的槽函数。
   * @param x Gizmo 在画布上的新 X 坐标。
   * @param y Gizmo 在画布上的新 Y 坐标。
   * @param modifiers 当前按下的键盘修饰键 (如 Shift, Ctrl)。
   */
  void GizmoDragMove(double x, double y, const Qt::KeyboardModifiers &modifiers) override;

 private:
  /**
   * @brief (静态工具函数) 将一个点（带有前后贝塞尔控制信息）添加到 QPainterPath 中。
   * @param path 指向要修改的 QPainterPath 对象的指针。
   * @param before 前一个点的贝塞尔控制信息（影响当前点入线的曲率）。
   * @param after 当前点的贝塞尔控制信息（影响当前点出线的曲率）。
   */
  static void AddPointToPath(QPainterPath *path, const Bezier &before, const Bezier &after);

  /**
   * @brief (静态工具函数) 根据给定的顶点数组生成一个 QPainterPath。
   * @param points 包含多边形顶点信息的 NodeValueArray。
   * @param size 顶点的数量。
   * @return QPainterPath 生成的路径对象。
   */
  static QPainterPath GeneratePath(const NodeValueArray &points, int size);

  /**
   * @brief (模板函数) 验证并调整存储 Gizmo 指针的 QVector 的大小。
   *  如果大小不匹配，会创建或删除 Gizmo 以达到 new_sz。
   * @tparam T Gizmo 的类型 (例如 PointGizmo*, LineGizmo*)。
   * @param vec Gizmo 指针的 QVector 引用。
   * @param new_sz 期望的向量大小。
   */
  template <typename T>
  void ValidateGizmoVectorSize(QVector<T *> &vec, int new_sz);

  /**
   * @brief (模板函数) 创建一个合适类型的 Gizmo。
   *  具体类型由模板参数 T 决定。
   * @tparam T Gizmo 的类型。
   * @return NodeGizmo* 指向新创建的 Gizmo 的指针。
   */
  template <typename T>
  NodeGizmo *CreateAppropriateGizmo();

  // --- Gizmo 相关私有成员变量 ---
  PathGizmo *poly_gizmo_;                          ///< 指向表示整个多边形路径的 PathGizmo 对象的指针。
  QVector<PointGizmo *> gizmo_position_handles_;   ///< 存储指向各个顶点位置控制 Gizmo (PointGizmo) 的 QVector。
  QVector<PointGizmo *> gizmo_bezier_handles_;     ///< 存储指向各个贝塞尔控制点 Gizmo (PointGizmo) 的 QVector。
  QVector<LineGizmo *> gizmo_bezier_lines_;        ///< 存储指向连接顶点和其贝塞尔控制点的线条 Gizmo (LineGizmo) 的 QVector。
};

}  // namespace olive

#endif  // POLYGONGENERATOR_H // 头文件宏定义结束