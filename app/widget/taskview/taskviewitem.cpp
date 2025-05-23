#include "taskviewitem.h"

#include <QDateTime>
#include <QVBoxLayout>

#include "ui/icons/icons.h"

namespace olive {

TaskViewItem::TaskViewItem(Task* task, QWidget* parent) : QFrame(parent), task_(task) {
  // Draw border around this item
  setFrameShape(QFrame::StyledPanel);

  // Create layout
  auto* layout = new QVBoxLayout(this);

  // Create header label
  task_name_lbl_ = new QLabel(this);
  task_name_lbl_->setText(task_->GetTitle());
  layout->addWidget(task_name_lbl_);

  // Create center layout (combines progress bar and a cancel button)
  auto* middle_layout = new QHBoxLayout();
  layout->addLayout(middle_layout);

  // Create progress bar
  progress_bar_ = new QProgressBar(this);
  progress_bar_->setRange(0, 100);
  middle_layout->addWidget(progress_bar_);

  // Create cancel button
  cancel_btn_ = new QPushButton(this);
  cancel_btn_->setIcon(icon::Error);
  middle_layout->addWidget(cancel_btn_);

  // Create stack with error label and elapsed/remaining time
  status_stack_ = new QStackedWidget();
  status_stack_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
  layout->addWidget(status_stack_);

  // Create elapsed timer
  elapsed_timer_lbl_ = new ElapsedCounterWidget();
  status_stack_->addWidget(elapsed_timer_lbl_);

  // Create error label
  task_error_lbl_ = new QLabel(this);
  status_stack_->addWidget(task_error_lbl_);

  // Set up elapsed timer
  status_stack_->setCurrentWidget(elapsed_timer_lbl_);

  // Connect to the task
  connect(task_, &Task::Started, elapsed_timer_lbl_, qOverload<qint64>(&ElapsedCounterWidget::Start));
  connect(task_, &Task::ProgressChanged, this, &TaskViewItem::UpdateProgress);
  connect(cancel_btn_, &QPushButton::clicked, this, [this] { emit TaskCancelled(task_); });
}

void TaskViewItem::Failed() {
  status_stack_->setCurrentWidget(task_error_lbl_);
  task_error_lbl_->setStyleSheet("color: red");
  task_error_lbl_->setText(tr("Error: %1").arg(task_->GetError()));
}

void TaskViewItem::UpdateProgress(double d) {
  progress_bar_->setValue(qRound(100.0 * d));
  elapsed_timer_lbl_->SetProgress(d);
}

}  // namespace olive
