#ifndef FLIPDISTORTNODE_H
#define FLIPDISTORTNODE_H

#include "node/node.h"

namespace olive {

class FlipDistortNode : public Node {
  Q_OBJECT
 public:
  FlipDistortNode();

  NODE_DEFAULT_FUNCTIONS(FlipDistortNode)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  static const QString kTextureInput;
  static const QString kHorizontalInput;
  static const QString kVerticalInput;
};

}  // namespace olive

#endif  // FLIPDISTORTNODE_H
