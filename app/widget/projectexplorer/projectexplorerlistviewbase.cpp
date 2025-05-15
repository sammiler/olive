

#include "projectexplorerlistviewbase.h"

#include <QMouseEvent>

namespace olive {

ProjectExplorerListViewBase::ProjectExplorerListViewBase(QWidget *parent) : QListView(parent) {
  // FIXME Is this necessary?
  setMovement(QListView::Free);

  // Set selection mode (allows multiple item selection)
  setSelectionMode(QAbstractItemView::ExtendedSelection);

  // Set resize mode
  setResizeMode(QListView::Adjust);

  // Set widget to emit a signal on right click
  setContextMenuPolicy(Qt::CustomContextMenu);
}

void ProjectExplorerListViewBase::mouseDoubleClickEvent(QMouseEvent *event) {
  // Cache here so if the index becomes invalid after the base call, we still know the truth
  bool item_at_location = indexAt(event->pos()).isValid();

  // Perform default double click functions
  QListView::mouseDoubleClickEvent(event);

  // QAbstractItemView already has a doubleClicked() signal, but we emit another here for double clicking empty space
  if (!item_at_location) {
    emit DoubleClickedEmptyArea();
  }
}

}  // namespace olive
