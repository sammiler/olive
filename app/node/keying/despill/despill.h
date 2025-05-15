

#ifndef DESPILLNODE_H
#define DESPILLNODE_H

#include "node/color/colormanager/colormanager.h"
#include "node/node.h"

namespace olive {

class DespillNode : public Node {
 public:
  DespillNode();

  NODE_DEFAULT_FUNCTIONS(DespillNode)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest& request) const override;
  void Value(const NodeValueRow& value, const NodeGlobals& globals, NodeValueTable* table) const override;

  static const QString kTextureInput;
  static const QString kColorInput;
  static const QString kMethodInput;
  static const QString kPreserveLuminanceInput;
};

}  // namespace olive

#endif  // DESPILLNODE_H
