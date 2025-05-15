#ifndef NODEPARAMVIEWITEMBASE_H
#define NODEPARAMVIEWITEMBASE_H

#include <QDockWidget>

#include "node/node.h"
#include "nodeparamviewitemtitlebar.h"

namespace olive {

class NodeParamViewItemBase : public QDockWidget {
  Q_OBJECT
 public:
  explicit NodeParamViewItemBase(QWidget *parent = nullptr);

  void SetHighlighted(bool e) {
    highlighted_ = e;

    update();
  }

  [[nodiscard]] bool IsHighlighted() const { return highlighted_; }

  [[nodiscard]] bool IsExpanded() const;

  static QString GetTitleBarTextFromNode(Node *n);

 public slots:
  void SetExpanded(bool e);

  void ToggleExpanded() { SetExpanded(!IsExpanded()); }

 signals:
  void PinToggled(bool e);

  void ExpandedChanged(bool e);

  void Moved();

  void Clicked();

 protected:
  void SetBody(QWidget *body);

  void paintEvent(QPaintEvent *event) override;

  [[nodiscard]] NodeParamViewItemTitleBar *title_bar() const { return title_bar_; }

  void changeEvent(QEvent *e) override;

  void moveEvent(QMoveEvent *event) override;

  void mousePressEvent(QMouseEvent *e) override;

 protected slots:
  virtual void Retranslate() {}

 private:
  NodeParamViewItemTitleBar *title_bar_;

  QWidget *body_{};

  QWidget *hidden_body_;

  bool highlighted_;
};

}  // namespace olive

#endif  // NODEPARAMVIEWITEMBASE_H
