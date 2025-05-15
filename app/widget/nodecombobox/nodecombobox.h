

#ifndef NODECOMBOBOX_H
#define NODECOMBOBOX_H

#include <QComboBox>

#include "node/node.h"

namespace olive {

class NodeComboBox : public QComboBox {
  Q_OBJECT
 public:
  explicit NodeComboBox(QWidget* parent = nullptr);

  void showPopup() override;

  [[nodiscard]] const QString& GetSelectedNode() const;

 public slots:
  void SetNode(const QString& id);

 protected:
  void changeEvent(QEvent* e) override;

 signals:
  void NodeChanged(const QString& id);

 private:
  void UpdateText();

  void SetNodeInternal(const QString& id, bool emit_signal);

  QString selected_id_;
};

}  // namespace olive

#endif  // FOOTAGECOMBOBOX_H
