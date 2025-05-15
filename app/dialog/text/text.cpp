

#include "text.h"

#include <QDebug>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

#include "ui/icons/icons.h"

namespace olive {

TextDialog::TextDialog(const QString& start, QWidget* parent) : QDialog(parent) {
  auto* layout = new QVBoxLayout(this);

  // Create text edit widget
  text_edit_ = new QPlainTextEdit();
  text_edit_->document()->setPlainText(start);
  layout->addWidget(text_edit_);

  // Create buttons
  auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  layout->addWidget(buttons);
  connect(buttons, &QDialogButtonBox::accepted, this, &TextDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, this, &TextDialog::reject);
}

}  // namespace olive
