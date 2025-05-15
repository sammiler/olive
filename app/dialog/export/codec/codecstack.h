#ifndef CODECSTACK_H
#define CODECSTACK_H

#include <QStackedWidget>

namespace olive {

class CodecStack : public QStackedWidget {
  Q_OBJECT
 public:
  explicit CodecStack(QWidget *parent = nullptr);

  void addWidget(QWidget *widget);

 signals:

 private slots:
  void OnChange(int index);
};

}  // namespace olive

#endif  // CODECSTACK_H
