#ifndef TYPESERIALIZER_H
#define TYPESERIALIZER_H

#include <olive/core/core.h>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "common/xmlutils.h"

namespace olive {

using namespace core;

class TypeSerializer {
 public:
  TypeSerializer() = default;

  static AudioParams LoadAudioParams(QXmlStreamReader *reader);
  static void SaveAudioParams(QXmlStreamWriter *writer, const AudioParams &a);
};

}  // namespace olive

#endif  // TYPESERIALIZER_H
