#ifndef PROJECTEXPLORERLISTVIEW_H
#define PROJECTEXPLORERLISTVIEW_H

#include "projectexplorerlistviewbase.h"
#include "projectexplorerlistviewitemdelegate.h"

namespace olive {

/**
 * @brief The view widget used when ProjectExplorer is in List View
 */
class ProjectExplorerListView : public ProjectExplorerListViewBase {
  Q_OBJECT
 public:
  explicit ProjectExplorerListView(QWidget* parent);

 private:
  ProjectExplorerListViewItemDelegate delegate_;
};

}  // namespace olive

#endif  // PROJECTEXPLORERLISTVIEW_H
