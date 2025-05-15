#include "progress.h"

#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "window/mainwindow/mainwindow.h"

namespace olive {

#define super QDialog

ProgressDialog::ProgressDialog(const QString& message, const QString& title, QWidget* parent)
    : super(parent), show_progress_(true), first_show_(true) {
  if (!title.isEmpty()) {
    setWindowTitle(title);
  }

  auto* layout = new QVBoxLayout(this);

  layout->addWidget(new QLabel(message));

  bar_ = new QProgressBar();
  bar_->setMinimum(0);
  bar_->setValue(0);
  bar_->setMaximum(100);
  layout->addWidget(bar_);

  elapsed_timer_lbl_ = new ElapsedCounterWidget();
  layout->addWidget(elapsed_timer_lbl_);

  auto* cancel_layout = new QHBoxLayout();
  layout->addLayout(cancel_layout);
  cancel_layout->setContentsMargins(0, 0, 0, 0);
  cancel_layout->setSpacing(0);
  cancel_layout->addStretch();

  auto* cancel_btn = new QPushButton(tr("Cancel"));

  // Signal that derivatives can connect to
  connect(cancel_btn, &QPushButton::clicked, this, &ProgressDialog::Cancelled, Qt::DirectConnection);

  // Stop updating the elapsed/remaining timers
  connect(cancel_btn, &QPushButton::clicked, elapsed_timer_lbl_, &ElapsedCounterWidget::Stop);

  // Disable the button so that users know they don't need to keep clicking it
  connect(cancel_btn, &QPushButton::clicked, this, &ProgressDialog::DisableSenderWidget);

  // Prevent the progress bar from continuing to move
  connect(cancel_btn, &QPushButton::clicked, this, &ProgressDialog::DisableProgressWidgets);

  cancel_layout->addWidget(cancel_btn);

  cancel_layout->addStretch();
}

void ProgressDialog::showEvent(QShowEvent* e) {
  super::showEvent(e);

  if (first_show_) {
    elapsed_timer_lbl_->Start();

    Core::instance()->main_window()->SetApplicationProgressStatus(MainWindow::kProgressShow);

    first_show_ = false;
  }
}

void ProgressDialog::closeEvent(QCloseEvent* e) {
  super::closeEvent(e);

  Core::instance()->main_window()->SetApplicationProgressStatus(MainWindow::kProgressNone);

  elapsed_timer_lbl_->Stop();

  first_show_ = true;
}

void ProgressDialog::SetProgress(double value) {
  if (!show_progress_) {
    return;
  }

  int percent = qRound(100.0 * value);

  bar_->setValue(percent);
  elapsed_timer_lbl_->SetProgress(value);

  Core::instance()->main_window()->SetApplicationProgressValue(percent);
}

void ProgressDialog::ShowErrorMessage(const QString& title, const QString& message) {
  Core::instance()->main_window()->SetApplicationProgressStatus(MainWindow::kProgressError);

  QMessageBox b(this);
  b.setIcon(QMessageBox::Critical);
  b.setWindowModality(Qt::WindowModal);
  b.setWindowTitle(title);
  b.setText(message);
  b.addButton(QMessageBox::Ok);
  b.exec();
}

void ProgressDialog::DisableSenderWidget() { dynamic_cast<QWidget*>(sender())->setEnabled(false); }

void ProgressDialog::DisableProgressWidgets() { show_progress_ = false; }

}  // namespace olive
