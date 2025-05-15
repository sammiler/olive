

#include "scope.h"

#include <QVBoxLayout>

#include "panel/viewer/viewer.h"

namespace olive {

ScopePanel::ScopePanel() : PanelWidget(QStringLiteral("ScopePanel")), viewer_(nullptr) {
  auto* central = new QWidget(this);
  setWidget(central);

  auto* layout = new QVBoxLayout(central);

  auto* toolbar_layout = new QHBoxLayout();
  toolbar_layout->setContentsMargins(0, 0, 0, 0);

  scope_type_combobox_ = new QComboBox();

  for (int i = 0; i < ScopePanel::kTypeCount; i++) {
    // These strings get filled in later in Retranslate()
    scope_type_combobox_->addItem(QString());
  }

  toolbar_layout->addWidget(scope_type_combobox_);
  toolbar_layout->addStretch();

  layout->addLayout(toolbar_layout);

  stack_ = new QStackedWidget();
  layout->addWidget(stack_);

  // Create waveform view
  waveform_view_ = new WaveformScope();
  stack_->addWidget(waveform_view_);

  // Create histogram
  histogram_ = new HistogramScope();
  stack_->addWidget(histogram_);

  connect(scope_type_combobox_, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), stack_,
          &QStackedWidget::setCurrentIndex);

  Retranslate();
}

void ScopePanel::SetType(ScopePanel::Type t) { scope_type_combobox_->setCurrentIndex(t); }

QString ScopePanel::TypeToName(ScopePanel::Type t) {
  switch (t) {
    case kTypeWaveform:
      return tr("Waveform");
    case kTypeHistogram:
      return tr("Histogram");
    case kTypeCount:
      break;
  }

  return {};
}

void ScopePanel::SetViewerPanel(ViewerPanelBase* vp) {
  if (viewer_ == vp) {
    return;
  }

  if (viewer_) {
    disconnect(viewer_, &ViewerPanelBase::TextureChanged, this, &ScopePanel::SetReferenceBuffer);
    disconnect(viewer_, &ViewerPanelBase::ColorManagerChanged, this, &ScopePanel::SetColorManager);
  }

  viewer_ = vp;

  if (viewer_) {
    // Connect viewer widget texture drawing to scope panel
    connect(viewer_, &ViewerPanelBase::TextureChanged, this, &ScopePanel::SetReferenceBuffer);
    connect(viewer_, &ViewerPanelBase::ColorManagerChanged, this, &ScopePanel::SetColorManager);

    SetColorManager(viewer_->GetColorManager());

    viewer_->UpdateTextureFromNode();
  } else {
    SetReferenceBuffer(nullptr);
    SetColorManager(nullptr);
  }
}

void ScopePanel::SetReferenceBuffer(const TexturePtr& frame) {
  histogram_->SetBuffer(frame);
  waveform_view_->SetBuffer(frame);
}

void ScopePanel::SetColorManager(ColorManager* manager) {
  histogram_->ConnectColorManager(manager);
  waveform_view_->ConnectColorManager(manager);
}

void ScopePanel::Retranslate() {
  SetTitle(tr("Scopes"));

  for (int i = 0; i < ScopePanel::kTypeCount; i++) {
    scope_type_combobox_->setItemText(i, TypeToName(static_cast<Type>(i)));
  }
}

}  // namespace olive
