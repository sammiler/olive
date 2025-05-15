

#include "audiomonitor.h"

#include "panel/panelmanager.h"

namespace olive {

#define super PanelWidget

AudioMonitorPanel::AudioMonitorPanel() : super(QStringLiteral("AudioMonitor")) {
  audio_monitor_ = new AudioMonitor(this);

  SetWidgetWithPadding(audio_monitor_);

  Retranslate();
}

void AudioMonitorPanel::Retranslate() { SetTitle(tr("Audio Monitor")); }

}  // namespace olive
