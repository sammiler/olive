#ifndef PIXELSAMPLERPANEL_H
#define PIXELSAMPLERPANEL_H

#include "panel/panel.h"
#include "widget/pixelsampler/pixelsampler.h"

namespace olive {

class PixelSamplerPanel : public PanelWidget {
  Q_OBJECT
 public:
  PixelSamplerPanel();

 public slots:
  void SetValues(const Color& reference, const Color& display);

 private:
  void Retranslate() override;

  ManagedPixelSamplerWidget* sampler_widget_;
};

}  // namespace olive

#endif  // PIXELSAMPLERPANEL_H
