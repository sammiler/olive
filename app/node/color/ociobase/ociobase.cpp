

#include "ociobase.h"

#include "node/color/colormanager/colormanager.h"
#include "node/project.h"

namespace olive {

const QString OCIOBaseNode::kTextureInput = QStringLiteral("tex_in");

OCIOBaseNode::OCIOBaseNode() : manager_(nullptr), processor_(nullptr) {
  AddInput(kTextureInput, NodeValue::kTexture, InputFlags(kInputFlagNotKeyframable));

  SetEffectInput(kTextureInput);

  SetFlag(kVideoEffect);
}

void OCIOBaseNode::AddedToGraphEvent(Project *p) {
  manager_ = p->color_manager();
  connect(manager_, &ColorManager::ConfigChanged, this, &OCIOBaseNode::ConfigChanged);
  ConfigChanged();
}

void OCIOBaseNode::RemovedFromGraphEvent(Project *p) {
  if (manager_) {
    disconnect(manager_, &ColorManager::ConfigChanged, this, &OCIOBaseNode::ConfigChanged);
    manager_ = nullptr;
  }
}

void OCIOBaseNode::Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const {
  auto tex_met = value[kTextureInput];
  TexturePtr t = tex_met.toTexture();
  if (t && processor_) {
    ColorTransformJob job;

    job.SetColorProcessor(processor_);
    job.SetInputTexture(tex_met);

    table->Push(NodeValue::kTexture, t->toJob(job), this);
  }
}

}  // namespace olive
