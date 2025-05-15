

#ifndef PANNODE_H
#define PANNODE_H

#include "node/node.h"

namespace olive {

class PanNode : public Node {
  Q_OBJECT
 public:
  PanNode();

  NODE_DEFAULT_FUNCTIONS(PanNode)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  void ProcessSamples(const NodeValueRow &values, const SampleBuffer &input, SampleBuffer &output,
                      int index) const override;

  void Retranslate() override;

  static const QString kSamplesInput;
  static const QString kPanningInput;

 private:
  NodeInput *samples_input_{};
  NodeInput *panning_input_{};
};

}  // namespace olive

#endif  // PANNODE_H
