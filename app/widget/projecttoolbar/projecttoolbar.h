#ifndef PROJECTTOOLBAR_H
#define PROJECTTOOLBAR_H

#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

#include "common/define.h"

namespace olive {

/**
 * @brief The ProjectToolbar class
 *
 * A toolbar consisting of project functions (new/open/save), edit functions (undo/redo), a search field, and a
 * project view selector (tree/icon/list).
 *
 * This object's signals can be connected to various functions in the application for better user experience.
 */
class ProjectToolbar : public QWidget {
  Q_OBJECT
 public:
  explicit ProjectToolbar(QWidget* parent);

  enum ViewType { TreeView, ListView, IconView };

 public slots:
  void SetView(ViewType type);

 protected:
  void changeEvent(QEvent*) override;

 signals:
  void NewClicked();
  void OpenClicked();
  void SaveClicked();

  void SearchChanged(const QString&);

  void ViewChanged(ViewType type);

 private:
  void Retranslate();
  void UpdateIcons();

  QPushButton* new_button_;
  QPushButton* open_button_;
  QPushButton* save_button_;

  QLineEdit* search_field_;

  QPushButton* tree_button_;
  QPushButton* list_button_;
  QPushButton* icon_button_;

 private slots:
  void ViewButtonClicked();
};

}  // namespace olive

#endif  // PROJECTTOOLBAR_H
