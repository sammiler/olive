#ifndef KEYSEQUENCEEDITOR_H
#define KEYSEQUENCEEDITOR_H

#include <QKeySequenceEdit>

#include "common/debug.h"

namespace olive {

/**
 * @brief The KeySequenceEditor class
 *
 * Simple derived class of QKeySequenceEdit that attaches to a QAction and provides functions for transferring
 * keyboard shortcuts to and from it.
 */
class KeySequenceEditor : public QKeySequenceEdit {
  Q_OBJECT
 public:
  /**
   * @brief KeySequenceEditor Constructor
   *
   * @param parent
   *
   * QWidget parent.
   *
   * @param a
   *
   * The QAction to link to. This cannot be changed throughout the lifetime of a KeySequenceEditor.
   */
  KeySequenceEditor(QWidget *parent, QAction *a);

  /**
   * @brief Sets the attached QAction's shortcut to the shortcut entered in this field.
   *
   * This is not done automatically in case the user cancels out of the Preferences dialog, in which case the
   * expectation is that the changes made will not be saved. Therefore, this needs to be triggered manually when
   * PreferencesDialog saves.
   */
  void set_action_shortcut();

  /**
   * @brief Set this shortcut back to the QAction's default shortcut
   *
   * Each QAction contains the default shortcut in its `property("default")` and can be used to restore the default
   * "hard-coded" shortcut with this function.
   *
   * This function does not save the default shortcut back into the QAction, it simply loads the default shortcut from
   * the QAction into this edit field. To save it into the QAction, it's necessary to call set_action_shortcut() after
   * calling this function.
   */
  void reset_to_default();

  /**
   * @brief Return attached QAction's unique ID
   *
   * Each of Olive's menu actions has a unique string ID (that, unlike the text, is not translated) for matching with
   * an external shortcut configuration file. The ID is stored in the QAction's `property("id")`. This function returns
   * that ID.
   *
   * @return
   *
   * The QAction's unique ID.
   */
  QString action_name();

  /**
   * @brief Serialize this shortcut entry into a string that can be saved to a file
   *
   * @return
   *
   * A string serialization of this shortcut. The format is "[ID]\t[SEQUENCE]" where [ID] is the attached QAction's
   * unique identifier and [SEQUENCE] is the current keyboard shortcut in the field (NOT necessarily the shortcut in
   * the QAction). If the entered shortcut is the same as the QAction's default shortcut, the return value is empty
   * because a default shortcut does not need to be saved to a file.
   */
  QString export_shortcut();

 protected:
  void keyPressEvent(QKeyEvent *e) override;

  void keyReleaseEvent(QKeyEvent *e) override;

 private:
  /**
   * @brief Internal reference to the linked QAction
   */
  QAction *action;
};

}  // namespace olive

#endif  // KEYSEQUENCEEDITOR_H
