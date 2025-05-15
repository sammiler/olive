#include "generatorwithmerge.h"

#include "node/math/merge/merge.h"

namespace olive {

#define super Node

const QString GeneratorWithMerge::kBaseInput = QStringLiteral("base_in");

GeneratorWithMerge::GeneratorWithMerge() {
  AddInput(kBaseInput, NodeValue::kTexture, InputFlags(kInputFlagNotKeyframable));
  SetEffectInput(kBaseInput);
  SetFlag(kVideoEffect);
}

void GeneratorWithMerge::Retranslate() {
  super::Retranslate();

  SetInputName(kBaseInput, tr("Base"));
}

ShaderCode GeneratorWithMerge::GetShaderCode(const ShaderRequest &request) const {
  if (request.id == QStringLiteral("mrg")) {
    return ShaderCode(FileFunctions::ReadFileAsString(":/shaders/alphaover.frag"));
  }

  return ShaderCode();
}

void GeneratorWithMerge::PushMergableJob(const NodeValueRow &value, const TexturePtr &job,
                                         NodeValueTable *table) const {
  if (TexturePtr base = value[kBaseInput].toTexture()) {
    // Push as merge node
    ShaderJob merge;

    merge.SetShaderID(QStringLiteral("mrg"));
    merge.Insert(MergeNode::kBaseIn, value[kBaseInput]);
    merge.Insert(MergeNode::kBlendIn, NodeValue(NodeValue::kTexture, job, this));

    table->Push(NodeValue::kTexture, base->toJob(merge), this);
  } else {
    // Just push generate job
    table->Push(NodeValue::kTexture, job, this);
  }
}

}  // namespace olive
