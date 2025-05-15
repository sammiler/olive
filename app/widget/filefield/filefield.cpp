

#include "filefield.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>

#include "ui/icons/icons.h"

namespace olive {

FileField::FileField(QWidget* parent) : QWidget(parent), directory_mode_(false) {
  auto* layout = new QHBoxLayout(this);

  layout->setContentsMargins(0, 0, 0, 0);

  line_edit_ = new QLineEdit();
  connect(line_edit_, &QLineEdit::textChanged, this, &FileField::LineEditChanged);
  connect(line_edit_, &QLineEdit::textEdited, this, &FileField::FilenameChanged);
  layout->addWidget(line_edit_);

  browse_btn_ = new QPushButton();
  browse_btn_->setIcon(icon::Open);
  connect(browse_btn_, &QPushButton::clicked, this, &FileField::BrowseBtnClicked);
  layout->addWidget(browse_btn_);
}

void FileField::BrowseBtnClicked() {
  QString s;

  if (directory_mode_) {
    s = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
  } else {
    s = QFileDialog::getOpenFileName(this, tr("Open File"));
  }

  if (!s.isEmpty()) {
    line_edit_->setText(s);
    emit FilenameChanged(s);
  }
}

void FileField::LineEditChanged(const QString& text) {
  if (QFileInfo::exists(text) || text.isEmpty()) {
    line_edit_->setStyleSheet(QString());
  } else {
    line_edit_->setStyleSheet(QStringLiteral("QLineEdit {color: red;}"));
  }
}

}  // namespace olive
