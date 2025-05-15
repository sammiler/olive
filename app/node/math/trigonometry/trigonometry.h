

#ifndef TRIGNODE_H
#define TRIGNODE_H

#include "node/node.h"

namespace olive {

class TrigonometryNode : public Node {
  Q_OBJECT
 public:
  TrigonometryNode();

  NODE_DEFAULT_FUNCTIONS(TrigonometryNode)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  static const QString kMethodIn;
  static const QString kXIn;

 private:
  enum Operation {
    kOpSine,
    kOpCosine,
    kOpTangent,
    kOpArcSine,
    kOpArcCosine,
    kOpArcTangent,
    kOpHypSine,
    kOpHypCosine,
    kOpHypTangent
  };
};

}  // namespace olive

#endif  // TRIGNODE_H
