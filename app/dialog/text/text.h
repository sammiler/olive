#ifndef RICHTEXTDIALOG_H
#define RICHTEXTDIALOG_H

#include <QDialog>
#include <QFontComboBox>
#include <QPlainTextEdit>

#include "common/define.h"
#include "widget/slider/floatslider.h"

namespace olive {

class TextDialog : public QDialog {
  Q_OBJECT
 public:
  explicit TextDialog(const QString& start, QWidget* parent = nullptr);

  [[nodiscard]] QString text() const { return text_edit_->toPlainText(); }

 private:
  QPlainTextEdit* text_edit_;
};

}  // namespace olive

#endif  // RICHTEXTDIALOG_H
