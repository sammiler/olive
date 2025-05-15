#ifndef POLYGONGENERATOR_H
#define POLYGONGENERATOR_H

#include <QPainterPath>

#include "node/generator/shape/generatorwithmerge.h"
#include "node/gizmo/line.h"
#include "node/gizmo/path.h"
#include "node/gizmo/point.h"
#include "node/inputdragger.h"
#include "node/node.h"

namespace olive {

class PolygonGenerator : public GeneratorWithMerge {
  Q_OBJECT
 public:
  PolygonGenerator();

  NODE_DEFAULT_FUNCTIONS(PolygonGenerator)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  void GenerateFrame(FramePtr frame, const GenerateJob &job) const override;

  void UpdateGizmoPositions(const NodeValueRow &row, const NodeGlobals &globals) override;

  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

  static const QString kPointsInput;
  static const QString kColorInput;

 protected:
  [[nodiscard]] ShaderJob GetGenerateJob(const NodeValueRow &value, const VideoParams &params) const;

 protected slots:
  void GizmoDragMove(double x, double y, const Qt::KeyboardModifiers &modifiers) override;

 private:
  static void AddPointToPath(QPainterPath *path, const Bezier &before, const Bezier &after);

  static QPainterPath GeneratePath(const NodeValueArray &points, int size);

  template <typename T>
  void ValidateGizmoVectorSize(QVector<T *> &vec, int new_sz);

  template <typename T>
  NodeGizmo *CreateAppropriateGizmo();

  PathGizmo *poly_gizmo_;
  QVector<PointGizmo *> gizmo_position_handles_;
  QVector<PointGizmo *> gizmo_bezier_handles_;
  QVector<LineGizmo *> gizmo_bezier_lines_;
};

}  // namespace olive

#endif  // POLYGONGENERATOR_H
