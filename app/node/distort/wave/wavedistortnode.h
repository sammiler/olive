#ifndef WAVEDISTORTNODE_H
#define WAVEDISTORTNODE_H

#include "node/node.h"

namespace olive {

class WaveDistortNode : public Node {
  Q_OBJECT
 public:
  WaveDistortNode();

  NODE_DEFAULT_FUNCTIONS(WaveDistortNode)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  static const QString kTextureInput;
  static const QString kFrequencyInput;
  static const QString kIntensityInput;
  static const QString kEvolutionInput;
  static const QString kVerticalInput;
};

}  // namespace olive

#endif  // WAVEDISTORTNODE_H
