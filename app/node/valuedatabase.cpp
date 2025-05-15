

#include "valuedatabase.h"

namespace olive {

NodeValueTable NodeValueDatabase::Merge() const {
  QHash<QString, NodeValueTable> copy = tables_;

  // Kinda hacky, but we don't need this table to slipstream
  copy.remove(QStringLiteral("global"));

  return NodeValueTable::Merge(copy.values());
}

}  // namespace olive
