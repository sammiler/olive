

#include "projectexplorertreeview.h"

#include <QMouseEvent>

namespace olive {

ProjectExplorerTreeView::ProjectExplorerTreeView(QWidget *parent) : QTreeView(parent) {
  // Set selection mode (allows multiple item selection)
  setSelectionMode(QAbstractItemView::ExtendedSelection);

  // Allow dragging and dropping
  setDragDropMode(QAbstractItemView::DragDrop);

  // Enable dragging
  setDragEnabled(true);

  // Allow dropping from external sources
  setAcceptDrops(true);

  // Set context menu to emit a signal
  setContextMenuPolicy(Qt::CustomContextMenu);
}

void ProjectExplorerTreeView::mouseDoubleClickEvent(QMouseEvent *event) {
  // Perform default double click functions
  QTreeView::mouseDoubleClickEvent(event);

  // QAbstractItemView already has a doubleClicked() signal, but we emit another here for double clicking empty space
  if (!indexAt(event->pos()).isValid()) {
    emit DoubleClickedEmptyArea();
  }
}

}  // namespace olive
