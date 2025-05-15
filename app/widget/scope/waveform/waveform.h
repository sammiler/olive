

#ifndef WAVEFORMSCOPE_H
#define WAVEFORMSCOPE_H

#include "widget/scope/scopebase/scopebase.h"

namespace olive {

class WaveformScope : public ScopeBase {
  Q_OBJECT
 public:
  explicit WaveformScope(QWidget* parent = nullptr);

  MANAGEDDISPLAYWIDGET_DEFAULT_DESTRUCTOR(WaveformScope)

 protected:
  ShaderCode GenerateShaderCode() override;

  void DrawScope(TexturePtr managed_tex, QVariant pipeline) override;
};

}  // namespace olive

#endif  // WAVEFORMSCOPE_H
