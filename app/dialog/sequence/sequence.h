

#ifndef SEQUENCEDIALOG_H
#define SEQUENCEDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QSpinBox>

#include "node/project/sequence/sequence.h"
#include "sequencedialogparametertab.h"
#include "sequencedialogpresettab.h"
#include "undo/undocommand.h"

namespace olive {

/**
 * @brief A dialog for editing Sequence parameters
 *
 * This dialog exposes all the parameters of a Sequence to users allowing them to set up a Sequence however they wish.
 * A Sequence can be sent to this dialog through the constructor. All fields will be filled using that Sequence's
 * parameters, allowing the user to view and edit them. Accepting the dialog will apply them back to that Sequence,
 * either directly or using an UndoCommand (see SetUndoable()).
 *
 * If creating a new Sequence, the Sequence must still be constructed first before sending it to SequenceDialog.
 * SequenceDialog does not create any new objects. In most cases when creating a new Sequence, editing its parameters
 * with SequenceDialog will be paired with the action of adding the Sequence to a project. In this situation, since the
 * latter will be the main undoable action, the parameter editing doesn't have to be undoable since to the user they'll
 * be viewed as one single action (see SetUndoable()).
 */
class SequenceDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief Used to set the dialog mode of operation (see SequenceDialog())
   */
  enum Type { kNew, kExisting };

  /**
   * @brief SequenceDialog Constructor
   *
   * @param s
   * Sequence to edit
   *
   * @param t
   * Mode of operation (changes some UI like the window title to best represent the action being performed)
   *
   * @param parent
   * QWidget parent
   */
  explicit SequenceDialog(Sequence* s, Type t = kExisting, QWidget* parent = nullptr);

  /**
   * @brief Set whether the parameter changes should be made into an undo command or not
   *
   * Defaults to true.
   */
  void SetUndoable(bool u);

  /**
   * @brief Set whether the name of this Sequence can be edited with this dialog
   *
   * Defaults to true.
   */
  void SetNameIsEditable(bool e);

 public slots:
  /**
   * @brief Function called when the user presses OK
   */
  void accept() override;

 private:
  Sequence* sequence_;

  SequenceDialogPresetTab* preset_tab_;

  SequenceDialogParameterTab* parameter_tab_;

  bool make_undoable_;

  QLineEdit* name_field_;

  /**
   * @brief An UndoCommand for setting the parameters on a sequence
   */
  class SequenceParamCommand : public UndoCommand {
   public:
    SequenceParamCommand(Sequence* s, VideoParams video_params, AudioParams audio_params, QString name, bool autocache);

    [[nodiscard]] Project* GetRelevantProject() const override;

   protected:
    void redo() override;
    void undo() override;

   private:
    Sequence* sequence_;

    VideoParams new_video_params_;
    AudioParams new_audio_params_;
    QString new_name_;
    bool new_autocache_;

    VideoParams old_video_params_;
    AudioParams old_audio_params_;
    QString old_name_;
    bool old_autocache_;
  };

 private slots:
  void SetAsDefaultClicked();
};

}  // namespace olive

#endif  // SEQUENCEDIALOG_H
