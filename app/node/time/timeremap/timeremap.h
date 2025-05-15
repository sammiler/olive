

#ifndef TIMEREMAPNODE_H
#define TIMEREMAPNODE_H

#include "node/node.h"

namespace olive {

class TimeRemapNode : public Node {
  Q_OBJECT
 public:
  TimeRemapNode();

  NODE_DEFAULT_FUNCTIONS(TimeRemapNode)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  [[nodiscard]] TimeRange InputTimeAdjustment(const QString& input, int element, const TimeRange& input_time,
                                              bool clamp) const override;
  [[nodiscard]] TimeRange OutputTimeAdjustment(const QString& input, int element,
                                               const TimeRange& input_time) const override;

  void Retranslate() override;

  void Value(const NodeValueRow& value, const NodeGlobals& globals, NodeValueTable* table) const override;

  static const QString kTimeInput;
  static const QString kInputInput;

 private:
  [[nodiscard]] rational GetRemappedTime(const rational& input) const;
};

}  // namespace olive

#endif  // TIMEREMAPNODE_H
