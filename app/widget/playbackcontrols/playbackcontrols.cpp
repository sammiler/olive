#include "playbackcontrols.h"

#include <QDebug>
#include <QEvent>
#include <QHBoxLayout>

#include "config/config.h"
#include "core.h"
#include "ui/icons/icons.h"

namespace olive {

PlaybackControls::PlaybackControls(QWidget* parent) : QWidget(parent), time_base_(0) {
  // Create lower controls
  auto* lower_control_layout = new QHBoxLayout(this);
  lower_control_layout->setSpacing(0);
  lower_control_layout->setContentsMargins(0, 0, 0, 0);

  QSizePolicy lower_container_size_policy(QSizePolicy::Maximum, QSizePolicy::Expanding);
  lower_container_size_policy.setHorizontalStretch(1);

  // In the lower-left, we create a current timecode label wrapped in a QWidget for fixed sizing
  lower_left_container_ = new QWidget();
  lower_left_container_->setVisible(false);
  lower_left_container_->setSizePolicy(lower_container_size_policy);

  lower_control_layout->addWidget(lower_left_container_);

  auto* lower_left_layout = new QHBoxLayout(lower_left_container_);
  lower_left_layout->setSpacing(0);
  lower_left_layout->setContentsMargins(0, 0, 0, 0);

  cur_tc_lbl_ = new RationalSlider();
  cur_tc_lbl_->SetDisplayType(RationalSlider::kTime);
  cur_tc_lbl_->SetMinimum(rational(0));
  connect(cur_tc_lbl_, &RationalSlider::ValueChanged, this, &PlaybackControls::TimeChanged);
  lower_left_layout->addWidget(cur_tc_lbl_);
  lower_left_layout->addStretch();

  // This is only here
  auto* blank_widget = new QWidget();
  // new QHBoxLayout(blank_widget);
  blank_widget->setSizePolicy(lower_container_size_policy);
  lower_control_layout->addWidget(blank_widget);

  // In the lower-middle, we create playback control buttons
  auto* lower_middle_container = new QWidget();
  lower_middle_container->setSizePolicy(lower_container_size_policy);
  lower_control_layout->addWidget(lower_middle_container);

  auto* lower_middle_layout = new QHBoxLayout(lower_middle_container);
  lower_middle_layout->setSpacing(0);
  lower_middle_layout->setContentsMargins(0, 0, 0, 0);
  lower_middle_layout->addStretch();

  QSizePolicy btn_sz_policy(QSizePolicy::Maximum, QSizePolicy::Preferred);

  // Go To Start Button
  go_to_start_btn_ = new QPushButton();
  go_to_start_btn_->setSizePolicy(btn_sz_policy);
  lower_middle_layout->addWidget(go_to_start_btn_);
  connect(go_to_start_btn_, &QPushButton::clicked, this, &PlaybackControls::BeginClicked);

  // Prev Frame Button
  prev_frame_btn_ = new QPushButton();
  prev_frame_btn_->setSizePolicy(btn_sz_policy);
  lower_middle_layout->addWidget(prev_frame_btn_);
  connect(prev_frame_btn_, &QPushButton::clicked, this, &PlaybackControls::PrevFrameClicked);

  // Play/Pause Button
  playpause_stack_ = new QStackedWidget();
  playpause_stack_->setSizePolicy(btn_sz_policy);
  lower_middle_layout->addWidget(playpause_stack_);

  play_btn_ = new QPushButton();
  playpause_stack_->addWidget(play_btn_);
  connect(play_btn_, &QPushButton::clicked, this, &PlaybackControls::PlayClicked);

  pause_btn_ = new QPushButton();
  playpause_stack_->addWidget(pause_btn_);
  connect(pause_btn_, &QPushButton::clicked, this, &PlaybackControls::PauseClicked);

  // Default to showing play button
  playpause_stack_->setCurrentWidget(play_btn_);

  // Next Frame Button
  next_frame_btn_ = new QPushButton();
  next_frame_btn_->setSizePolicy(btn_sz_policy);
  lower_middle_layout->addWidget(next_frame_btn_);
  connect(next_frame_btn_, &QPushButton::clicked, this, &PlaybackControls::NextFrameClicked);

  // Go To End Button
  go_to_end_btn_ = new QPushButton();
  go_to_end_btn_->setSizePolicy(btn_sz_policy);
  lower_middle_layout->addWidget(go_to_end_btn_);
  connect(go_to_end_btn_, &QPushButton::clicked, this, &PlaybackControls::EndClicked);

  lower_middle_layout->addStretch();

  auto* av_btn_widget = new QWidget();
  av_btn_widget->setSizePolicy(lower_container_size_policy);
  auto* av_btn_layout = new QHBoxLayout(av_btn_widget);
  av_btn_layout->setSpacing(0);
  av_btn_layout->setContentsMargins(0, 0, 0, 0);
  video_drag_btn_ = new DragButton();
  connect(video_drag_btn_, &QPushButton::clicked, this, &PlaybackControls::VideoClicked);
  connect(video_drag_btn_, &DragButton::DragStarted, this, &PlaybackControls::VideoDragged);
  av_btn_layout->addWidget(video_drag_btn_);
  audio_drag_btn_ = new DragButton();
  connect(audio_drag_btn_, &QPushButton::clicked, this, &PlaybackControls::AudioClicked);
  connect(audio_drag_btn_, &DragButton::DragStarted, this, &PlaybackControls::AudioDragged);
  av_btn_layout->addWidget(audio_drag_btn_);
  lower_control_layout->addWidget(av_btn_widget);

  // The lower-right, we create another timecode label, this time to show the end timecode
  lower_right_container_ = new QWidget();
  lower_right_container_->setVisible(false);
  lower_right_container_->setSizePolicy(lower_container_size_policy);
  lower_control_layout->addWidget(lower_right_container_);

  auto* lower_right_layout = new QHBoxLayout(lower_right_container_);
  lower_right_layout->setSpacing(0);
  lower_right_layout->setContentsMargins(0, 0, 0, 0);

  lower_right_layout->addStretch();
  end_tc_lbl_ = new QLabel();
  lower_right_layout->addWidget(end_tc_lbl_);

  UpdateIcons();

  SetTimebase(rational(0));

  SetAudioVideoDragButtonsVisible(false);

  connect(Core::instance(), &Core::TimecodeDisplayChanged, this, &PlaybackControls::TimecodeChanged);

  play_blink_timer_ = new QTimer(this);
  play_blink_timer_->setInterval(500);
  connect(play_blink_timer_, &QTimer::timeout, this, &PlaybackControls::PlayBlink);
}

void PlaybackControls::SetTimecodeEnabled(bool enabled) {
  lower_left_container_->setVisible(enabled);
  lower_right_container_->setVisible(enabled);
}

void PlaybackControls::SetTimebase(const rational& r) {
  time_base_ = r;
  cur_tc_lbl_->SetTimebase(r);

  cur_tc_lbl_->setVisible(!r.isNull());
  end_tc_lbl_->setVisible(!r.isNull());

  setEnabled(!r.isNull());
}

void PlaybackControls::SetAudioVideoDragButtonsVisible(bool e) {
  video_drag_btn_->setVisible(e);
  audio_drag_btn_->setVisible(e);
}

void PlaybackControls::SetTime(const rational& r) { cur_tc_lbl_->SetValue(r); }

void PlaybackControls::SetEndTime(const rational& r) {
  if (time_base_.isNull()) {
    return;
  }

  end_time_ = r;

  end_tc_lbl_->setText(QString::fromStdString(
      Timecode::time_to_timecode(end_time_, time_base_, Core::instance()->GetTimecodeDisplay())));
}

void PlaybackControls::ShowPauseButton() {
  // Play was clicked, toggle to pause
  playpause_stack_->setCurrentWidget(pause_btn_);
}

void PlaybackControls::ShowPlayButton() { playpause_stack_->setCurrentWidget(play_btn_); }

void PlaybackControls::changeEvent(QEvent* e) {
  QWidget::changeEvent(e);

  if (e->type() == QEvent::StyleChange) {
    UpdateIcons();
  }
}

void PlaybackControls::UpdateIcons() {
  go_to_start_btn_->setIcon(icon::GoToStart);
  prev_frame_btn_->setIcon(icon::PrevFrame);
  play_btn_->setIcon(icon::Play);
  pause_btn_->setIcon(icon::Pause);
  next_frame_btn_->setIcon(icon::NextFrame);
  go_to_end_btn_->setIcon(icon::GoToEnd);
  video_drag_btn_->setIcon(icon::Video);
  audio_drag_btn_->setIcon(icon::Audio);
}

void PlaybackControls::SetButtonRecordingState(QPushButton* btn, bool on) {
  btn->setStyleSheet(on ? QStringLiteral("background: red;") : QString());
}

void PlaybackControls::TimecodeChanged() {
  // Update end time
  SetEndTime(end_time_);
}

void PlaybackControls::PlayBlink() { SetButtonRecordingState(play_btn_, play_btn_->styleSheet().isEmpty()); }

}  // namespace olive
