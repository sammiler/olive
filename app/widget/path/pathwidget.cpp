

#include "pathwidget.h"

#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>

#include "common/filefunctions.h"

namespace olive {

PathWidget::PathWidget(const QString &path, QWidget *parent) : QWidget(parent) {
  auto *layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);

  path_edit_ = new QLineEdit();
  path_edit_->setText(path);
  layout->addWidget(path_edit_);
  connect(path_edit_, &QLineEdit::textChanged, this, &PathWidget::LineEditChanged);

  browse_btn_ = new QPushButton(tr("Browse"));
  layout->addWidget(browse_btn_);

  connect(browse_btn_, &QPushButton::clicked, this, &PathWidget::BrowseClicked);
}

void PathWidget::BrowseClicked() {
  QString dir =
      QFileDialog::getExistingDirectory(dynamic_cast<QWidget *>(parent()), tr("Browse for path"), path_edit_->text());

  if (!dir.isEmpty()) {
    path_edit_->setText(dir);
  }
}

void PathWidget::LineEditChanged() {
  if (FileFunctions::DirectoryIsValid(text(), false)) {
    path_edit_->setStyleSheet(QString());
  } else {
    path_edit_->setStyleSheet(QStringLiteral("QLineEdit {color: red;}"));
  }
}

}  // namespace olive
