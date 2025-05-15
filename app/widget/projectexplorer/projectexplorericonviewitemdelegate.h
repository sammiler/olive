#ifndef PROJECTEXPLORERICONVIEWITEMDELEGATE_H
#define PROJECTEXPLORERICONVIEWITEMDELEGATE_H

#include <QStyledItemDelegate>

#include "common/define.h"

namespace olive {

/**
 * @brief The delegate that's used to draw items when ProjectExplorer is in Icon view
 */
class ProjectExplorerIconViewItemDelegate : public QStyledItemDelegate {
 public:
  explicit ProjectExplorerIconViewItemDelegate(QObject *parent = nullptr);

  [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

}  // namespace olive

#endif  // PROJECTEXPLORERICONVIEWITEMDELEGATE_H
