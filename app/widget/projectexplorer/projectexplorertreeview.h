#ifndef PROJECTEXPLORERTREEVIEW_H
#define PROJECTEXPLORERTREEVIEW_H

#include <QTreeView>

#include "common/define.h"

namespace olive {

/**
 * @brief The view widget used when ProjectExplorer is in Tree View
 *
 * A fairly simple subclass of QTreeView that provides a double clicked signal whether the index is valid or not
 * (QAbstractItemView has a doubleClicked() signal but it's only emitted with a valid index).
 */
class ProjectExplorerTreeView : public QTreeView {
  Q_OBJECT
 public:
  explicit ProjectExplorerTreeView(QWidget* parent);

 protected:
  /**
   * @brief Double click event override
   *
   * Function that signals DoubleClickedView().
   *
   * FIXME: This code is the same as the code in ProjectExplorerListViewBase. Is there a way to merge these two through
   *
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

#endif  // PROJECTEXPLORERTREEVIEW_H
