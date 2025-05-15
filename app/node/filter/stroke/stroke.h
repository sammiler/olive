#ifndef STROKEFILTERNODE_H
#define STROKEFILTERNODE_H

#include "node/node.h"

namespace olive {

class StrokeFilterNode : public Node {
  Q_OBJECT
 public:
  StrokeFilterNode();

  NODE_DEFAULT_FUNCTIONS(StrokeFilterNode)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

  static const QString kTextureInput;
  static const QString kColorInput;
  static const QString kRadiusInput;
  static const QString kOpacityInput;
  static const QString kInnerInput;
};

}  // namespace olive

#endif  // STROKEFILTERNODE_H
