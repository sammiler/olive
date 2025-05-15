

#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>

#include "common/define.h"

namespace olive {

class ClickableLabel : public QLabel {
  Q_OBJECT
 public:
  explicit ClickableLabel(const QString& text, QWidget* parent = nullptr);
  explicit ClickableLabel(QWidget* parent = nullptr);

 protected:
  void mouseReleaseEvent(QMouseEvent* event) override;
  void mouseDoubleClickEvent(QMouseEvent* event) override;

 signals:
  void MouseClicked();
  void MouseDoubleClicked();
};

}  // namespace olive

#endif  // CLICKABLELABEL_H
