

#ifndef CROPDISTORTNODE_H
#define CROPDISTORTNODE_H

#include <QVector2D>

#include "node/gizmo/point.h"
#include "node/gizmo/polygon.h"
#include "node/inputdragger.h"
#include "node/node.h"

namespace olive {

class CropDistortNode : public Node {
  Q_OBJECT
 public:
  CropDistortNode();

  NODE_DEFAULT_FUNCTIONS(CropDistortNode)

  [[nodiscard]] QString Name() const override { return tr("Crop"); }

  [[nodiscard]] QString id() const override { return QStringLiteral("org.olivevideoeditor.Olive.crop"); }

  [[nodiscard]] QVector<CategoryID> Category() const override { return {kCategoryDistort}; }

  [[nodiscard]] QString Description() const override { return tr("Crop the edges of an image."); }

  void Retranslate() override;

  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

  void UpdateGizmoPositions(const NodeValueRow &row, const NodeGlobals &globals) override;

  static const QString kTextureInput;
  static const QString kLeftInput;
  static const QString kTopInput;
  static const QString kRightInput;
  static const QString kBottomInput;
  static const QString kFeatherInput;

 protected slots:
  void GizmoDragMove(double x_diff, double y_diff, const Qt::KeyboardModifiers &modifiers) override;

 private:
  void CreateCropSideInput(const QString &id);

  // Gizmo variables
  PointGizmo *point_gizmo_[kGizmoScaleCount]{};
  PolygonGizmo *poly_gizmo_;
  QVector2D temp_resolution_;
};

}  // namespace olive

#endif  // CROPDISTORTNODE_H
