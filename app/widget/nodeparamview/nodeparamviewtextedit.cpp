

#include "nodeparamviewtextedit.h"

#include <QHBoxLayout>

#include "dialog/text/text.h"
#include "ui/icons/icons.h"

namespace olive {

NodeParamViewTextEdit::NodeParamViewTextEdit(QWidget* parent) : QWidget(parent) {
  auto* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);

  line_edit_ = new QPlainTextEdit();
  line_edit_->setUndoRedoEnabled(true);
  connect(line_edit_, &QPlainTextEdit::textChanged, this, &NodeParamViewTextEdit::InnerWidgetTextChanged);
  layout->addWidget(line_edit_);

  edit_btn_ = new QPushButton();
  edit_btn_->setIcon(icon::ToolEdit);
  edit_btn_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
  layout->addWidget(edit_btn_);
  connect(edit_btn_, &QPushButton::clicked, this, &NodeParamViewTextEdit::ShowTextDialog);

  edit_in_viewer_btn_ = new QPushButton(tr("Edit In Viewer"));
  edit_in_viewer_btn_->setIcon(icon::Pencil);
  layout->addWidget(edit_in_viewer_btn_);
  connect(edit_in_viewer_btn_, &QPushButton::clicked, this, &NodeParamViewTextEdit::RequestEditInViewer);

  SetEditInViewerOnlyMode(false);
}

void NodeParamViewTextEdit::SetEditInViewerOnlyMode(bool on) {
  line_edit_->setVisible(!on);
  edit_btn_->setVisible(!on);
  edit_in_viewer_btn_->setVisible(on);
}

void NodeParamViewTextEdit::ShowTextDialog() {
  TextDialog d(this->text(), this);
  if (d.exec() == QDialog::Accepted) {
    QString s = d.text();

    line_edit_->setPlainText(s);
    emit textEdited(s);
  }
}

void NodeParamViewTextEdit::InnerWidgetTextChanged() { emit textEdited(this->text()); }

}  // namespace olive
