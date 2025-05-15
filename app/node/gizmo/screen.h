

#ifndef SCREENGIZMO_H
#define SCREENGIZMO_H

#include "draggable.h"

namespace olive {

class ScreenGizmo : public DraggableGizmo {
  Q_OBJECT
 public:
  explicit ScreenGizmo(QObject *parent = nullptr);
};

}  // namespace olive

#endif  // SCREENGIZMO_H
