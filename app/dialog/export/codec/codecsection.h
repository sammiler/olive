

#ifndef CODECSECTION_H
#define CODECSECTION_H

#include <QWidget>

#include "codec/encoder.h"

namespace olive {

class CodecSection : public QWidget {
  Q_OBJECT
 public:
  explicit CodecSection(QWidget* parent = nullptr);

  virtual void AddOpts(EncodingParams* params) { Q_UNUSED(params) }

  virtual void SetOpts(const EncodingParams* p) { Q_UNUSED(p) }
};

}  // namespace olive

#endif  // CODECSECTION_H
