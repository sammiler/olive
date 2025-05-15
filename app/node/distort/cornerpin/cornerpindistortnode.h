

#ifndef CORNERPINDISTORTNODE_H
#define CORNERPINDISTORTNODE_H

#include <QVector2D>

#include "node/gizmo/point.h"
#include "node/gizmo/polygon.h"
#include "node/inputdragger.h"
#include "node/node.h"

namespace olive {
class CornerPinDistortNode : public Node {
  Q_OBJECT
 public:
  CornerPinDistortNode();

  NODE_DEFAULT_FUNCTIONS(CornerPinDistortNode)

  [[nodiscard]] QString Name() const override { return tr("Corner Pin"); }

  [[nodiscard]] QString id() const override { return QStringLiteral("org.olivevideoeditor.Olive.cornerpin"); }

  [[nodiscard]] QVector<CategoryID> Category() const override { return {kCategoryDistort}; }

  [[nodiscard]] QString Description() const override { return tr("Distort the image by dragging the corners."); }

  void Retranslate() override;

  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

  void UpdateGizmoPositions(const NodeValueRow &row, const NodeGlobals &globals) override;

  /**
   * @brief Convenience function - converts the 2D slider values from being
   * an offset to the actual pixel value.
   */
  [[nodiscard]] static QPointF ValueToPixel(int value, const NodeValueRow &row, const QVector2D &resolution);

  static const QString kTextureInput;
  static const QString kPerspectiveInput;
  static const QString kTopLeftInput;
  static const QString kTopRightInput;
  static const QString kBottomRightInput;
  static const QString kBottomLeftInput;

 protected slots:
  void GizmoDragMove(double x, double y, const Qt::KeyboardModifiers &modifiers) override;

 private:
  // Gizmo variables
  static const int kGizmoCornerCount = 4;
  PointGizmo *gizmo_resize_handle_[kGizmoCornerCount]{};
  PolygonGizmo *gizmo_whole_rect_;
};

}  // namespace olive

#endif  // CORNERPINDISTORTNODE_H
