#ifndef RENDERCACHE_H
#define RENDERCACHE_H

#include "codec/decoder.h"

namespace olive {

template <typename K, typename V>
class RenderCache : public QHash<K, V> {
 public:
  QMutex *mutex() { return &mutex_; }

 private:
  QMutex mutex_;
};

struct DecoderPair {
  DecoderPtr decoder = nullptr;
  qint64 last_modified = 0;
};

using DecoderCache = RenderCache<Decoder::CodecStream, DecoderPair>;
using ShaderCache = RenderCache<QString, QVariant>;

}  // namespace olive

#endif  // RENDERCACHE_H
