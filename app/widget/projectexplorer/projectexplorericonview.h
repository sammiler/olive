#ifndef PROJECTEXPLORERICONVIEW_H
#define PROJECTEXPLORERICONVIEW_H

#include "projectexplorericonviewitemdelegate.h"
#include "projectexplorerlistviewbase.h"

namespace olive {

/**
 * @brief The view widget used when ProjectExplorer is in Icon View
 */
class ProjectExplorerIconView : public ProjectExplorerListViewBase {
  Q_OBJECT
 public:
  explicit ProjectExplorerIconView(QWidget* parent);

 private:
  ProjectExplorerIconViewItemDelegate delegate_;
};

}  // namespace olive

#endif  // PROJECTEXPLORERICONVIEW_H
