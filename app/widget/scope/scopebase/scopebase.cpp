#include "scopebase.h"

#include <utility>

#include "config/config.h"

namespace olive {

#define super ManagedDisplayWidget

ScopeBase::ScopeBase(QWidget* parent) : super(parent), texture_(nullptr), managed_tex_up_to_date_(false) {
  EnableDefaultContextMenu();
}

void ScopeBase::SetBuffer(TexturePtr frame) {
  texture_ = std::move(frame);
  managed_tex_up_to_date_ = false;
  update();
}

void ScopeBase::showEvent(QShowEvent* e) { super::showEvent(e); }

void ScopeBase::DrawScope(TexturePtr managed_tex, QVariant pipeline) {
  ShaderJob job;

  job.Insert(QStringLiteral("ove_maintex"), NodeValue(NodeValue::kTexture, QVariant::fromValue(managed_tex)));

  renderer()->Blit(std::move(pipeline), job, GetViewportParams());
}

void ScopeBase::OnInit() {
  super::OnInit();

  pipeline_ = renderer()->CreateNativeShader(GenerateShaderCode());
}

void ScopeBase::OnPaint() {
  // Clear display surface
  renderer()->ClearDestination();

  if (texture_) {
    // Convert reference frame to display space
    if (!managed_tex_ || !managed_tex_up_to_date_ || managed_tex_->params() != texture_->params()) {
      managed_tex_ = renderer()->CreateTexture(texture_->params());

      ColorTransformJob job;
      job.SetColorProcessor(color_service());
      job.SetInputTexture(texture_);
      job.SetInputAlphaAssociation(kAlphaNone);

      renderer()->BlitColorManaged(job, managed_tex_.get());
    }

    DrawScope(managed_tex_, pipeline_);
  }
}

void ScopeBase::OnDestroy() {
  managed_tex_ = nullptr;
  texture_ = nullptr;
  pipeline_.clear();

  super::OnDestroy();
}

}  // namespace olive
