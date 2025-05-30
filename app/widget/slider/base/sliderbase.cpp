#include "sliderbase.h"

#include <QDebug>
#include <QEvent>
#include <QMessageBox>

#include "common/qtutils.h"
#include "core.h"
#include "window/mainwindow/mainwindow.h"

namespace olive {

#define super QStackedWidget

SliderBase::SliderBase(QWidget *parent) : super(parent), tristate_(false), format_plural_(false) {
  // Standard (non-numeric) sliders are not draggable, so we indicate as such
  setCursor(Qt::PointingHandCursor);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

  label_ = new SliderLabel(this);
  addWidget(label_);

  editor_ = new FocusableLineEdit(this);
  addWidget(editor_);

  connect(label_, &SliderLabel::focused, this, &SliderBase::ShowEditor);
  connect(label_, &SliderLabel::RequestReset, this, &SliderBase::ResetValue);
  connect(editor_, &FocusableLineEdit::Confirmed, this, &SliderBase::LineEditConfirmed);
  connect(editor_, &FocusableLineEdit::Cancelled, this, &SliderBase::LineEditCancelled);
}

void SliderBase::SetAlignment(Qt::Alignment alignment) {
  label_->setAlignment(alignment);
  editor_->setAlignment(alignment);
}

bool SliderBase::IsTristate() const { return tristate_; }

void SliderBase::SetTristate() {
  tristate_ = true;
  UpdateLabel();
}

const QVariant &SliderBase::GetValueInternal() const { return value_; }

void SliderBase::SetValueInternal(const QVariant &v) {
  if (!CanSetValue()) {
    return;
  }

  value_ = AdjustValue(v);

  // Disable tristate
  tristate_ = false;

  UpdateLabel();
}

void SliderBase::SetDefaultValue(const QVariant &v) { default_value_ = v; }

void SliderBase::changeEvent(QEvent *e) {
  if (e->type() == QEvent::LanguageChange) {
    UpdateLabel();
  }
  super::changeEvent(e);
}

bool SliderBase::GetLabelSubstitution(const QVariant &v, QString *out) const {
  for (auto it = label_substitutions_.constBegin(); it != label_substitutions_.constEnd(); it++) {
    if (it->first == v) {
      *out = it->second;
      return true;
    }
  }

  return false;
}

void SliderBase::UpdateLabel() {
  QString s;

  if (tristate_) {
    s = tr("---");
  } else if (GetLabelSubstitution(GetValueInternal(), &s)) {
    // String will already be set, just pass through
  } else {
    s = GetFormattedValueToString();
  }

  label_->setText(s);
}

QVariant SliderBase::AdjustValue(const QVariant &value) const { return value; }

bool SliderBase::CanSetValue() const { return true; }

void SliderBase::ValueSignalEvent(const QVariant &value) { Q_UNUSED(value) }

void SliderBase::ShowEditor() {
  // This was a simple click
  // Load label's text into editor
  editor_->setText(ValueToString(value_));

  // Show editor
  setCurrentWidget(editor_);

  // Select all text in the editor
  editor_->setFocus();
  editor_->selectAll();
}

void SliderBase::LineEditConfirmed() {
  bool is_valid = true;
  QVariant test_val = StringToValue(editor_->text(), &is_valid);

  // Ensure editor doesn't signal that the focus is lost
  editor_->blockSignals(true);
  label_->blockSignals(true);

  if (is_valid) {
    SetValueInternal(test_val);

    setCurrentWidget(label_);

    ValueSignalEvent(value_);
  } else {
    QMessageBox::critical(this, tr("Invalid Value"), tr("The entered value is not valid for this field."),
                          QMessageBox::Ok);

    // Refocus editor
    editor_->setFocus();
  }

  editor_->blockSignals(false);
  label_->blockSignals(false);
}

void SliderBase::LineEditCancelled() {
  // Ensure editor doesn't signal that the focus is lost
  editor_->blockSignals(true);
  label_->blockSignals(true);

  // Set widget back to label
  setCurrentWidget(label_);

  editor_->blockSignals(false);
  label_->blockSignals(false);
}

void SliderBase::ResetValue() {
  if (default_value_.isValid()) {
    SetValueInternal(default_value_);
    ValueSignalEvent(value_);
  }
}

void SliderBase::SetFormat(const QString &s, const bool plural) {
  custom_format_ = s;
  format_plural_ = plural;
  UpdateLabel();
}

void SliderBase::ClearFormat() {
  custom_format_.clear();
  UpdateLabel();
}

bool SliderBase::IsFormatPlural() const { return format_plural_; }

QString SliderBase::GetFormat() const {
  if (custom_format_.isEmpty()) {
    return QStringLiteral("%1");
  } else {
    return custom_format_;
  }
}

QString SliderBase::GetFormattedValueToString() const { return GetFormattedValueToString(GetValueInternal()); }

QString SliderBase::GetFormattedValueToString(const QVariant &v) const {
  if (format_plural_) {
    return tr(GetFormat().toUtf8().constData(), nullptr, v.toInt());
  } else {
    return GetFormat().arg(ValueToString(v));
  }
}

}  // namespace olive
