

#ifndef TIMEINPUT_H
#define TIMEINPUT_H

#include "node/node.h"

namespace olive {

class TimeInput : public Node {
  Q_OBJECT
 public:
  TimeInput();

  NODE_DEFAULT_FUNCTIONS(TimeInput)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;
};

}  // namespace olive

#endif  // TIMEINPUT_H
