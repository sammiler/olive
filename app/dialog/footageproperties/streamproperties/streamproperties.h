

#ifndef STREAMPROPERTIES_H
#define STREAMPROPERTIES_H

#include <QWidget>

#include "common/define.h"
#include "undo/undocommand.h"

namespace olive {

class StreamProperties : public QWidget {
 public:
  explicit StreamProperties(QWidget* parent = nullptr);

  virtual void Accept(MultiUndoCommand*) {}

  virtual bool SanityCheck() { return true; }
};

}  // namespace olive

#endif  // STREAMPROPERTIES_H
