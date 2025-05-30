#include "numericsliderbase.h"

#include "common/qtutils.h"
#include "config/config.h"
#include "core.h"

namespace olive {

bool NumericSliderBase::effects_slider_is_being_dragged_ = false;

NumericSliderBase::NumericSliderBase(QWidget *parent)
    : SliderBase(parent),
      drag_ladder_(nullptr),
      ladder_element_count_(0),
      dragged_(false),
      has_min_(false),
      has_max_(false),
      dragged_diff_(0),
      drag_multiplier_(1.0),
      setting_drag_value_(false) {
  // Numeric sliders are draggable, so we have a cursor that indicates that
  setCursor(Qt::SizeHorCursor);

  connect(label(), &SliderLabel::LabelPressed, this, &NumericSliderBase::LabelPressed);
}

void NumericSliderBase::SetDragMultiplier(const double &d) { drag_multiplier_ = d; }

void NumericSliderBase::LabelPressed() {
  drag_ladder_ = new SliderLadder(drag_multiplier_, ladder_element_count_, GetFormattedValueToString(99999999));
  connect(drag_ladder_, &SliderLadder::DraggedByValue, this, &NumericSliderBase::LadderDragged);
  connect(drag_ladder_, &SliderLadder::Released, this, &NumericSliderBase::LadderReleased);

  drag_ladder_->SetValue(GetFormattedValueToString());
  drag_ladder_->resize(drag_ladder_->sizeHint());
  RepositionLadder();
  drag_ladder_->show();

  drag_start_value_ = GetValueInternal();
}

void NumericSliderBase::LadderDragged(int value, double multiplier) {
  dragged_ = true;

  dragged_diff_ += value * multiplier;

  // Store current value to try and prevent any unnecessary signalling if the value doesn't change
  QVariant pre_set_value = GetValueInternal();

  setting_drag_value_ = true;
  SetValueInternal(AdjustDragDistanceInternal(drag_start_value_, dragged_diff_));
  setting_drag_value_ = false;

  if (GetValueInternal() != pre_set_value) {
    // We retrieve the value instead of storing it ourselves because SetValueInternal may do extra
    // processing (such as clamping).
    drag_ladder_->SetValue(GetFormattedValueToString());

    if (!UsingLadders()) {
      RepositionLadder();
    }

    ValueSignalEvent(GetValueInternal());
  }
}

void NumericSliderBase::LadderReleased() {
  drag_ladder_->deleteLater();
  drag_ladder_ = nullptr;
  dragged_diff_ = 0;

  if (dragged_) {
    // This was a drag, send another value changed event
    ValueSignalEvent(GetValueInternal());

    dragged_ = false;
  } else {
    ShowEditor();
  }
}

void NumericSliderBase::RepositionLadder() {
  if (drag_ladder_) {
    if (UsingLadders()) {
      drag_ladder_->move(QCursor::pos() - QPoint(drag_ladder_->width() / 2, drag_ladder_->height() / 2));
    } else {
      QPoint label_global_pos = label()->mapToGlobal(label()->pos());
      int text_width = QtUtils::QFontMetricsWidth(label()->fontMetrics(), label()->text());

      if (label()->alignment() & Qt::AlignRight) {
        label_global_pos.setX(label_global_pos.x() + label()->width() - text_width);
      } else if (label()->alignment() & Qt::AlignHCenter) {
        label_global_pos.setX(label_global_pos.x() + label()->width() / 2 - text_width / 2);
      }

      int ladder_x = label_global_pos.x() + text_width / 2 - drag_ladder_->width() / 2;
      int ladder_y = label_global_pos.y() + label()->height() / 2 - drag_ladder_->height() / 2;

      drag_ladder_->move(ladder_x, ladder_y);
    }

    drag_ladder_->StartListeningToMouseInput();
  }
}

bool NumericSliderBase::IsDragging() const { return drag_ladder_; }

bool NumericSliderBase::UsingLadders() const {
  return ladder_element_count_ > 0 && OLIVE_CONFIG("UseSliderLadders").toBool();
}

QVariant NumericSliderBase::AdjustValue(const QVariant &value) const {
  // Clamps between min/max
  if (has_min_ && ValueLessThan(value, min_value_)) {
    return min_value_;
  } else if (has_max_ && ValueGreaterThan(value, max_value_)) {
    return max_value_;
  }

  return value;
}

void NumericSliderBase::SetOffset(const QVariant &v) {
  offset_ = v;

  UpdateLabel();
}

QVariant NumericSliderBase::AdjustDragDistanceInternal(const QVariant &start, const double &drag) const {
  return start.toDouble() + drag;
}

void NumericSliderBase::SetMinimumInternal(const QVariant &v) {
  min_value_ = v;
  has_min_ = true;

  // Limit value by this new minimum value
  if (ValueLessThan(GetValueInternal(), min_value_)) {
    SetValueInternal(min_value_);
  }
}

void NumericSliderBase::SetMaximumInternal(const QVariant &v) {
  max_value_ = v;
  has_max_ = true;

  // Limit value by this new maximum value
  if (ValueGreaterThan(GetValueInternal(), max_value_)) {
    SetValueInternal(max_value_);
  }
}

bool NumericSliderBase::ValueGreaterThan(const QVariant &lhs, const QVariant &rhs) const {
  return lhs.toDouble() > rhs.toDouble();
}

bool NumericSliderBase::ValueLessThan(const QVariant &lhs, const QVariant &rhs) const {
  return lhs.toDouble() < rhs.toDouble();
}

bool NumericSliderBase::CanSetValue() const { return !IsDragging() || setting_drag_value_; }

}  // namespace olive
