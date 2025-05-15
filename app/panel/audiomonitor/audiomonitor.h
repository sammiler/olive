

#ifndef AUDIOMONITORPANEL_H
#define AUDIOMONITORPANEL_H

#include "panel/panel.h"
#include "widget/audiomonitor/audiomonitor.h"

namespace olive {

/**
 * @brief PanelWidget wrapper around an AudioMonitor
 */
class AudioMonitorPanel : public PanelWidget {
  Q_OBJECT
 public:
  AudioMonitorPanel();

  [[nodiscard]] bool IsPlaying() const { return audio_monitor_->IsPlaying(); }

  void SetParams(const AudioParams& params) { audio_monitor_->SetParams(params); }

 private:
  void Retranslate() override;

  AudioMonitor* audio_monitor_;
};

}  // namespace olive

#endif  // AUDIOMONITORPANEL_H
