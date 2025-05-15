

#ifndef PREFERENCESDISKTAB_H
#define PREFERENCESDISKTAB_H

#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>

#include "dialog/configbase/configdialogbase.h"
#include "render/diskmanager.h"
#include "widget/path/pathwidget.h"
#include "widget/slider/floatslider.h"

namespace olive {

class PreferencesDiskTab : public ConfigDialogBaseTab {
  Q_OBJECT
 public:
  PreferencesDiskTab();

  bool Validate() override;

  void Accept(MultiUndoCommand* command) override;

 private:
  PathWidget* disk_cache_location_;

  FloatSlider* cache_ahead_slider_;

  FloatSlider* cache_behind_slider_;

  DiskCacheFolder* default_disk_cache_folder_;
};

}  // namespace olive

#endif  // PREFERENCESDISKTAB_H
