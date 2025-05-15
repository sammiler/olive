

#include "timeformat.h"

#include <QDateTime>

namespace olive {

#define super Node

const QString TimeFormatNode::kTimeInput = QStringLiteral("time_in");
const QString TimeFormatNode::kFormatInput = QStringLiteral("format_in");
const QString TimeFormatNode::kLocalTimeInput = QStringLiteral("localtime_in");

TimeFormatNode::TimeFormatNode() {
  AddInput(kTimeInput, NodeValue::kFloat);
  AddInput(kFormatInput, NodeValue::kText, QStringLiteral("hh:mm:ss"));
  AddInput(kLocalTimeInput, NodeValue::kBoolean);
}

QString TimeFormatNode::Name() const { return tr("Time Format"); }

QString TimeFormatNode::id() const { return QStringLiteral("org.olivevideoeditor.Olive.timeformat"); }

QVector<Node::CategoryID> TimeFormatNode::Category() const { return {kCategoryGenerator}; }

QString TimeFormatNode::Description() const { return tr("Format time (in Unix epoch seconds) into a string."); }

void TimeFormatNode::Retranslate() {
  super::Retranslate();

  SetInputName(kTimeInput, tr("Time"));
  SetInputName(kFormatInput, tr("Format"));
  SetInputName(kLocalTimeInput, tr("Interpret time as local time"));
}

void TimeFormatNode::Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const {
  qint64 ms_since_epoch = value[kTimeInput].toDouble() * 1000;
  bool time_is_local = value[kLocalTimeInput].toBool();
  QDateTime dt = QDateTime::fromMSecsSinceEpoch(ms_since_epoch, time_is_local ? Qt::LocalTime : Qt::UTC);
  QString format = value[kFormatInput].toString();
  QString output = dt.toString(format);
  table->Push(NodeValue(NodeValue::kText, output, this));
}

}  // namespace olive
