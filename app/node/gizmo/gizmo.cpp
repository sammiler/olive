

#include "gizmo.h"

namespace olive {

NodeGizmo::NodeGizmo(QObject *parent) : visible_(true) { setParent(parent); }

NodeGizmo::~NodeGizmo() { setParent(nullptr); }

}  // namespace olive
