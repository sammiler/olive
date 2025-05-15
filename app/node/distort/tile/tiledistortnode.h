#ifndef TILEDISTORTNODE_H
#define TILEDISTORTNODE_H

#include "node/gizmo/point.h"
#include "node/node.h"

namespace olive {

class TileDistortNode : public Node {
  Q_OBJECT
 public:
  TileDistortNode();

  NODE_DEFAULT_FUNCTIONS(TileDistortNode)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  void UpdateGizmoPositions(const NodeValueRow &row, const NodeGlobals &globals) override;

  static const QString kTextureInput;
  static const QString kScaleInput;
  static const QString kPositionInput;
  static const QString kAnchorInput;
  static const QString kMirrorXInput;
  static const QString kMirrorYInput;

 protected slots:
  void GizmoDragMove(double x, double y, const Qt::KeyboardModifiers &modifiers) override;

 private:
  enum Anchor {
    kTopLeft,
    kTopCenter,
    kTopRight,
    kMiddleLeft,
    kMiddleCenter,
    kMiddleRight,
    kBottomLeft,
    kBottomCenter,
    kBottomRight
  };

  PointGizmo *gizmo_;
};

}  // namespace olive

#endif  // TILEDISTORTNODE_H
