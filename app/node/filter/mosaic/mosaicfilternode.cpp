#include "mosaicfilternode.h"

namespace olive {

const QString MosaicFilterNode::kTextureInput = QStringLiteral("tex_in");
const QString MosaicFilterNode::kHorizInput = QStringLiteral("horiz_in");
const QString MosaicFilterNode::kVertInput = QStringLiteral("vert_in");

#define super Node

MosaicFilterNode::MosaicFilterNode() {
  AddInput(kTextureInput, NodeValue::kTexture, InputFlags(kInputFlagNotKeyframable));

  AddInput(kHorizInput, NodeValue::kFloat, 32.0);
  SetInputProperty(kHorizInput, QStringLiteral("min"), 1.0);

  AddInput(kVertInput, NodeValue::kFloat, 18.0);
  SetInputProperty(kVertInput, QStringLiteral("min"), 1.0);

  SetFlag(kVideoEffect);
  SetEffectInput(kTextureInput);
}

void MosaicFilterNode::Retranslate() {
  super::Retranslate();

  SetInputName(kTextureInput, tr("Texture"));
  SetInputName(kHorizInput, tr("Horizontal"));
  SetInputName(kVertInput, tr("Vertical"));
}

void MosaicFilterNode::Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const {
  if (TexturePtr texture = value[kTextureInput].toTexture()) {
    if (texture && value[kHorizInput].toInt() != texture->width() && value[kVertInput].toInt() != texture->height()) {
      ShaderJob job(value);

      // Mipmapping makes this look weird, so we just use bilinear for finding the color of each block
      job.SetInterpolation(kTextureInput, Texture::kLinear);

      table->Push(NodeValue::kTexture, texture->toJob(job), this);
    } else {
      table->Push(value[kTextureInput]);
    }
  }
}

ShaderCode MosaicFilterNode::GetShaderCode(const ShaderRequest &request) const {
  Q_UNUSED(request)

  return ShaderCode(FileFunctions::ReadFileAsString(":/shaders/mosaic.frag"));
}

}  // namespace olive
