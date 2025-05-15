#ifndef SCOPE_PANEL_H
#define SCOPE_PANEL_H

#include <QComboBox>
#include <QStackedWidget>

#include "panel/panel.h"
#include "panel/viewer/viewerbase.h"
#include "widget/scope/histogram/histogram.h"
#include "widget/scope/waveform/waveform.h"

namespace olive {

class ScopePanel : public PanelWidget {
  Q_OBJECT
 public:
  enum Type {
    kTypeWaveform,
    kTypeHistogram,

    kTypeCount
  };

  ScopePanel();

  void SetType(Type t);

  static QString TypeToName(Type t);

  void SetViewerPanel(ViewerPanelBase* vp);

  [[nodiscard]] ViewerPanelBase* GetConnectedViewerPanel() const { return viewer_; }

 public slots:
  void SetReferenceBuffer(const TexturePtr& frame);

  void SetColorManager(ColorManager* manager);

 protected:
  void Retranslate() override;

 private:
  Type type_;

  QStackedWidget* stack_;

  QComboBox* scope_type_combobox_;

  WaveformScope* waveform_view_;

  HistogramScope* histogram_;

  ViewerPanelBase* viewer_;
};

}  // namespace olive

#endif  // SCOPE_PANEL_H
