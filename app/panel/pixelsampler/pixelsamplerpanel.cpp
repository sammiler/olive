

#include "pixelsamplerpanel.h"

#include "core.h"

namespace olive {

PixelSamplerPanel::PixelSamplerPanel() : PanelWidget(QStringLiteral("PixelSamplerPanel")) {
  sampler_widget_ = new ManagedPixelSamplerWidget(this);
  SetWidgetWithPadding(sampler_widget_);

  connect(this, &PixelSamplerPanel::shown, Core::instance(),
          [] { Core::instance()->RequestPixelSamplingInViewers(true); });
  connect(this, &PixelSamplerPanel::hidden, Core::instance(),
          [] { Core::instance()->RequestPixelSamplingInViewers(false); });
  connect(Core::instance(), &Core::ColorPickerColorEmitted, this, &PixelSamplerPanel::SetValues);

  Retranslate();
}

void PixelSamplerPanel::SetValues(const Color &reference, const Color &display) {
  sampler_widget_->SetValues(reference, display);
}

void PixelSamplerPanel::Retranslate() { SetTitle(tr("Pixel Sampler")); }

}  // namespace olive
