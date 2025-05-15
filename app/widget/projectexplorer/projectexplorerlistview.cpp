#include "projectexplorerlistview.h"

namespace olive {

ProjectExplorerListView::ProjectExplorerListView(QWidget *parent) : ProjectExplorerListViewBase(parent) {
  setViewMode(QListView::ListMode);

  setItemDelegate(&delegate_);
}

}  // namespace olive
