#include "merge.h"

#include "node/traverser.h"

namespace olive {

const QString MergeNode::kBaseIn = QStringLiteral("base_in");
const QString MergeNode::kBlendIn = QStringLiteral("blend_in");

#define super Node

MergeNode::MergeNode() {
  AddInput(kBaseIn, NodeValue::kTexture, InputFlags(kInputFlagNotKeyframable));

  AddInput(kBlendIn, NodeValue::kTexture, InputFlags(kInputFlagNotKeyframable));

  SetFlag(kDontShowInParamView);
}

QString MergeNode::Name() const { return tr("Merge"); }

QString MergeNode::id() const { return QStringLiteral("org.olivevideoeditor.Olive.merge"); }

QVector<Node::CategoryID> MergeNode::Category() const { return {kCategoryMath}; }

QString MergeNode::Description() const { return tr("Merge two textures together."); }

void MergeNode::Retranslate() {
  super::Retranslate();

  SetInputName(kBaseIn, tr("Base"));

  SetInputName(kBlendIn, tr("Blend"));
}

ShaderCode MergeNode::GetShaderCode(const ShaderRequest &request) const {
  Q_UNUSED(request)

  return ShaderCode(FileFunctions::ReadFileAsString(":/shaders/alphaover.frag"));
}

void MergeNode::Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const {
  TexturePtr base_tex = value[kBaseIn].toTexture();
  TexturePtr blend_tex = value[kBlendIn].toTexture();

  if (base_tex || blend_tex) {
    if (!base_tex || (blend_tex && blend_tex->channel_count() < VideoParams::kRGBAChannelCount)) {
      // We only have a blend texture or the blend texture is RGB only, no need to alpha over
      table->Push(value[kBlendIn]);
    } else if (!blend_tex) {
      // We only have a base texture, no need to alpha over
      table->Push(value[kBaseIn]);
    } else {
      table->Push(NodeValue::kTexture, base_tex->toJob(ShaderJob(value)), this);
    }
  }
}

}  // namespace olive
