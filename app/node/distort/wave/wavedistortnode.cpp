

#include "wavedistortnode.h"

namespace olive {

const QString WaveDistortNode::kTextureInput = QStringLiteral("tex_in");
const QString WaveDistortNode::kFrequencyInput = QStringLiteral("frequency_in");
const QString WaveDistortNode::kIntensityInput = QStringLiteral("intensity_in");
const QString WaveDistortNode::kEvolutionInput = QStringLiteral("evolution_in");
const QString WaveDistortNode::kVerticalInput = QStringLiteral("vertical_in");

#define super Node

WaveDistortNode::WaveDistortNode() {
  AddInput(kTextureInput, NodeValue::kTexture, InputFlags(kInputFlagNotKeyframable));

  AddInput(kFrequencyInput, NodeValue::kFloat, 10);
  AddInput(kIntensityInput, NodeValue::kFloat, 10);
  AddInput(kEvolutionInput, NodeValue::kFloat, 0);

  AddInput(kVerticalInput, NodeValue::kCombo, false);

  SetFlag(kVideoEffect);
  SetEffectInput(kTextureInput);
}

QString WaveDistortNode::Name() const { return tr("Wave"); }

QString WaveDistortNode::id() const { return QStringLiteral("org.oliveeditor.Olive.wave"); }

QVector<Node::CategoryID> WaveDistortNode::Category() const { return {kCategoryDistort}; }

QString WaveDistortNode::Description() const { return tr("Distorts an image along a sine wave."); }

void WaveDistortNode::Retranslate() {
  super::Retranslate();

  SetInputName(kTextureInput, tr("Input"));
  SetInputName(kFrequencyInput, tr("Frequency"));
  SetInputName(kIntensityInput, tr("Intensity"));
  SetInputName(kEvolutionInput, tr("Evolution"));
  SetInputName(kVerticalInput, tr("Direction"));
  SetComboBoxStrings(kVerticalInput, {tr("Horizontal"), tr("Vertical")});
}

ShaderCode WaveDistortNode::GetShaderCode(const ShaderRequest &request) const {
  Q_UNUSED(request)
  return ShaderCode(FileFunctions::ReadFileAsString(":/shaders/wave.frag"));
}

void WaveDistortNode::Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const {
  // If there's no texture, no need to run an operation
  if (TexturePtr texture = value[kTextureInput].toTexture()) {
    // Only run shader if at least one of flip or flop are selected
    if (!qIsNull(value[kIntensityInput].toDouble())) {
      table->Push(NodeValue::kTexture, Texture::Job(texture->params(), ShaderJob(value)), this);
    } else {
      // If we're not flipping or flopping just push the texture
      table->Push(value[kTextureInput]);
    }
  }
}

}  // namespace olive
