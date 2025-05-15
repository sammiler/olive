#ifndef MERGENODE_H
#define MERGENODE_H

#include "node/node.h"

namespace olive {

class MergeNode : public Node {
  Q_OBJECT
 public:
  MergeNode();

  NODE_DEFAULT_FUNCTIONS(MergeNode)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest& request) const override;
  void Value(const NodeValueRow& value, const NodeGlobals& globals, NodeValueTable* table) const override;

  static const QString kBaseIn;
  static const QString kBlendIn;

 private:
  NodeInput* base_in_{};

  NodeInput* blend_in_{};
};

}  // namespace olive

#endif  // MERGENODE_H
