

#ifndef BLURFILTERNODE_H
#define BLURFILTERNODE_H

#include "node/gizmo/point.h"
#include "node/node.h"

namespace olive {

class BlurFilterNode : public Node {
  Q_OBJECT
 public:
  BlurFilterNode();

  enum Method { kBox, kGaussian, kDirectional, kRadial };

  NODE_DEFAULT_FUNCTIONS(BlurFilterNode)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  [[nodiscard]] Method GetMethod() const { return static_cast<Method>(GetStandardValue(kMethodInput).toInt()); }

  void UpdateGizmoPositions(const NodeValueRow &row, const NodeGlobals &globals) override;

  static const QString kTextureInput;
  static const QString kMethodInput;
  static const QString kRadiusInput;
  static const QString kHorizInput;
  static const QString kVertInput;
  static const QString kRepeatEdgePixelsInput;

  static const QString kDirectionalDegreesInput;

  static const QString kRadialCenterInput;

 protected slots:
  void GizmoDragMove(double x, double y, const Qt::KeyboardModifiers &modifiers) override;

 protected:
  void InputValueChangedEvent(const QString &input, int element) override;

 private:
  void UpdateInputs(Method method);

  PointGizmo *radial_center_gizmo_;
};

}  // namespace olive

#endif  // BLURFILTERNODE_H
