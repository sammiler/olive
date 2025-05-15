

#ifndef NOISEGENERATORNODE_H
#define NOISEGENERATORNODE_H

#include "node/node.h"

namespace olive {

class NoiseGeneratorNode : public Node {
  Q_OBJECT
 public:
  NoiseGeneratorNode();

  NODE_DEFAULT_FUNCTIONS(NoiseGeneratorNode)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  static const QString kBaseIn;
  static const QString kColorInput;
  static const QString kStrengthInput;
};

}  // namespace olive

#endif  // NOISEGENERATORNODE_H
