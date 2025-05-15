

#include "block.h"

#include <QDebug>

#include "node/inputdragger.h"
#include "node/output/track/track.h"
#include "transition/transition.h"
#include "widget/slider/rationalslider.h"

namespace olive {

#define super Node

const QString Block::kLengthInput = QStringLiteral("length_in");

Block::Block() : previous_(nullptr), next_(nullptr), track_(nullptr) {
  AddInput(kLengthInput, NodeValue::kRational,
           InputFlags(kInputFlagNotConnectable | kInputFlagNotKeyframable | kInputFlagHidden));
  SetInputProperty(kLengthInput, QStringLiteral("min"), QVariant::fromValue(rational(0, 1)));
  SetInputProperty(kLengthInput, QStringLiteral("view"), RationalSlider::kTime);
  SetInputProperty(kLengthInput, QStringLiteral("viewlock"), true);

  SetInputFlag(kEnabledInput, kInputFlagNotConnectable);
  SetInputFlag(kEnabledInput, kInputFlagNotKeyframable);

  SetFlag(kDontShowInParamView);
}

QVector<Node::CategoryID> Block::Category() const { return {kCategoryTimeline}; }

rational Block::length() const { return GetStandardValue(kLengthInput).value<rational>(); }

void Block::set_length_and_media_out(const rational &length) {
  if (length == this->length()) {
    return;
  }

  set_length_internal(length);
}

void Block::set_length_and_media_in(const rational &length) {
  if (length == this->length()) {
    return;
  }

  // Set the length without setting media out
  set_length_internal(length);
}

bool Block::is_enabled() const { return GetStandardValue(kEnabledInput).toBool(); }

void Block::set_enabled(bool e) {
  SetStandardValue(kEnabledInput, e);

  emit EnabledChanged();
}

void Block::InputValueChangedEvent(const QString &input, int element) {
  super::InputValueChangedEvent(input, element);

  if (input == kLengthInput) {
    emit LengthChanged();
  } else if (input == kEnabledInput) {
    emit EnabledChanged();
  }
}

void Block::set_length_internal(const rational &length) { SetStandardValue(kLengthInput, QVariant::fromValue(length)); }

void Block::Retranslate() {
  super::Retranslate();

  SetInputName(kLengthInput, tr("Length"));
  SetInputName(kEnabledInput, tr("Enabled"));
}

void Block::InvalidateCache(const TimeRange &range, const QString &from, int element, InvalidateCacheOptions options) {
  TimeRange r;

  if (from == kLengthInput) {
    // We must intercept the signal here
    r = TimeRange(qMin(length(), last_length_), RATIONAL_MAX);

    if (!NodeInputDragger::IsInputBeingDragged()) {
      last_length_ = length();
    }

    options.insert(QStringLiteral("lengthevent"), true);
  } else {
    r = range;
  }

  super::InvalidateCache(r, from, element, options);
}

void Block::set_previous_next(Block *previous, Block *next) {
  if (previous) {
    previous->set_next(next);
  }
  if (next) {
    next->set_previous(previous);
  }
}

}  // namespace olive
