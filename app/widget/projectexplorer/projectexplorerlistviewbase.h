#ifndef PROJECTEXPLORERLISTVIEWBASE_H
#define PROJECTEXPLORERLISTVIEWBASE_H

#include <QListView>

#include "common/define.h"

namespace olive {

/**
 * @brief A QListView derivative that contains functionality used by both List view and Icon view (which are both based
 * on QListView)
 */
class ProjectExplorerListViewBase : public QListView {
  Q_OBJECT
 public:
  explicit ProjectExplorerListViewBase(QWidget* parent);

 protected:
  /**
   * @brief Double click event override
   *
   * Function that signals DoubleClickedView().
   *
   * FIXME: This code is the same as the code in ProjectExplorerTreeView. Is there a way to merge these two through
   * subclassing?
   */
  void mouseDoubleClickEvent(QMouseEvent* event) override;

 signals:
  /**
   * @brief Unconditional double click signal
   *
   * Emits a signal when the view is double clicked but not on any particular item
   */
  void DoubleClickedEmptyArea();
};

}  // namespace olive

#endif  // PROJECTEXPLORERLISTVIEWBASE_H
