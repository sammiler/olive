

#include "despill.h"

#include "node/project.h"

namespace olive {

const QString DespillNode::kTextureInput = QStringLiteral("tex_in");
const QString DespillNode::kColorInput = QStringLiteral("color_in");
const QString DespillNode::kMethodInput = QStringLiteral("method_in");
const QString DespillNode::kPreserveLuminanceInput = QStringLiteral("preserve_luminance_input");

#define super Node

DespillNode::DespillNode() {
  AddInput(kTextureInput, NodeValue::kTexture, InputFlags(kInputFlagNotKeyframable));

  AddInput(kColorInput, NodeValue::kCombo, 0);

  AddInput(kMethodInput, NodeValue::kCombo, 0);

  AddInput(kPreserveLuminanceInput, NodeValue::kBoolean, false);

  SetFlag(kVideoEffect);
  SetEffectInput(kTextureInput);
}

QString DespillNode::Name() const { return tr("Despill"); }

QString DespillNode::id() const { return QStringLiteral("org.olivevideoeditor.Olive.despill"); }

QVector<Node::CategoryID> DespillNode::Category() const { return {kCategoryKeying}; }

QString DespillNode::Description() const { return tr("Selection of simple depsill operations"); }

void DespillNode::Retranslate() {
  super::Retranslate();

  SetInputName(kTextureInput, tr("Input"));

  SetInputName(kColorInput, tr("Key Color"));
  SetComboBoxStrings(kColorInput, {tr("Green"), tr("Blue")});

  SetInputName(kMethodInput, tr("Method"));
  SetComboBoxStrings(kMethodInput, {tr("Average"), tr("Double Red Average"), tr("Double Average"), tr("Limit")});

  SetInputName(kPreserveLuminanceInput, tr("Preserve Luminance"));
}

ShaderCode DespillNode::GetShaderCode(const ShaderRequest &request) const {
  Q_UNUSED(request)
  return ShaderCode(FileFunctions::ReadFileAsString(":/shaders/despill.frag"));
}

void DespillNode::Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const {
  ShaderJob job;
  job.Insert(value);

  // Set luma coefficients
  double luma_coeffs[3] = {0.0f, 0.0f, 0.0f};
  project()->color_manager()->GetDefaultLumaCoefs(luma_coeffs);
  job.Insert(QStringLiteral("luma_coeffs"),
             NodeValue(NodeValue::kVec3, QVector3D(luma_coeffs[0], luma_coeffs[1], luma_coeffs[2])));

  // If there's no texture, no need to run an operation
  if (TexturePtr tex = job.Get(kTextureInput).toTexture()) {
    table->Push(NodeValue::kTexture, tex->toJob(job), this);
  }
}

}  // namespace olive
