#include "importerrordialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>

namespace olive {

ProjectImportErrorDialog::ProjectImportErrorDialog(const QStringList& filenames, QWidget* parent) : QDialog(parent) {
  auto* layout = new QVBoxLayout(this);

  setWindowTitle(tr("Import Error"));

  layout->addWidget(
      new QLabel(tr("The following files failed to import. Olive likely does not "
                    "support their formats.")));

  auto* list_widget = new QListWidget();
  foreach (const QString& s, filenames) {
    list_widget->addItem(s);
  }
  layout->addWidget(list_widget);

  auto* buttons = new QDialogButtonBox();
  buttons->setStandardButtons(QDialogButtonBox::Ok);
  buttons->setCenterButtons(true);
  connect(buttons, &QDialogButtonBox::accepted, this, &ProjectImportErrorDialog::accept);
  layout->addWidget(buttons);
}

}  // namespace olive
