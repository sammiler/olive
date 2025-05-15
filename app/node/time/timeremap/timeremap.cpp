#include "timeremap.h"

#include "widget/slider/rationalslider.h"

namespace olive {

const QString TimeRemapNode::kTimeInput = QStringLiteral("time_in");
const QString TimeRemapNode::kInputInput = QStringLiteral("input_in");

#define super Node

TimeRemapNode::TimeRemapNode() {
  AddInput(kTimeInput, NodeValue::kRational, QVariant::fromValue(rational(0)), InputFlags(kInputFlagNotConnectable));
  SetInputProperty(kTimeInput, QStringLiteral("view"), RationalSlider::kTime);
  SetInputProperty(kTimeInput, QStringLiteral("viewlock"), true);

  AddInput(kInputInput, NodeValue::kNone, InputFlags(kInputFlagNotKeyframable));
}

QString TimeRemapNode::Name() const { return tr("Time Remap"); }

QString TimeRemapNode::id() const { return QStringLiteral("org.olivevideoeditor.Olive.timeremap"); }

QVector<Node::CategoryID> TimeRemapNode::Category() const { return {kCategoryTime}; }

QString TimeRemapNode::Description() const { return tr("Arbitrarily remap time through the nodes."); }

TimeRange TimeRemapNode::InputTimeAdjustment(const QString &input, int element, const TimeRange &input_time,
                                             bool clamp) const {
  if (input == kInputInput) {
    return TimeRange(GetRemappedTime(input_time.in()), GetRemappedTime(input_time.out()));
  } else {
    return super::InputTimeAdjustment(input, element, input_time, clamp);
  }
}

TimeRange TimeRemapNode::OutputTimeAdjustment(const QString &input, int element, const TimeRange &input_time) const {
  /*if (input == kInputInput) {
    rational target_time = GetValueAtTime(kTimeInput, input_time.in()).value<rational>();

    return TimeRange(target_time, target_time + input_time.length());
  } else {
    return super::OutputTimeAdjustment(input, element, input_time);
  }*/
  return super::OutputTimeAdjustment(input, element, input_time);
}

void TimeRemapNode::Retranslate() {
  super::Retranslate();

  SetInputName(kTimeInput, QStringLiteral("Time"));
  SetInputName(kInputInput, QStringLiteral("Input"));
}

void TimeRemapNode::Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const {
  table->Push(value[kInputInput]);
}

rational TimeRemapNode::GetRemappedTime(const rational &input) const {
  return GetValueAtTime(kTimeInput, input).value<rational>();
}

}  // namespace olive
