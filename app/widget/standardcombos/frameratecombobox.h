#ifndef FRAMERATECOMBOBOX_H
#define FRAMERATECOMBOBOX_H

#include <QComboBox>
#include <QEvent>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>

#include "common/qtutils.h"
#include "render/videoparams.h"
namespace olive {

class FrameRateComboBox : public QWidget {
  Q_OBJECT
 public:
  explicit FrameRateComboBox(QWidget* parent = nullptr) : QWidget(parent) {
    inner_ = new QComboBox();

    auto* layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(inner_);

    RepopulateList();

    old_index_ = 0;

    connect(inner_, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &FrameRateComboBox::IndexChanged);
  }

  [[nodiscard]] rational GetFrameRate() const {
    if (inner_->currentIndex() == inner_->count() - 1) {
      return custom_rate_;
    } else {
      return inner_->currentData().value<rational>();
    }
  }

  void SetFrameRate(const rational& r) {
    int standard_rates = inner_->count() - 1;
    for (int i = 0; i < standard_rates; i++) {
      if (inner_->itemData(i).value<rational>() == r) {
        // Set standard frame rate
        old_index_ = i;
        SetInnerIndexWithoutSignal(i);
        return;
      }
    }

    // If we're here, set a custom rate
    custom_rate_ = r;
    old_index_ = inner_->count() - 1;
    SetInnerIndexWithoutSignal(old_index_);
    RepopulateList();
  }

 signals:
  void FrameRateChanged(const rational& frame_rate);

 protected:
  void changeEvent(QEvent* event) override {
    QWidget::changeEvent(event);

    if (event->type() == QEvent::LanguageChange) {
      RepopulateList();
    }
  }

 private slots:
  void IndexChanged(int index) {
    if (index == inner_->count() - 1) {
      // Custom
      QString s;
      bool ok;

      if (!custom_rate_.isNull()) {
        s = QString::number(custom_rate_.toDouble());
      }

      while (true) {
        s = QInputDialog::getText(this, tr("Custom Frame Rate"), tr("Enter custom frame rate:"), QLineEdit::Normal, s,
                                  &ok);

        if (ok) {
          rational r;

          // Try converting to double, assuming most users will input frame rates this way
          double d = s.toDouble(&ok);

          if (ok) {
            // Try converting from double
            r = rational::fromDouble(d, &ok);
          } else {
            // Try converting to rational in case someone formatted that way
            r = rational::fromString(s.toUtf8().constData(), &ok);
          }

          if (ok) {
            custom_rate_ = r;
            emit FrameRateChanged(r);
            old_index_ = index;
            RepopulateList();
            break;

          } else {
            // Show message and continue loop
            QMessageBox::critical(this, tr("Invalid Input"), tr("Failed to convert \"%1\" to a frame rate.").arg(s));
          }

        } else {
          // User cancelled, revert to original value
          SetInnerIndexWithoutSignal(old_index_);
          break;
        }
      }
    } else {
      old_index_ = index;
      emit FrameRateChanged(GetFrameRate());
    }
  }

 private:
  void RepopulateList() {
    int temp_index = inner_->currentIndex();

    inner_->blockSignals(true);

    inner_->clear();

    foreach (const rational& fr, VideoParams::kSupportedFrameRates) {
      inner_->addItem(VideoParams::FrameRateToString(fr), QVariant::fromValue(fr));
    }

    if (custom_rate_.isNull()) {
      inner_->addItem(tr("Custom..."));
    } else {
      inner_->addItem(tr("Custom (%1)").arg(VideoParams::FrameRateToString(custom_rate_)));
    }

    inner_->setCurrentIndex(temp_index);

    inner_->blockSignals(false);
  }

  void SetInnerIndexWithoutSignal(int index) {
    inner_->blockSignals(true);
    inner_->setCurrentIndex(index);
    inner_->blockSignals(false);
  }

  QComboBox* inner_;

  rational custom_rate_;

  int old_index_;
};

}  // namespace olive

#endif  // FRAMERATECOMBOBOX_H
