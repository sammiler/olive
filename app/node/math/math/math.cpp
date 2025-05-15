

#include "math.h"

#include <cmath>

#include <QString>

namespace olive {

const QString MathNode::kMethodIn = QStringLiteral("method_in");
const QString MathNode::kParamAIn = QStringLiteral("param_a_in");
const QString MathNode::kParamBIn = QStringLiteral("param_b_in");
const QString MathNode::kParamCIn = QStringLiteral("param_c_in");

#define super MathNodeBase

MathNode::MathNode() {
  AddInput(kMethodIn, NodeValue::kCombo, InputFlags(kInputFlagNotConnectable | kInputFlagNotKeyframable));

  AddInput(kParamAIn, NodeValue::kFloat, 0.0);
  SetInputProperty(kParamAIn, QStringLiteral("decimalplaces"), 8);
  SetInputProperty(kParamAIn, QStringLiteral("autotrim"), true);

  AddInput(kParamBIn, NodeValue::kFloat, 0.0);
  SetInputProperty(kParamBIn, QStringLiteral("decimalplaces"), 8);
  SetInputProperty(kParamBIn, QStringLiteral("autotrim"), true);
}

QString MathNode::Name() const {
  // Default to naming after the operation
  if (parent()) {
    QString op_name = GetOperationName(GetOperation());
    if (!op_name.isEmpty()) {
      return op_name;
    }
  }

  return tr("Math");
}

QString MathNode::id() const { return QStringLiteral("org.olivevideoeditor.Olive.math"); }

QVector<Node::CategoryID> MathNode::Category() const { return {kCategoryMath}; }

QString MathNode::Description() const { return tr("Perform a mathematical operation between two values."); }

void MathNode::Retranslate() {
  super::Retranslate();

  SetInputName(kMethodIn, tr("Method"));
  SetInputName(kParamAIn, tr("Value"));
  SetInputName(kParamBIn, tr("Value"));

  QStringList operations = {GetOperationName(kOpAdd),
                            GetOperationName(kOpSubtract),
                            GetOperationName(kOpMultiply),
                            GetOperationName(kOpDivide),
                            QString(),
                            GetOperationName(kOpPower)};

  SetComboBoxStrings(kMethodIn, operations);
}

ShaderCode MathNode::GetShaderCode(const ShaderRequest &request) const {
  return GetShaderCodeInternal(request.id, kParamAIn, kParamBIn);
}

void MathNode::Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const {
  // Auto-detect what values to operate with
  // FIXME: Very inefficient
  NodeValueTable at, bt;
  at.Push(value[kParamAIn]);
  bt.Push(value[kParamBIn]);
  PairingCalculator calc(at, bt);

  // Do nothing if no pairing was found
  if (!calc.FoundMostLikelyPairing()) {
    return;
  }

  return ValueInternal(GetOperation(), calc.GetMostLikelyPairing(), kParamAIn, calc.GetMostLikelyValueA(), kParamBIn,
                       calc.GetMostLikelyValueB(), globals, table);
}

void MathNode::ProcessSamples(const NodeValueRow &values, const SampleBuffer &input, SampleBuffer &output,
                              int index) const {
  return ProcessSamplesInternal(values, GetOperation(), kParamAIn, kParamBIn, input, output, index);
}

}  // namespace olive
