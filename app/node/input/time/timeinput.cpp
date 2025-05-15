

#include "timeinput.h"

namespace olive {

#define super Node

TimeInput::TimeInput() = default;

QString TimeInput::Name() const { return tr("Time"); }

QString TimeInput::id() const { return QStringLiteral("org.olivevideoeditor.Olive.time"); }

QVector<Node::CategoryID> TimeInput::Category() const { return {kCategoryTime}; }

QString TimeInput::Description() const { return tr("Generates the time (in seconds) at this frame."); }

void TimeInput::Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const {
  table->Push(NodeValue::kFloat, globals.time().in().toDouble(), this, false, QStringLiteral("time"));
}

}  // namespace olive
