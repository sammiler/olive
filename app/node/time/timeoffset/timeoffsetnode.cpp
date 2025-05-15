#include "timeoffsetnode.h"

#include "widget/slider/rationalslider.h"

namespace olive {

const QString TimeOffsetNode::kTimeInput = QStringLiteral("time_in");
const QString TimeOffsetNode::kInputInput = QStringLiteral("input_in");

#define super Node

TimeOffsetNode::TimeOffsetNode() {
  AddInput(kTimeInput, NodeValue::kRational, QVariant::fromValue(rational(0)), InputFlags(kInputFlagNotConnectable));
  SetInputProperty(kTimeInput, QStringLiteral("view"), RationalSlider::kTime);
  SetInputProperty(kTimeInput, QStringLiteral("viewlock"), true);

  AddInput(kInputInput, NodeValue::kNone, InputFlags(kInputFlagNotKeyframable));
}

void TimeOffsetNode::Retranslate() {
  super::Retranslate();

  SetInputName(kTimeInput, QStringLiteral("Time"));
  SetInputName(kInputInput, QStringLiteral("Input"));
}

TimeRange TimeOffsetNode::InputTimeAdjustment(const QString &input, int element, const TimeRange &input_time,
                                              bool clamp) const {
  if (input == kInputInput) {
    return TimeRange(GetRemappedTime(input_time.in()), GetRemappedTime(input_time.out()));
  } else {
    return super::InputTimeAdjustment(input, element, input_time, clamp);
  }
}

TimeRange TimeOffsetNode::OutputTimeAdjustment(const QString &input, int element, const TimeRange &input_time) const {
  /*if (input == kInputInput) {
    rational target_time = GetValueAtTime(kTimeInput, input_time.in()).value<rational>();

    return TimeRange(target_time, target_time + input_time.length());
  } else {
    return super::OutputTimeAdjustment(input, element, input_time);
  }*/
  return super::OutputTimeAdjustment(input, element, input_time);
}

void TimeOffsetNode::Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const {
  table->Push(value[kInputInput]);
}

rational TimeOffsetNode::GetRemappedTime(const rational &input) const {
  return input + GetValueAtTime(kTimeInput, input).value<rational>();
}

}  // namespace olive
