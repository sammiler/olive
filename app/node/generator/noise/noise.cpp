#include "noise.h"

#include "widget/slider/floatslider.h"

namespace olive {

const QString NoiseGeneratorNode::kBaseIn = QStringLiteral("base_in");
const QString NoiseGeneratorNode::kColorInput = QStringLiteral("color_in");
const QString NoiseGeneratorNode::kStrengthInput = QStringLiteral("strength_in");

#define super Node

NoiseGeneratorNode::NoiseGeneratorNode() {
  AddInput(kBaseIn, NodeValue::kTexture, InputFlags(kInputFlagNotKeyframable));

  AddInput(kStrengthInput, NodeValue::kFloat, 0.2);
  SetInputProperty(kStrengthInput, QStringLiteral("view"), FloatSlider::kPercentage);
  SetInputProperty(kStrengthInput, QStringLiteral("min"), 0);

  AddInput(kColorInput, NodeValue::kBoolean, false);

  SetEffectInput(kBaseIn);
  SetFlag(kVideoEffect);
}

QString NoiseGeneratorNode::Name() const { return tr("Noise"); }

QString NoiseGeneratorNode::id() const { return QStringLiteral("org.olivevideoeditor.Olive.noise"); }

QVector<Node::CategoryID> NoiseGeneratorNode::Category() const { return {kCategoryGenerator}; }

QString NoiseGeneratorNode::Description() const { return tr("Generates noise patterns"); }

void NoiseGeneratorNode::Retranslate() {
  super::Retranslate();

  SetInputName(kBaseIn, tr("Base"));
  SetInputName(kStrengthInput, tr("Strength"));
  SetInputName(kColorInput, tr("Color"));
}

ShaderCode NoiseGeneratorNode::GetShaderCode(const ShaderRequest &request) const {
  return ShaderCode(FileFunctions::ReadFileAsString(":/shaders/noise.frag"));
}

void NoiseGeneratorNode::Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const {
  ShaderJob job(value);

  job.Insert(value);
  job.Insert(QStringLiteral("time_in"), NodeValue(NodeValue::kFloat, globals.time().in().toDouble(), this));

  TexturePtr base = value[kBaseIn].toTexture();

  table->Push(NodeValue::kTexture, Texture::Job(base ? base->params() : globals.vparams(), job), this);
}
}  // namespace olive
