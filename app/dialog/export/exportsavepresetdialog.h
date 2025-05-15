

#ifndef EXPORTSAVEPRESETDIALOG_H
#define EXPORTSAVEPRESETDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>

#include "codec/encoder.h"

namespace olive {

class ExportSavePresetDialog : public QDialog {
  Q_OBJECT
 public:
  explicit ExportSavePresetDialog(EncodingParams p, QWidget *parent = nullptr);

  [[nodiscard]] QString GetSelectedPresetName() const { return name_edit_->text(); }

 public slots:
  void accept() override;

 private:
  QLineEdit *name_edit_;

  EncodingParams params_;
};

}  // namespace olive

#endif  // EXPORTSAVEPRESETDIALOG_H
