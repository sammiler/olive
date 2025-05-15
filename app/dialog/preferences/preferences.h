#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QCheckBox>
#include <QDialog>
#include <QListWidget>
#include <QMenuBar>
#include <QStackedWidget>
#include <QTabWidget>

#include "dialog/configbase/configdialogbase.h"

namespace olive {

class MainWindow;

/**
 * @brief The PreferencesDialog class
 *
 * A dialog for the global application settings. Mostly an interface for Config.
 */
class PreferencesDialog : public ConfigDialogBase {
  Q_OBJECT

 public:
  explicit PreferencesDialog(MainWindow *main_window);

 protected:
  void AcceptEvent() override;
};

}  // namespace olive

#endif  // PREFERENCESDIALOG_H
