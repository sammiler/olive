#include "elapsedcounterwidget.h"

#include <olive/core/core.h>
#include <QDateTime>
#include <QHBoxLayout>
#include <cmath>

namespace olive {

using namespace core;

ElapsedCounterWidget::ElapsedCounterWidget(QWidget* parent) : QWidget(parent), last_progress_(0), start_time_(0) {
  auto* layout = new QHBoxLayout(this);
  layout->setSpacing(layout->spacing() * 8);
  layout->setContentsMargins(0, 0, 0, 0);

  elapsed_lbl_ = new QLabel();
  layout->addWidget(elapsed_lbl_);

  remaining_lbl_ = new QLabel();
  layout->addWidget(remaining_lbl_);

  elapsed_timer_.setInterval(500);
  connect(&elapsed_timer_, &QTimer::timeout, this, &ElapsedCounterWidget::UpdateTimers);
  UpdateTimers();
}

void ElapsedCounterWidget::SetProgress(double d) {
  last_progress_ = d;
  UpdateTimers();
}

void ElapsedCounterWidget::Start() { Start(QDateTime::currentMSecsSinceEpoch()); }

void ElapsedCounterWidget::Start(qint64 start_time) {
  start_time_ = start_time;
  elapsed_timer_.start();
  UpdateTimers();
}

void ElapsedCounterWidget::Stop() { elapsed_timer_.stop(); }

void ElapsedCounterWidget::UpdateTimers() {
  int64_t elapsed_ms, remaining_ms;

  if (last_progress_ > 0) {
    elapsed_ms = QDateTime::currentMSecsSinceEpoch() - start_time_;

    double ms_per_progress_unit = elapsed_ms / last_progress_;
    double remaining_progress = 1.0 - last_progress_;

    remaining_ms = std::ceil(ms_per_progress_unit * remaining_progress);
  } else {
    elapsed_ms = 0;
    remaining_ms = 0;
  }

  elapsed_lbl_->setText(tr("Elapsed: %1").arg(QString::fromStdString(Timecode::time_to_string(elapsed_ms))));
  remaining_lbl_->setText(tr("Remaining: %1").arg(QString::fromStdString(Timecode::time_to_string(remaining_ms))));
}

}  // namespace olive
