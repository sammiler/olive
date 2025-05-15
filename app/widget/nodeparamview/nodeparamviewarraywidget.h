

#ifndef NODEPARAMVIEWARRAYWIDGET_H
#define NODEPARAMVIEWARRAYWIDGET_H

#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "node/param.h"

namespace olive {

class NodeParamViewArrayButton : public QPushButton {
  Q_OBJECT
 public:
  enum Type { kAdd, kRemove };

  explicit NodeParamViewArrayButton(Type type, QWidget* parent = nullptr);

 protected:
  void changeEvent(QEvent* event) override;

 private:
  void Retranslate();

  Type type_;
};

class NodeParamViewArrayWidget : public QWidget {
  Q_OBJECT
 public:
  NodeParamViewArrayWidget(Node* node, QString input, QWidget* parent = nullptr);

 signals:
  void DoubleClicked();

 protected:
  void mouseDoubleClickEvent(QMouseEvent* event) override;

 private:
  Node* node_;

  QString input_;

  QLabel* count_lbl_;

 private slots:
  void UpdateCounter(const QString& input, int old_size, int new_size);
};

}  // namespace olive

#endif  // NODEPARAMVIEWARRAYWIDGET_H
