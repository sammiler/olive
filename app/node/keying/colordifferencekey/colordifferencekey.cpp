#include "colordifferencekey.h"

namespace olive {

const QString ColorDifferenceKeyNode::kTextureInput = QStringLiteral("tex_in");
const QString ColorDifferenceKeyNode::kGarbageMatteInput = QStringLiteral("garbage_in");
const QString ColorDifferenceKeyNode::kCoreMatteInput = QStringLiteral("core_in");
const QString ColorDifferenceKeyNode::kColorInput = QStringLiteral("color_in");
const QString ColorDifferenceKeyNode::kShadowsInput = QStringLiteral("shadows_in");
const QString ColorDifferenceKeyNode::kHighlightsInput = QStringLiteral("highlights_in");
const QString ColorDifferenceKeyNode::kMaskOnlyInput = QStringLiteral("mask_only_in");

#define super Node

ColorDifferenceKeyNode::ColorDifferenceKeyNode() {
  AddInput(kTextureInput, NodeValue::kTexture, InputFlags(kInputFlagNotKeyframable));

  AddInput(kGarbageMatteInput, NodeValue::kTexture, InputFlags(kInputFlagNotKeyframable));

  AddInput(kCoreMatteInput, NodeValue::kTexture, InputFlags(kInputFlagNotKeyframable));

  AddInput(kColorInput, NodeValue::kCombo, 0);

  AddInput(kHighlightsInput, NodeValue::kFloat, 1.0f);
  SetInputProperty(kHighlightsInput, QStringLiteral("min"), 0.0);
  SetInputProperty(kHighlightsInput, QStringLiteral("base"), 0.01);

  AddInput(kShadowsInput, NodeValue::kFloat, 1.0f);
  SetInputProperty(kShadowsInput, QStringLiteral("min"), 0.0);
  SetInputProperty(kShadowsInput, QStringLiteral("base"), 0.01);

  AddInput(kMaskOnlyInput, NodeValue::kBoolean, false);

  SetFlag(kVideoEffect);
  SetEffectInput(kTextureInput);
}

QString ColorDifferenceKeyNode::Name() const { return tr("Color Difference Key"); }

QString ColorDifferenceKeyNode::id() const { return QStringLiteral("org.olivevideoeditor.Olive.colordifferencekey"); }

QVector<Node::CategoryID> ColorDifferenceKeyNode::Category() const { return {kCategoryKeying}; }

QString ColorDifferenceKeyNode::Description() const {
  return tr("A simple color key based on the distance of one color from other colors.");
}

void ColorDifferenceKeyNode::Retranslate() {
  super::Retranslate();

  SetInputName(kTextureInput, tr("Input"));
  SetInputName(kGarbageMatteInput, tr("Garbage Matte"));
  SetInputName(kCoreMatteInput, tr("Core Matte"));
  SetInputName(kColorInput, tr("Key Color"));
  SetComboBoxStrings(kColorInput, {tr("Green"), tr("Blue")});
  SetInputName(kShadowsInput, tr("Shadows"));
  SetInputName(kHighlightsInput, tr("Highlights"));
  SetInputName(kMaskOnlyInput, tr("Show Mask Only"));
}

ShaderCode ColorDifferenceKeyNode::GetShaderCode(const ShaderRequest &request) const {
  Q_UNUSED(request)
  return ShaderCode(FileFunctions::ReadFileAsString(":/shaders/colordifferencekey.frag"));
}

void ColorDifferenceKeyNode::Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const {
  // If there's no texture, no need to run an operation
  if (TexturePtr tex = value[kTextureInput].toTexture()) {
    ShaderJob job;
    job.Insert(value);
    table->Push(NodeValue::kTexture, tex->toJob(job), this);
  }
}

}  // namespace olive
