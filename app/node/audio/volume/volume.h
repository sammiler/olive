#ifndef VOLUMENODE_H
#define VOLUMENODE_H

#include "node/math/math/mathbase.h"

namespace olive {

class VolumeNode : public MathNodeBase {
  Q_OBJECT
 public:
  VolumeNode();

  NODE_DEFAULT_FUNCTIONS(VolumeNode)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  void ProcessSamples(const NodeValueRow &values, const SampleBuffer &input, SampleBuffer &output,
                      int index) const override;

  void Retranslate() override;

  static const QString kSamplesInput;
  static const QString kVolumeInput;
};

}  // namespace olive

#endif  // VOLUMENODE_H
