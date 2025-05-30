#include "sliderladder.h"

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QScreen>
#include <QVBoxLayout>
#include <QtMath>

#ifdef Q_OS_MAC
#include <ApplicationServices/ApplicationServices.h>
#endif

#include "common/lerp.h"
#include "common/qtutils.h"
#include "config/config.h"

namespace olive {

SliderLadder::SliderLadder(double drag_multiplier, int nb_outer_values, const QString &width_hint, QWidget *parent)
    : QFrame(parent, Qt::Popup) {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  setFrameShape(QFrame::Box);
  setLineWidth(1);

  if (!OLIVE_CONFIG("UseSliderLadders").toBool()) {
    nb_outer_values = 0;
  }

  for (int i = nb_outer_values - 1; i >= 0; i--) {
    elements_.append(new SliderLadderElement(qPow(10, i + 1) * drag_multiplier, width_hint));
  }

  // Create center entry
  auto *start_element = new SliderLadderElement(drag_multiplier, width_hint);
  active_element_ = elements_.size();
  start_element->SetHighlighted(true);
  elements_.append(start_element);

  for (int i = 0; i < nb_outer_values; i++) {
    elements_.append(new SliderLadderElement(qPow(10, -i - 1) * drag_multiplier, width_hint));
  }

  foreach (SliderLadderElement *e, elements_) {
    layout->addWidget(e);
  }

  if (elements_.size() == 1) {
    elements_.first()->SetMultiplierVisible(false);
  }

  drag_timer_.setInterval(10);
  connect(&drag_timer_, &QTimer::timeout, this, &SliderLadder::TimerUpdate);

  screen_ = nullptr;
  foreach (QScreen *screen, qApp->screens()) {
    if (screen->geometry().contains(QCursor::pos())) {
      screen_ = screen;
      break;
    }
  }

  if (UsingLadders()) {
    drag_start_x_ = -1;
    wrap_count_ = 0;
  } else {
#if defined(Q_OS_MAC)
    CGAssociateMouseAndMouseCursorPosition(false);
    CGDisplayHideCursor(kCGDirectMainDisplay);
    CGGetLastMouseDelta(nullptr, nullptr);
#else
    drag_start_x_ = QCursor::pos().x();
    drag_start_y_ = QCursor::pos().y();

    dynamic_cast<QGuiApplication *>(QApplication::instance())->setOverrideCursor(Qt::BlankCursor);
#endif
  }
}

SliderLadder::~SliderLadder() {
  if (UsingLadders()) {
    if (wrap_count_ != 0) {
      // If wrapped, restore cursor to ladder
      QCursor::setPos(pos() + rect().center());
    }
  } else {
#if defined(Q_OS_MAC)
    CGAssociateMouseAndMouseCursorPosition(true);
    CGDisplayShowCursor(kCGDirectMainDisplay);
#else
    dynamic_cast<QGuiApplication *>(QApplication::instance())->restoreOverrideCursor();
#endif
  }
}

void SliderLadder::SetValue(const QString &s) {
  foreach (SliderLadderElement *e, elements_) {
    e->SetValue(s);
  }
}

void SliderLadder::StartListeningToMouseInput() {
  QMetaObject::invokeMethod(&drag_timer_, "start", Qt::QueuedConnection);
}

void SliderLadder::mouseReleaseEvent(QMouseEvent *event) {
  Q_UNUSED(event)

  this->close();
}

void SliderLadder::closeEvent(QCloseEvent *event) {
  Q_UNUSED(event)

  drag_timer_.stop();

  emit Released();

  QFrame::closeEvent(event);
}

void SliderLadder::TimerUpdate() {
  int ladder_left = this->x();
  int ladder_right = this->x() + this->width() - 1;
  int now_pos = QCursor::pos().x();

  if (UsingLadders()) {
    bool is_under_mouse = (now_pos >= ladder_left && now_pos <= ladder_right && wrap_count_ == 0);

    if (drag_start_x_ != -1 && (is_under_mouse || (drag_start_x_ < ladder_left && now_pos > ladder_right) ||
                                (drag_start_x_ > ladder_right && now_pos < ladder_left))) {
      // We're ending a drag, try to return the value back to its beginning
      int anchor;

      if (drag_start_x_ < ladder_left) {
        anchor = ladder_left;
      } else {
        anchor = ladder_right;
      }

      int makeup_value = anchor - drag_start_x_;
      emit DraggedByValue(makeup_value, elements_.at(active_element_)->GetMultiplier());

      drag_start_x_ = -1;
    }

    if (is_under_mouse) {
      // Determine which element is currently active
      for (int i = 0; i < elements_.size(); i++) {
        if (elements_.at(i)->underMouse()) {
          if (i != active_element_) {
            elements_.at(active_element_)->SetHighlighted(false);
            active_element_ = i;
            elements_.at(active_element_)->SetHighlighted(true);
          }

          break;
        }
      }

    } else {
      if (drag_start_x_ == -1) {
        // Drag is a new leave from the ladder, calculate origin
        if (now_pos < ladder_left) {
          drag_start_x_ = ladder_left;
        } else {
          drag_start_x_ = ladder_right;
        }
      }

      emit DraggedByValue(now_pos - drag_start_x_, elements_.at(active_element_)->GetMultiplier());

      // Determine if cursor is at desktop edge, if so wrap around to other side
      if (screen_) {
        int left = screen_->geometry().left();
        int right = screen_->geometry().right();
        int width = right - left;
        if (now_pos <= left || now_pos >= right) {
          int orig_now_pos = now_pos;
          int orig_wrap = wrap_count_;

          if (now_pos <= left) {
            wrap_count_--;
            now_pos += width;
          } else {
            wrap_count_++;
            now_pos -= width;
          }

          QPoint p(now_pos, QCursor::pos().y());
          QCursor::setPos(p);
          if (QCursor::pos() != p) {
            wrap_count_ = orig_wrap;
            now_pos = orig_now_pos;
          }
        }
      }

      drag_start_x_ = now_pos;
    }

  } else {
    int32_t x_mvmt, y_mvmt;

    // Keep cursor in the same position
#if defined(Q_OS_MAC)
    CGGetLastMouseDelta(&x_mvmt, &y_mvmt);
#else
    QPoint current_pos = QCursor::pos();

    x_mvmt = current_pos.x() - drag_start_x_;
    y_mvmt = current_pos.y() - drag_start_y_;

    QCursor::setPos(QPoint(drag_start_x_, drag_start_y_));
#endif

    if (x_mvmt || y_mvmt) {
      double multiplier = 1.0;

      if (qApp->keyboardModifiers() & Qt::ControlModifier) {
        multiplier *= 0.01;
      }

      if (qApp->keyboardModifiers() & Qt::ShiftModifier) {
        multiplier *= 100.0;
      }

      emit DraggedByValue(x_mvmt + y_mvmt, multiplier);
    }
  }
}

bool SliderLadder::UsingLadders() const { return elements_.size() > 1; }

SliderLadderElement::SliderLadderElement(const double &multiplier, const QString &width_hint, QWidget *parent)
    : QWidget(parent), multiplier_(multiplier), highlighted_(false), multiplier_visible_(true) {
  auto *layout = new QVBoxLayout(this);

  label_ = new QLabel();
  label_->setAlignment(Qt::AlignCenter);
  label_->setFixedWidth(QtUtils::QFontMetricsWidth(label_->fontMetrics(), width_hint));
  layout->addWidget(label_);

  QPalette p = palette();
  QColor highlight_color = palette().text().color();
  highlight_color.setAlpha(64);
  p.setColor(QPalette::Highlight, highlight_color);
  setPalette(p);

  setAutoFillBackground(true);

  UpdateLabel();
}

void SliderLadderElement::SetHighlighted(bool e) {
  highlighted_ = e;

  if (highlighted_) {
    setBackgroundRole(QPalette::Highlight);
  } else {
    setBackgroundRole(QPalette::Window);
  }

  UpdateLabel();
}

void SliderLadderElement::SetValue(const QString &value) {
  value_ = value;

  UpdateLabel();
}

void SliderLadderElement::SetMultiplierVisible(bool e) {
  multiplier_visible_ = e;

  UpdateLabel();
}

void SliderLadderElement::UpdateLabel() {
  if (multiplier_visible_) {
    QString val_text;

    if (highlighted_) {
      val_text = value_;
    }

    label_->setText(QStringLiteral("%1\n%2").arg(QString::number(multiplier_), val_text));
  } else {
    label_->setText(value_);
  }
}

}  // namespace olive
