#ifndef SOLIDGENERATOR_H
#define SOLIDGENERATOR_H

#include "node/node.h"

namespace olive {

class SolidGenerator : public Node {
  Q_OBJECT
 public:
  SolidGenerator();

  NODE_DEFAULT_FUNCTIONS(SolidGenerator)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

  static const QString kColorInput;
};

}  // namespace olive

#endif  // SOLIDGENERATOR_H
