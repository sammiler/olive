#ifndef TIMEFORMAT_H
#define TIMEFORMAT_H

#include "node/node.h"

namespace olive {

class TimeFormatNode : public Node {
  Q_OBJECT
 public:
  TimeFormatNode();

  NODE_DEFAULT_FUNCTIONS(TimeFormatNode)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  static const QString kTimeInput;
  static const QString kFormatInput;
  static const QString kLocalTimeInput;
};

}  // namespace olive

#endif  // TIMEFORMAT_H
