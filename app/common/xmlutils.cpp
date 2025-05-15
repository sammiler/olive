

#include "xmlutils.h"

#include "node/block/block.h"
#include "node/factory.h"

namespace olive {

bool XMLReadNextStartElement(QXmlStreamReader *reader, CancelAtom *cancel_atom) {
  QXmlStreamReader::TokenType token;

  while ((token = reader->readNext()) != QXmlStreamReader::Invalid && token != QXmlStreamReader::EndDocument &&
         (!cancel_atom || !cancel_atom->IsCancelled())) {
    if (reader->isEndElement()) {
      return false;
    } else if (reader->isStartElement()) {
      return true;
    }
  }

  return false;
}

}  // namespace olive
