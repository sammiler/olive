

#ifndef MATHNODE_H
#define MATHNODE_H

#include "mathbase.h"

namespace olive {

class MathNode : public MathNodeBase {
  Q_OBJECT
 public:
  MathNode();

  NODE_DEFAULT_FUNCTIONS(MathNode)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

  [[nodiscard]] Operation GetOperation() const { return static_cast<Operation>(GetStandardValue(kMethodIn).toInt()); }

  void SetOperation(Operation o) { SetStandardValue(kMethodIn, o); }

  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  void ProcessSamples(const NodeValueRow &values, const SampleBuffer &input, SampleBuffer &output,
                      int index) const override;

  static const QString kMethodIn;
  static const QString kParamAIn;
  static const QString kParamBIn;
  static const QString kParamCIn;
};

}  // namespace olive

#endif  // MATHNODE_H
