#include "exportsavepresetdialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <utility>

namespace olive {

ExportSavePresetDialog::ExportSavePresetDialog(EncodingParams p, QWidget *parent)
    : QDialog(parent), params_(std::move(p)) {
  auto layout = new QVBoxLayout(this);

  name_edit_ = new QLineEdit();

  // Populate existing list
  QStringList l = EncodingParams::GetListOfPresets();
  if (!l.empty()) {
    auto list_widget_ = new QListWidget();
    for (const QString &f : l) {
      list_widget_->addItem(f);
    }
    connect(list_widget_, &QListWidget::currentTextChanged, name_edit_, &QLineEdit::setText);
    layout->addWidget(list_widget_);
  }

  auto name_layout = new QHBoxLayout();
  layout->addLayout(name_layout);

  name_layout->addWidget(new QLabel(tr("Name:")));

  name_edit_->setFocus();
  name_layout->addWidget(name_edit_);

  auto btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(btns, &QDialogButtonBox::accepted, this, &ExportSavePresetDialog::accept);
  connect(btns, &QDialogButtonBox::rejected, this, &ExportSavePresetDialog::reject);
  layout->addWidget(btns);

  setWindowTitle(tr("Save Export Preset"));
}

void ExportSavePresetDialog::accept() {
  if (name_edit_->text().isEmpty()) {
    QMessageBox::critical(this, tr("Invalid Name"), tr("You must enter a name to save an export preset."));
    return;
  }

  QDir d(EncodingParams::GetPresetPath());
  if (!d.exists()) {
    d.mkpath(QStringLiteral("."));
  }

  QFile f(d.filePath(name_edit_->text()));
  if (f.exists()) {
    if (QMessageBox::question(
            this, tr("Overwrite Preset"),
            tr("A preset with the name \"%1\" already exists. Do you wish to overwrite it?").arg(name_edit_->text()),
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
      return;
    }
  }

  if (!f.open(QFile::WriteOnly)) {
    QMessageBox::critical(this, tr("Write Error"), tr("Failed to open file \"%1\" for writing.").arg(f.fileName()));
    return;
  }

  params_.Save(&f);

  f.close();

  QDialog::accept();
}

}  // namespace olive
