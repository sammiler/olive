#ifndef PROJECT_PANEL_H
#define PROJECT_PANEL_H

#include "footagemanagementpanel.h"
#include "node/project.h"
#include "panel/panel.h"
#include "widget/projectexplorer/projectexplorer.h"

namespace olive {

/**
 * @brief A PanelWidget wrapper around a ProjectExplorer and a ProjectToolbar
 */
class ProjectPanel : public PanelWidget, public FootageManagementPanel {
  Q_OBJECT
 public:
  explicit ProjectPanel(const QString &unique_name);

  [[nodiscard]] Project *project() const;
  void set_project(Project *p);

  [[nodiscard]] Folder *get_root() const;

  void set_root(Folder *item);

  [[nodiscard]] QVector<Node *> SelectedItems() const;

  [[nodiscard]] Folder *GetSelectedFolder() const;

  [[nodiscard]] QVector<ViewerOutput *> GetSelectedFootage() const override;

  [[nodiscard]] ProjectViewModel *model() const;

  bool SelectItem(Node *n, bool deselect_all_first = true) { return explorer_->SelectItem(n, deselect_all_first); }

  void SelectAll() override;
  void DeselectAll() override;

  void DeleteSelected() override;

  void RenameSelected() override;

 public slots:
  void Edit(Node *item);

 signals:
  void ProjectNameChanged();

  void SelectionChanged(const QVector<Node *> &selected);

 private:
  void Retranslate() override;

  ProjectExplorer *explorer_;

 private slots:
  static void ItemDoubleClickSlot(Node *item);

  void ShowNewMenu();

  void UpdateSubtitle();

  static void SaveConnectedProject();
};

}  // namespace olive

#endif  // PROJECT_PANEL_H
