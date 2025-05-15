

#ifndef TIMEOFFSETNODE_H
#define TIMEOFFSETNODE_H

#include "node/node.h"

namespace olive {

class TimeOffsetNode : public Node {
 public:
  TimeOffsetNode();

  NODE_DEFAULT_FUNCTIONS(TimeOffsetNode)

  [[nodiscard]] QString Name() const override { return tr("Time Offset"); }

  [[nodiscard]] QString id() const override { return QStringLiteral("org.olivevideoeditor.Olive.timeoffset"); }

  [[nodiscard]] QVector<CategoryID> Category() const override { return {kCategoryTime}; }

  [[nodiscard]] QString Description() const override { return tr("Offset time passing through the graph."); }

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

#endif  // TIMEOFFSETNODE_H
