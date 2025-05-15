#include "valuenode.h"

namespace olive {

const QString ValueNode::kTypeInput = QStringLiteral("type_in");
const QString ValueNode::kValueInput = QStringLiteral("value_in");
const QVector<NodeValue::Type> ValueNode::kSupportedTypes = {
    NodeValue::kFloat, NodeValue::kInt,  NodeValue::kRational, NodeValue::kVec2, NodeValue::kVec3,    NodeValue::kVec4,
    NodeValue::kColor, NodeValue::kText, NodeValue::kMatrix,   NodeValue::kFont, NodeValue::kBoolean,
};

#define super Node

ValueNode::ValueNode() {
  AddInput(kTypeInput, NodeValue::kCombo, 0, InputFlags(kInputFlagNotConnectable | kInputFlagNotKeyframable));

  AddInput(kValueInput, kSupportedTypes.first(), QVariant(), InputFlags(kInputFlagNotConnectable));
}

void ValueNode::Retranslate() {
  super::Retranslate();

  SetInputName(kTypeInput, QStringLiteral("Type"));
  SetInputName(kValueInput, QStringLiteral("Value"));

  QStringList type_names;
  type_names.reserve(kSupportedTypes.size());
  foreach (NodeValue::Type type, kSupportedTypes) {
    type_names.append(NodeValue::GetPrettyDataTypeName(type));
  }
  SetComboBoxStrings(kTypeInput, type_names);
}

void ValueNode::Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const {
  Q_UNUSED(globals)

  // Ensure value is pushed onto the table
  table->Push(value[kValueInput]);
}

void ValueNode::InputValueChangedEvent(const QString &input, int element) {
  if (input == kTypeInput) {
    SetInputDataType(kValueInput, kSupportedTypes.at(GetStandardValue(kTypeInput).toInt()));
  }

  super::InputValueChangedEvent(input, element);
}

}  // namespace olive
