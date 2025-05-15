#ifndef TRANSFORMDISTORTNODE_H
#define TRANSFORMDISTORTNODE_H

#include "node/generator/matrix/matrix.h" // 引入基类 MatrixGenerator，表明此节点生成一个变换矩阵
#include "node/gizmo/point.h"             // 引入点 Gizmo (交互控件)
#include "node/gizmo/polygon.h"           // 引入多边形 Gizmo
#include "node/gizmo/screen.h"            // 引入屏幕 Gizmo (可能用于旋转等)

namespace olive {

/**
 * @brief 代表“变换”效果的节点。
 * 该节点允许用户在二维空间中对图像进行平移、旋转、缩放等仿射变换。
 * 它继承自 MatrixGenerator，核心是生成一个 4x4 的变换矩阵。
 */
class TransformDistortNode : public MatrixGenerator {
  Q_OBJECT // Qt 对象宏，用于支持信号和槽机制以及元对象系统
 public:
  /**
   * @brief TransformDistortNode 构造函数。
   */
  TransformDistortNode();

  NODE_DEFAULT_FUNCTIONS(TransformDistortNode) // 节点默认功能宏，可能包含克隆、类型信息等标准实现

  /**
   * @brief 获取此节点的名称。
   * @return QString 类型的节点名称 (例如 "变换")，支持国际化翻译。
   */
  QString Name() const override { return tr("Transform"); }

  /**
   * @brief 获取此节点的短名称。
   *  重写了 MatrixGenerator 的 "Ortho" 短名称。
   * @return QString 类型的节点短名称，与 Name() 相同。
   */
  QString ShortName() const override {
    // Override MatrixGenerator's short name "Ortho"
    return Name();
  }

  /**
   * @brief 获取此节点的唯一标识符。
   * @return QString 类型的节点 ID (例如 "org.olivevideoeditor.Olive.transform")。
   */
  QString id() const override { return QStringLiteral("org.olivevideoeditor.Olive.transform"); }

  /**
   * @brief 获取此节点所属的分类 ID 列表。
   * @return CategoryID 的 QVector，表示此节点属于 "扭曲" (kCategoryDistort) 分类（变换操作常归于此类）。
   */
  QVector<CategoryID> Category() const override { return {kCategoryDistort}; }

  /**
   * @brief 获取此节点的描述信息。
   * @return QString 类型的节点描述 (例如 "在2D空间中变换图像。等效于乘以一个正交矩阵。")，支持国际化翻译。
   */
  QString Description() const override {
    return tr("Transform an image in 2D space. Equivalent to multiplying by an orthographic matrix.");
  }

  /**
   * @brief 当界面语言等需要重新翻译时调用，用于更新本地化文本 (如名称、描述)。
   */
  void Retranslate() override;

  /**
   * @brief 计算并输出节点在特定时间点的值（通常是变换后的图像，或者是变换矩阵本身，取决于如何使用）。
   * @param value 当前输入行数据 (包含输入图像及变换参数)。
   * @param globals 全局节点处理参数。
   * @param table 用于存储输出值的表 (包含输出的变换后图像或变换矩阵)。
   */
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  /**
   * @brief 获取用于应用此变换效果的着色器代码。
   * @param request 包含着色器请求参数的对象。
   * @return ShaderCode 对象，包含顶点和片段着色器代码，通常顶点着色器会使用生成的变换矩阵。
   */
  ShaderCode GetShaderCode(const ShaderRequest &request) const override;

  /**
   * @brief 定义自动缩放的类型。
   */
  enum AutoScaleType {
    kAutoScaleNone,    ///< 不进行自动缩放
    kAutoScaleFit,     ///< 自动缩放以适应目标分辨率 (保持宽高比，可能会有黑边)
    kAutoScaleFill,    ///< 自动缩放以填充目标分辨率 (保持宽高比，可能会裁剪)
    kAutoScaleStretch  ///< 自动缩放以拉伸至目标分辨率 (不保持宽高比)
  };

  /**
   * @brief (静态工具函数) 根据序列分辨率和纹理分辨率调整变换矩阵，应用自动缩放和偏移。
   * @param mat 原始的变换矩阵。
   * @param sequence_res 序列（画布）的分辨率。
   * @param texture_res 输入纹理（图像）的分辨率。
   * @param offset 应用于纹理的额外偏移量。
   * @param autoscale_type 自动缩放的类型，默认为不缩放。
   * @return QMatrix4x4 调整后的变换矩阵。
   */
  static QMatrix4x4 AdjustMatrixByResolutions(const QMatrix4x4 &mat, const QVector2D &sequence_res,
                                              const QVector2D &texture_res, const QVector2D &offset,
                                              AutoScaleType autoscale_type = kAutoScaleNone);

  /**
   * @brief 更新 Gizmo (交互控件，如控制点、旋转手柄) 在屏幕上的位置。
   * @param row 当前节点的参数值。
   * @param globals 全局节点处理参数。
   */
  void UpdateGizmoPositions(const NodeValueRow &row, const NodeGlobals &globals) override;
  /**
   * @brief 获取用于 Gizmo 显示的变换。
   * @param row 当前节点的参数值。
   * @param globals 全局节点处理参数。
   * @return QTransform Gizmo 的变换矩阵。
   */
  QTransform GizmoTransformation(const NodeValueRow &row, const NodeGlobals &globals) const override;

  // --- 静态常量，用作节点输入参数的键名 ---
  static const QString kParentInput;          ///< "ParentMatrix" - 父级变换矩阵的输入参数键名（用于层级变换）。
  static const QString kTextureInput;         ///< "Texture" - 输入纹理（图像）的参数键名。
  static const QString kAutoscaleInput;       ///< "Autoscale" - 自动缩放类型的参数键名。
  static const QString kInterpolationInput;   ///< "Interpolation" - 图像插值方式（如双线性、最近邻）的参数键名。

 protected slots:
  /**
   * @brief 当 Gizmo 拖动开始时调用的槽函数。
   * @param row 当前节点的参数值。
   * @param x 拖动开始时的 X 坐标。
   * @param y 拖动开始时的 Y 坐标。
   * @param time 当前时间。
   */
  void GizmoDragStart(const olive::NodeValueRow &row, double x, double y, const olive::rational &time) override;

  /**
   * @brief 当 Gizmo 被拖动时调用的槽函数。
   * @param x Gizmo 在画布上的新 X 坐标。
   * @param y Gizmo 在画布上的新 Y 坐标。
   * @param modifiers 当前按下的键盘修饰键 (如 Shift, Ctrl)。
   */
  void GizmoDragMove(double x, double y, const Qt::KeyboardModifiers &modifiers) override;

 private:
  /**
   * @brief (静态工具函数) 根据给定的 X, Y 偏移、半分辨率和变换矩阵，创建一个缩放控制点。
   * @param x X方向偏移。
   * @param y Y方向偏移。
   * @param half_res 图像半分辨率 (宽/2, 高/2)。
   * @param mat 当前变换矩阵。
   * @return QPointF 计算得到的缩放控制点在图像空间中的位置。
   */
  static QPointF CreateScalePoint(double x, double y, const QPointF &half_res, const QMatrix4x4 &mat);

  /**
   * @brief (静态工具函数) 根据当前参数和纹理信息，生成应用了自动缩放的变换矩阵。
   * @param generated_matrix 由节点参数（如平移、旋转、缩放）直接生成的原始变换矩阵。
   * @param value 当前节点的参数值。
   * @param globals 全局节点处理参数。
   * @param texture_params 输入纹理的参数（包含分辨率等）。
   * @return QMatrix4x4 应用自动缩放后的最终变换矩阵。
   */
  static QMatrix4x4 GenerateAutoScaledMatrix(const QMatrix4x4 &generated_matrix, const NodeValueRow &value,
                                             const NodeGlobals &globals, const VideoParams &texture_params);

  /**
   * @brief 检查给定的 Gizmo 是否是用于缩放操作的 Gizmo。
   * @param g 指向 NodeGizmo 对象的指针。
   * @return 如果是缩放 Gizmo 则返回 true，否则返回 false。
   */
  bool IsAScaleGizmo(NodeGizmo *g) const;

  // --- Gizmo 相关私有成员变量 ---
  double gizmo_start_angle_{};           ///< Gizmo 拖动开始时的旋转角度。
  QTransform gizmo_inverted_transform_;  ///< Gizmo 拖动开始时的逆变换矩阵，用于将屏幕坐标转换回对象坐标。
  QPointF gizmo_anchor_pt_;              ///< Gizmo 拖动时的锚点（通常是变换中心或相反的控制点）。
  bool gizmo_scale_uniform_{};           ///< Gizmo 拖动时是否进行等比缩放。
  double gizmo_last_angle_{};            ///< 上一次记录的 Gizmo 旋转角度。
  double gizmo_last_alt_angle_{};        ///< 上一次记录的 Gizmo 备用旋转角度（可能用于处理多圈旋转）。
  int gizmo_rotate_wrap_{};              ///< Gizmo 旋转的圈数计数。

  /**
   * @brief 定义 Gizmo 旋转方向的枚举。
   */
  enum RotationDirection {
    kDirectionNone,     ///< 无方向或未确定
    kDirectionPositive,  ///< 正方向 (通常为顺时针)
    kDirectionNegative   ///< 负方向 (通常为逆时针)
  };

  /**
   * @brief (静态工具函数) 根据上一个角度和当前角度判断旋转方向。
   * @param last 上一个角度。
   * @param current 当前角度。
   * @return RotationDirection 枚举值。
   */
  static RotationDirection GetDirectionFromAngles(double last, double current);
  RotationDirection gizmo_rotate_last_dir_;      ///< Gizmo 上一次的主要旋转方向。
  RotationDirection gizmo_rotate_last_alt_dir_;  ///< Gizmo 上一次的备用旋转方向。

  /**
   * @brief 定义 Gizmo 缩放操作的类型。
   */
  enum GizmoScaleType {
    kGizmoScaleXOnly, ///< 仅在 X 轴方向缩放
    kGizmoScaleYOnly, ///< 仅在 Y 轴方向缩放
    kGizmoScaleBoth   ///< 在 X 和 Y 轴方向同时缩放（可能等比或不等比）
  };

  GizmoScaleType gizmo_scale_axes_; ///< 当前 Gizmo 缩放操作的类型。
  QVector2D gizmo_scale_anchor_;    ///< Gizmo 缩放操作的锚点（通常是与拖动点相对的角点或边）。

  // --- Gizmo 屏幕对象存储 ---
  // static const int kGizmoScaleCount = 8; // 假设 kGizmoScaleCount 在 .cpp 文件中定义，表示缩放/边框控制点数量
  PointGizmo *point_gizmo_[/*kGizmoScaleCount*/8]{}; ///< 指向各个缩放/边框控制点 Gizmo (PointGizmo) 的指针数组。
  PointGizmo *anchor_gizmo_;                         ///< 指向锚点 Gizmo (PointGizmo) 的指针。
  PolygonGizmo *poly_gizmo_;                         ///< 指向代表整个可交互边界框的 Gizmo (PolygonGizmo) 的指针。
  ScreenGizmo *rotation_gizmo_;                      ///< 指向用于旋转操作的 Gizmo (ScreenGizmo) 的指针。
};

}  // namespace olive

#endif  // TRANSFORMDISTORTNODE_H