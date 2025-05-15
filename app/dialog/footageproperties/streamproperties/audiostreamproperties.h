

#ifndef AUDIOSTREAMPROPERTIES_H
#define AUDIOSTREAMPROPERTIES_H

#include "node/project/footage/footage.h"
#include "streamproperties.h"

namespace olive {

class AudioStreamProperties : public StreamProperties {
 public:
  AudioStreamProperties(Footage *footage, int audio_index);

  void Accept(MultiUndoCommand *parent) override;

 private:
  Footage *footage_;

  int audio_index_;
};

}  // namespace olive

#endif  // AUDIOSTREAMPROPERTIES_H
