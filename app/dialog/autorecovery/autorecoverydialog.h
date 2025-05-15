

#ifndef AUTORECOVERYDIALOG_H
#define AUTORECOVERYDIALOG_H

#include <QDialog>
#include <QTreeWidget>

#include "common/define.h"

namespace olive {

class AutoRecoveryDialog : public QDialog {
  Q_OBJECT
 public:
  AutoRecoveryDialog(const QString& message, const QStringList& recoveries, bool autocheck_latest, QWidget* parent);

 public slots:
  void accept() override;

 private:
  void Init(const QString& header_text);

  void PopulateTree(const QStringList& recoveries, bool autocheck);

  QTreeWidget* tree_widget_{};

  QVector<QTreeWidgetItem*> checkable_items_;

  enum DataRole { kFilenameRole = Qt::UserRole };
};

}  // namespace olive

#endif  // AUTORECOVERYDIALOG_H
