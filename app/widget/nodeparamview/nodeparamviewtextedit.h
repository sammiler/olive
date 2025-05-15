

#ifndef NODEPARAMVIEWTEXTEDIT_H
#define NODEPARAMVIEWTEXTEDIT_H

#include <QPlainTextEdit>
#include <QPushButton>
#include <QWidget>

#include "common/define.h"

namespace olive {

class NodeParamViewTextEdit : public QWidget {
  Q_OBJECT
 public:
  explicit NodeParamViewTextEdit(QWidget *parent = nullptr);

  [[nodiscard]] QString text() const { return line_edit_->toPlainText(); }

  void SetEditInViewerOnlyMode(bool on);

 public slots:
  void setText(const QString &s) {
    line_edit_->blockSignals(true);
    line_edit_->setPlainText(s);
    line_edit_->blockSignals(false);
  }

  void setTextPreservingCursor(const QString &s) {
    // Save cursor position
    int cursor_pos = line_edit_->textCursor().position();

    // Set text
    this->setText(s);

    // Get new text cursor
    QTextCursor c = line_edit_->textCursor();
    c.setPosition(cursor_pos);
    line_edit_->setTextCursor(c);
  }

 signals:
  void textEdited(const QString &);

  void RequestEditInViewer();

 private:
  QPlainTextEdit *line_edit_;

  QPushButton *edit_btn_;

  QPushButton *edit_in_viewer_btn_;

 private slots:
  void ShowTextDialog();

  void InnerWidgetTextChanged();
};

}  // namespace olive

#endif  // NODEPARAMVIEWTEXTEDIT_H
