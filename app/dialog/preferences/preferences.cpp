

#include "preferences.h"

#include <QDialogButtonBox>
#include <QListWidget>
#include <QSplitter>
#include <QVBoxLayout>

#include "config/config.h"
#include "tabs/preferencesappearancetab.h"
#include "tabs/preferencesaudiotab.h"
#include "tabs/preferencesbehaviortab.h"
#include "tabs/preferencesdisktab.h"
#include "tabs/preferencesgeneraltab.h"
#include "tabs/preferenceskeyboardtab.h"
#include "window/mainwindow/mainwindow.h"

namespace olive {

PreferencesDialog::PreferencesDialog(MainWindow *main_window) : ConfigDialogBase(main_window) {
  setWindowTitle(tr("Preferences"));

  AddTab(new PreferencesGeneralTab(), tr("General"));
  AddTab(new PreferencesAppearanceTab(), tr("Appearance"));
  AddTab(new PreferencesBehaviorTab(), tr("Behavior"));
  AddTab(new PreferencesDiskTab(), tr("Disk"));
  AddTab(new PreferencesAudioTab(), tr("Audio"));
  AddTab(new PreferencesKeyboardTab(main_window), tr("Keyboard"));
}

void PreferencesDialog::AcceptEvent() { Config::Save(); }

}  // namespace olive
