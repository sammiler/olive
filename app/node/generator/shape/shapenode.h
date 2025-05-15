

#ifndef SHAPENODE_H
#define SHAPENODE_H

#include "shapenodebase.h"

namespace olive {

class ShapeNode : public ShapeNodeBase {
  Q_OBJECT
 public:
  ShapeNode();

  enum Type { kRectangle, kEllipse, kRoundedRectangle };

  NODE_DEFAULT_FUNCTIONS(ShapeNode)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  static QString kTypeInput;
  static QString kRadiusInput;

 protected:
  void InputValueChangedEvent(const QString &input, int element) override;
};

}  // namespace olive

#endif  // SHAPENODE_H
