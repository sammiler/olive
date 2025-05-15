#ifndef CONFIGBASE_H
#define CONFIGBASE_H

#include <QDialog>
#include <QListWidget>
#include <QStackedWidget>

#include "configdialogbasetab.h"

namespace olive {

class ConfigDialogBase : public QDialog {
  Q_OBJECT
 public:
  explicit ConfigDialogBase(QWidget* parent = nullptr);

 private slots:
  /**
   * @brief Override of accept to save preferences to Config.
   */
  void accept() override;

 protected:
  void AddTab(ConfigDialogBaseTab* tab, const QString& title);

  virtual void AcceptEvent() {}

 private:
  QListWidget* list_widget_;

  QStackedWidget* preference_pane_stack_;

  QList<ConfigDialogBaseTab*> tabs_;
};

}  // namespace olive

#endif  // CONFIGBASE_H
