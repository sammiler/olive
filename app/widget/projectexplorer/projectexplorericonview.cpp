#include "projectexplorericonview.h"

namespace olive {

ProjectExplorerIconView::ProjectExplorerIconView(QWidget *parent) : ProjectExplorerListViewBase(parent) {
  setViewMode(QListView::IconMode);

  setItemDelegate(&delegate_);
}

}  // namespace olive
