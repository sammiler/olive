#include "keysequenceeditor.h"

#include <QAction>
#include <QKeyEvent>

namespace olive {

#define super QKeySequenceEdit

KeySequenceEditor::KeySequenceEditor(QWidget* parent, QAction* a) : super(parent), action(a) {
  setKeySequence(action->shortcut());
}

void KeySequenceEditor::set_action_shortcut() { action->setShortcut(keySequence()); }

void KeySequenceEditor::reset_to_default() { setKeySequence(action->property("keydefault").toString()); }

QString KeySequenceEditor::action_name() { return action->property("id").toString(); }

QString KeySequenceEditor::export_shortcut() {
  QKeySequence ks = keySequence();
  if (ks != action->property("keydefault").value<QKeySequence>()) {
    return action->property("id").toString() + "\t" + ks.toString();
  }
  return nullptr;
}

void KeySequenceEditor::keyPressEvent(QKeyEvent* e) {
  if (e->key() == Qt::Key_Backspace) {
    clear();
  } else if (e->key() == Qt::Key_Escape) {
    e->ignore();
  } else {
    super::keyPressEvent(e);
  }
}

void KeySequenceEditor::keyReleaseEvent(QKeyEvent* e) {
  if (e->key() == Qt::Key_Backspace) {
    // Do nothing
  } else if (e->key() == Qt::Key_Escape) {
    e->ignore();
  } else {
    super::keyReleaseEvent(e);
  }
}

}  // namespace olive
