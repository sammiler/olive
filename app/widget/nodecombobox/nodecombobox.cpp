#include "nodecombobox.h"

#include <QAction>
#include <QDebug>
#include <QEvent>

#include "node/factory.h"
#include "ui/icons/icons.h"
#include "widget/menu/menu.h"

namespace olive {

NodeComboBox::NodeComboBox(QWidget *parent) : QComboBox(parent) {}

void NodeComboBox::showPopup() {
  Menu *m = NodeFactory::CreateMenu(this, true);

  QAction *selected = m->exec(parentWidget()->mapToGlobal(pos()));

  if (selected) {
    QString new_id = NodeFactory::GetIDFromMenuAction(selected);

    SetNodeInternal(new_id, true);
  }

  delete m;
}

const QString &NodeComboBox::GetSelectedNode() const { return selected_id_; }

void NodeComboBox::SetNode(const QString &id) { SetNodeInternal(id, false); }

void NodeComboBox::changeEvent(QEvent *e) {
  if (e->type() == QEvent::LanguageChange) {
    UpdateText();
  }

  QComboBox::changeEvent(e);
}

void NodeComboBox::UpdateText() {
  clear();

  if (!selected_id_.isEmpty()) {
    addItem(NodeFactory::GetNameFromID(selected_id_));
  }
}

void NodeComboBox::SetNodeInternal(const QString &id, bool emit_signal) {
  if (selected_id_ != id) {
    selected_id_ = id;

    UpdateText();

    if (emit_signal) {
      emit NodeChanged(selected_id_);
    }
  }
}

}  // namespace olive
