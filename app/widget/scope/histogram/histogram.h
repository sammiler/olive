#ifndef HISTOGRAMSCOPE_H
#define HISTOGRAMSCOPE_H

#include "widget/scope/scopebase/scopebase.h"

namespace olive {

class HistogramScope : public ScopeBase {
  Q_OBJECT
 public:
  explicit HistogramScope(QWidget* parent = nullptr);

  MANAGEDDISPLAYWIDGET_DEFAULT_DESTRUCTOR(HistogramScope)

 protected slots:
  void OnInit() override;

  void OnDestroy() override;

 protected:
  ShaderCode GenerateShaderCode() override;
  QVariant CreateSecondaryShader();

  void DrawScope(TexturePtr managed_tex, QVariant pipeline) override;

 private:
  QVariant pipeline_secondary_;
  TexturePtr texture_row_sums_;
};

}  // namespace olive

#endif  // HISTOGRAMSCOPE_H
