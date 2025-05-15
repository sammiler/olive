#include "nodeparamviewarraywidget.h"

#include <QEvent>
#include <QHBoxLayout>
#include <utility>

#include "node/node.h"

namespace olive {

NodeParamViewArrayWidget::NodeParamViewArrayWidget(Node *node, QString input, QWidget *parent)
    : QWidget(parent), node_(node), input_(std::move(input)) {
  auto *layout = new QHBoxLayout(this);

  count_lbl_ = new QLabel();
  layout->addWidget(count_lbl_);

  connect(node_, &Node::InputArraySizeChanged, this, &NodeParamViewArrayWidget::UpdateCounter);

  UpdateCounter(input_, 0, node_->InputArraySize(input_));
}

void NodeParamViewArrayWidget::mouseDoubleClickEvent(QMouseEvent *event) {
  QWidget::mouseDoubleClickEvent(event);

  emit DoubleClicked();
}

void NodeParamViewArrayWidget::UpdateCounter(const QString &input, int old_size, int new_size) {
  Q_UNUSED(old_size)
  if (input == input_) {
    count_lbl_->setText(tr("%n element(s)", nullptr, new_size));
  }
}

NodeParamViewArrayButton::NodeParamViewArrayButton(NodeParamViewArrayButton::Type type, QWidget *parent)
    : QPushButton(parent), type_(type) {
  Retranslate();

  int sz = sizeHint().height() / 3 * 2;
  setFixedSize(sz, sz);
}

void NodeParamViewArrayButton::changeEvent(QEvent *event) {
  if (event->type() == QEvent::LanguageChange) {
    Retranslate();
  }

  QPushButton::changeEvent(event);
}

void NodeParamViewArrayButton::Retranslate() {
  if (type_ == kAdd) {
    setText(tr("+"));
  } else {
    setText(tr("-"));
  }
}

}  // namespace olive
