

#ifndef PROJECTEXPLORERLISTVIEWITEMDELEGATE_H
#define PROJECTEXPLORERLISTVIEWITEMDELEGATE_H

#include <QStyledItemDelegate>

#include "common/define.h"

namespace olive {

/**
 * @brief The delegate that's used to draw items when ProjectExplorer is in List view
 */
class ProjectExplorerListViewItemDelegate : public QStyledItemDelegate {
 public:
  explicit ProjectExplorerListViewItemDelegate(QObject *parent = nullptr);

  [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

}  // namespace olive

#endif  // PROJECTEXPLORERLISTVIEWITEMDELEGATE_H
