#ifndef PREFERENCESGENERALTAB_H
#define PREFERENCESGENERALTAB_H

#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>

#include "dialog/configbase/configdialogbase.h"
#include "node/project/sequence/sequence.h"
#include "widget/slider/integerslider.h"
#include "widget/slider/rationalslider.h"

namespace olive {

class PreferencesGeneralTab : public ConfigDialogBaseTab {
  Q_OBJECT
 public:
  PreferencesGeneralTab();

  void Accept(MultiUndoCommand* command) override;

 private:
  void AddLanguage(const QString& locale_name);

  QComboBox* language_combobox_;

  QComboBox* autoscroll_method_;

  QCheckBox* rectified_waveforms_;

  RationalSlider* default_still_length_;

  QCheckBox* autorecovery_enabled_;

  IntegerSlider* autorecovery_interval_;

  IntegerSlider* autorecovery_maximum_;
};

}  // namespace olive

#endif  // PREFERENCESGENERALTAB_H
