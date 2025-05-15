

#ifndef PROJECTPROPERTIESDIALOG_H
#define PROJECTPROPERTIESDIALOG_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QGridLayout>
#include <QLineEdit>
#include <QRadioButton>

#include "node/project.h"
#include "widget/path/pathwidget.h"

namespace olive {

class ProjectPropertiesDialog : public QDialog {
  Q_OBJECT
 public:
  ProjectPropertiesDialog(Project* p, QWidget* parent);

 public slots:
  void accept() override;

 private:
  bool VerifyPathAndWarnIfBad(const QString& path);

  Project* working_project_;

  QLineEdit* ocio_filename_;

  QComboBox* default_input_colorspace_;

  QComboBox* reference_space_;

  bool ocio_config_is_valid_;

  QString ocio_config_error_;

  PathWidget* custom_cache_path_;

  static const int kDiskCacheRadioCount = 3;
  QRadioButton* disk_cache_radios_[kDiskCacheRadioCount]{};

 private slots:
  void BrowseForOCIOConfig();

  void OCIOFilenameUpdated();

  void OpenDiskCacheSettings();
};

}  // namespace olive

#endif  // PROJECTPROPERTIESDIALOG_H
