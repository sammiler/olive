

#ifndef PROJECTIMPORTERRORDIALOG_H
#define PROJECTIMPORTERRORDIALOG_H

#include <QDialog>

#include "common/define.h"

namespace olive {

class ProjectImportErrorDialog : public QDialog {
  Q_OBJECT
 public:
  explicit ProjectImportErrorDialog(const QStringList& filenames, QWidget* parent = nullptr);
};

}  // namespace olive

#endif  // PROJECTIMPORTERRORDIALOG_H
