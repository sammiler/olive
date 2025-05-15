#ifndef FOOTAGERELINKDIALOG_H
#define FOOTAGERELINKDIALOG_H

#include <QDialog>
#include <QTreeWidget>

#include "node/project/footage/footage.h"

namespace olive {

class FootageRelinkDialog : public QDialog {
  Q_OBJECT
 public:
  explicit FootageRelinkDialog(const QVector<Footage*>& footage, QWidget* parent = nullptr);

 private:
  void UpdateFootageItem(int index);

  QTreeWidget* table_;

  QVector<Footage*> footage_;

 private slots:
  void BrowseForFootage();
};

}  // namespace olive

#endif  // FOOTAGERELINKDIALOG_H
