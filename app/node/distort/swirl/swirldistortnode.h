

#ifndef SWIRLDISTORTNODE_H
#define SWIRLDISTORTNODE_H

#include "node/gizmo/point.h"
#include "node/node.h"

namespace olive {

class SwirlDistortNode : public Node {
  Q_OBJECT
 public:
  SwirlDistortNode();

  NODE_DEFAULT_FUNCTIONS(SwirlDistortNode)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  void UpdateGizmoPositions(const NodeValueRow &row, const NodeGlobals &globals) override;

  static const QString kTextureInput;
  static const QString kRadiusInput;
  static const QString kAngleInput;
  static const QString kPositionInput;

 protected slots:
  void GizmoDragMove(double x, double y, const Qt::KeyboardModifiers &modifiers) override;

 private:
  PointGizmo *gizmo_;
};

}  // namespace olive

#endif  // SWIRLDISTORTNODE_H
