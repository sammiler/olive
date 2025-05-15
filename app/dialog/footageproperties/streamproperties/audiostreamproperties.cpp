

#include "audiostreamproperties.h"

namespace olive {

AudioStreamProperties::AudioStreamProperties(Footage *footage, int audio_index)
    : footage_(footage), audio_index_(audio_index) {}

void AudioStreamProperties::Accept(MultiUndoCommand *) {
  Q_UNUSED(footage_)
  Q_UNUSED(audio_index_)
}

}  // namespace olive
