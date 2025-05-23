#include "sequence.h"

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>
#include <utility>

#include "common/channellayout.h"
#include "common/qtutils.h"
#include "config/config.h"
#include "core.h"
#include "undo/undostack.h"

namespace olive {

SequenceDialog::SequenceDialog(Sequence* s, Type t, QWidget* parent)
    : QDialog(parent), sequence_(s), make_undoable_(true) {
  auto* layout = new QVBoxLayout(this);

  auto* splitter = new QSplitter();
  layout->addWidget(splitter);

  preset_tab_ = new SequenceDialogPresetTab();
  splitter->addWidget(preset_tab_);

  parameter_tab_ = new SequenceDialogParameterTab(sequence_);
  splitter->addWidget(parameter_tab_);

  connect(preset_tab_, &SequenceDialogPresetTab::PresetChanged, parameter_tab_,
          &SequenceDialogParameterTab::PresetChanged);
  connect(preset_tab_, &SequenceDialogPresetTab::PresetAccepted, this, &SequenceDialog::accept);
  connect(parameter_tab_, &SequenceDialogParameterTab::SaveParametersAsPreset, preset_tab_,
          &SequenceDialogPresetTab::SaveParametersAsPreset);

  // Set up name section
  auto* name_layout = new QHBoxLayout();
  name_layout->addWidget(new QLabel(tr("Name:")));
  name_field_ = new QLineEdit();
  name_layout->addWidget(name_field_);
  layout->addLayout(name_layout);

  // Set up dialog buttons
  auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  QPushButton* default_btn = buttons->addButton(tr("Set As Default"), QDialogButtonBox::ActionRole);
  connect(buttons, &QDialogButtonBox::accepted, this, &SequenceDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, this, &SequenceDialog::reject);
  connect(default_btn, &QPushButton::clicked, this, &SequenceDialog::SetAsDefaultClicked);
  layout->addWidget(buttons);

  // Set window title based on type
  switch (t) {
    case kNew:
      setWindowTitle(tr("New Sequence"));
      break;
    case kExisting:
      setWindowTitle(tr("Editing \"%1\"").arg(sequence_->GetLabel()));
      break;
  }

  name_field_->setText(sequence_->GetLabel());
}

void SequenceDialog::SetUndoable(bool u) { make_undoable_ = u; }

void SequenceDialog::SetNameIsEditable(bool e) { name_field_->setEnabled(e); }

void SequenceDialog::accept() {
  if (name_field_->isEnabled() && name_field_->text().isEmpty()) {
    QtUtils::MsgBox(this, QMessageBox::Critical, tr("Error editing Sequence"),
                    tr("Please enter a name for this Sequence."));
    return;
  }

  if (!VideoParams::FormatIsFloat(parameter_tab_->GetSelectedPreviewFormat()) &&
      !OLIVE_CONFIG("PreviewNonFloatDontAskAgain").toBool()) {
    QMessageBox b(this);
    auto* dont_show_again_ = new QCheckBox(tr("Don't ask me again"));

    b.setIcon(QMessageBox::Warning);
    b.setWindowTitle(tr("Low Quality Preview"));
    b.setText(
        tr("The preview resolution has been set to a non-float format. This may cause banding and clipping artifacts "
           "in the preview.\n\n"
           "Do you wish to continue?"));
    b.setCheckBox(dont_show_again_);

    b.addButton(QMessageBox::Yes);
    b.addButton(QMessageBox::No);

    if (b.exec() == QMessageBox::No) {
      return;
    }

    if (dont_show_again_->isChecked()) {
      OLIVE_CONFIG("PreviewNonFloatDontAskAgain") = true;
    }
  }

  // Generate video and audio parameter structs from data
  VideoParams video_params =
      VideoParams(parameter_tab_->GetSelectedVideoWidth(), parameter_tab_->GetSelectedVideoHeight(),
                  parameter_tab_->GetSelectedVideoFrameRate().flipped(), parameter_tab_->GetSelectedPreviewFormat(),
                  VideoParams::kInternalChannelCount, parameter_tab_->GetSelectedVideoPixelAspect(),
                  parameter_tab_->GetSelectedVideoInterlacingMode(), parameter_tab_->GetSelectedPreviewResolution());

  AudioParams audio_params =
      AudioParams(parameter_tab_->GetSelectedAudioSampleRate(), parameter_tab_->GetSelectedAudioChannelLayout(),
                  Sequence::kDefaultSampleFormat);

  if (make_undoable_) {
    // Make undoable command to change the parameters
    auto* param_command = new SequenceParamCommand(sequence_, video_params, audio_params, name_field_->text(),
                                                   olive::SequenceDialogParameterTab::GetSelectedPreviewAutoCache());

    Core::instance()->undo_stack()->push(param_command,
                                         tr("Set Sequence Parameters For \"%1\"").arg(sequence_->GetLabel()));

  } else {
    // Set sequence values directly with no undo command
    sequence_->SetVideoParams(video_params);
    sequence_->SetAudioParams(audio_params);
    sequence_->SetLabel(name_field_->text());
    olive::Sequence::SetVideoAutoCacheEnabled(olive::SequenceDialogParameterTab::GetSelectedPreviewAutoCache());
  }

  QDialog::accept();
}

void SequenceDialog::SetAsDefaultClicked() {
  if (QtUtils::MsgBox(this, QMessageBox::Question, tr("Confirm Set As Default"),
                      tr("Are you sure you want to set the current parameters as defaults?"),
                      QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
    // Maybe replace with Preset system
    OLIVE_CONFIG("DefaultSequenceWidth") = parameter_tab_->GetSelectedVideoWidth();
    OLIVE_CONFIG("DefaultSequenceHeight") = parameter_tab_->GetSelectedVideoHeight();
    OLIVE_CONFIG("DefaultSequencePixelAspect") = QVariant::fromValue(parameter_tab_->GetSelectedVideoPixelAspect());
    OLIVE_CONFIG("DefaultSequenceFrameRate") =
        QVariant::fromValue(parameter_tab_->GetSelectedVideoFrameRate().flipped());
    OLIVE_CONFIG("DefaultSequenceInterlacing") = parameter_tab_->GetSelectedVideoInterlacingMode();
    OLIVE_CONFIG("DefaultSequenceAudioFrequency") = parameter_tab_->GetSelectedAudioSampleRate();
    OLIVE_CONFIG("DefaultSequenceAudioLayout") = QVariant::fromValue(parameter_tab_->GetSelectedAudioChannelLayout());
  }
}

SequenceDialog::SequenceParamCommand::SequenceParamCommand(Sequence* s, VideoParams video_params,
                                                           AudioParams audio_params, QString name, bool autocache)
    : sequence_(s),
      new_video_params_(std::move(video_params)),
      new_audio_params_(std::move(audio_params)),
      new_name_(std::move(name)),
      new_autocache_(autocache),
      old_video_params_(s->GetVideoParams()),
      old_audio_params_(s->GetAudioParams()),
      old_name_(s->GetLabel()),
      old_autocache_(olive::Sequence::IsVideoAutoCacheEnabled()) {}

Project* SequenceDialog::SequenceParamCommand::GetRelevantProject() const { return sequence_->project(); }

void SequenceDialog::SequenceParamCommand::redo() {
  if (sequence_->GetVideoParams() != new_video_params_) {
    sequence_->SetVideoParams(new_video_params_);
  }
  if (sequence_->GetAudioParams() != new_audio_params_) {
    sequence_->SetAudioParams(new_audio_params_);
  }
  sequence_->SetLabel(new_name_);
  olive::Sequence::SetVideoAutoCacheEnabled(new_autocache_);
}

void SequenceDialog::SequenceParamCommand::undo() {
  if (sequence_->GetVideoParams() != old_video_params_) {
    sequence_->SetVideoParams(old_video_params_);
  }
  if (sequence_->GetAudioParams() != old_audio_params_) {
    sequence_->SetAudioParams(old_audio_params_);
  }
  sequence_->SetLabel(old_name_);
  olive::Sequence::SetVideoAutoCacheEnabled(old_autocache_);
}

}  // namespace olive
