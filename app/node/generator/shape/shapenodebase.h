#ifndef SHAPENODEBASE_H
#define SHAPENODEBASE_H

#include "generatorwithmerge.h"
#include "node/gizmo/point.h"
#include "node/gizmo/polygon.h"
#include "node/inputdragger.h"
#include "node/node.h"

namespace olive {

class ShapeNodeBase : public GeneratorWithMerge {
  Q_OBJECT
 public:
  explicit ShapeNodeBase(bool create_color_input = true);

  void Retranslate() override;

  void UpdateGizmoPositions(const NodeValueRow &row, const NodeGlobals &globals) override;

  void SetRect(QRectF rect, const VideoParams &sequence_res, MultiUndoCommand *command);

  static const QString kPositionInput;
  static const QString kSizeInput;
  static const QString kColorInput;

 protected:
  [[nodiscard]] PolygonGizmo *poly_gizmo() const { return poly_gizmo_; }

 protected slots:
  void GizmoDragMove(double x, double y, const Qt::KeyboardModifiers &modifiers) override;

 private:
  QVector2D GenerateGizmoAnchor(const QVector2D &pos, const QVector2D &size, NodeGizmo *gizmo,
                                QVector2D *pt = nullptr) const;

  bool IsGizmoTop(NodeGizmo *g) const;
  bool IsGizmoBottom(NodeGizmo *g) const;
  bool IsGizmoLeft(NodeGizmo *g) const;
  bool IsGizmoRight(NodeGizmo *g) const;
  bool IsGizmoHorizontalCenter(NodeGizmo *g) const;
  bool IsGizmoVerticalCenter(NodeGizmo *g) const;
  bool IsGizmoCorner(NodeGizmo *g) const;

  // Gizmo variables
  static const int kGizmoWholeRect = kGizmoScaleCount;
  PointGizmo *point_gizmo_[kGizmoScaleCount]{};
  PolygonGizmo *poly_gizmo_;
};

}  // namespace olive

#endif  // SHAPENODEBASE_H
