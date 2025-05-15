

#ifndef PREFERENCESTAB_H
#define PREFERENCESTAB_H

#include <QWidget>

#include "config/config.h"
#include "undo/undocommand.h"

namespace olive {

class ConfigDialogBaseTab : public QWidget {
 public:
  ConfigDialogBaseTab() = default;

  virtual bool Validate();

  virtual void Accept(MultiUndoCommand *parent) = 0;
};

}  // namespace olive

#endif  // PREFERENCESTAB_H
