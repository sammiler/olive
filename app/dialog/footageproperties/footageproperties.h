

#ifndef MEDIAPROPERTIESDIALOG_H
#define MEDIAPROPERTIESDIALOG_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QListWidget>
#include <QStackedWidget>

#include "node/project/footage/footage.h"
#include "undo/undocommand.h"

namespace olive {

/**
 * @brief The MediaPropertiesDialog class
 *
 * A dialog for setting properties on Media. This can be loaded from any part of the application provided it's given
 * a valid Media object.
 */
class FootagePropertiesDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief MediaPropertiesDialog Constructor
   *
   * @param parent
   *
   * QWidget parent. Usually MainWindow or Project panel.
   *
   * @param i
   *
   * Media object to set properties for.
   */
  FootagePropertiesDialog(QWidget* parent, Footage* footage);

 private:
  class StreamEnableChangeCommand : public UndoCommand {
   public:
    StreamEnableChangeCommand(Footage* footage, Track::Type type, int index_in_type, bool enabled);

    [[nodiscard]] Project* GetRelevantProject() const override;

   protected:
    void redo() override;
    void undo() override;

   private:
    Footage* footage_;
    Track::Type type_;
    int index_;

    bool old_enabled_{};
    bool new_enabled_;
  };

  /**
   * @brief Stack of widgets that changes based on whether the stream is a video or audio stream
   */
  QStackedWidget* stacked_widget_;

  /**
   * @brief Media name text field
   */
  QLineEdit* footage_name_field_;

  /**
   * @brief Internal pointer to Media object (set in constructor)
   */
  Footage* footage_;

  /**
   * @brief A list widget for listing the tracks in Media
   */
  QListWidget* track_list;

  /**
   * @brief Frame rate to conform to
   */
  QDoubleSpinBox* conform_fr{};

 private slots:
  /**
   * @brief Overridden accept function for saving the properties back to the Media class
   */
  void accept() override;
};

}  // namespace olive

#endif  // MEDIAPROPERTIESDIALOG_H
