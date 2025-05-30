#include "footageproperties.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QSpinBox>
#include <QTreeWidgetItem>

#include "core.h"
#include "node/nodeundo.h"
#include "streamproperties/audiostreamproperties.h"
#include "streamproperties/videostreamproperties.h"

namespace olive {

FootagePropertiesDialog::FootagePropertiesDialog(QWidget *parent, Footage *footage)
    : QDialog(parent), footage_(footage) {
  auto *layout = new QGridLayout(this);

  setWindowTitle(tr("\"%1\" Properties").arg(footage_->GetLabelOrName()));
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  int row = 0;

  layout->addWidget(new QLabel(tr("Name:")), row, 0);

  footage_name_field_ = new QLineEdit(footage_->GetLabel());
  layout->addWidget(footage_name_field_, row, 1);
  row++;

  layout->addWidget(new QLabel(tr("Tracks:")), row, 0, 1, 2);
  row++;

  track_list = new QListWidget();
  layout->addWidget(track_list, row, 0, 1, 2);

  row++;

  stacked_widget_ = new QStackedWidget();
  layout->addWidget(stacked_widget_, row, 0, 1, 2);

  int first_usable_stream = -1;

  for (int i = 0; i < footage_->GetTotalStreamCount(); i++) {
    Track::Reference reference = footage_->GetReferenceFromRealIndex(i);

    QString description;
    bool is_enabled = false;

    switch (reference.type()) {
      case Track::kVideo: {
        stacked_widget_->addWidget(new VideoStreamProperties(footage_, reference.index()));

        VideoParams vp = footage_->GetVideoParams(reference.index());
        is_enabled = vp.enabled();
        description = Footage::DescribeVideoStream(vp);
        break;
      }
      case Track::kAudio: {
        stacked_widget_->addWidget(new AudioStreamProperties(footage_, reference.index()));

        AudioParams ap = footage_->GetAudioParams(reference.index());
        is_enabled = ap.enabled();
        description = Footage::DescribeAudioStream(ap);
        break;
      }
      case Track::kSubtitle: {
        SubtitleParams sp = footage_->GetSubtitleParams(reference.index());
        is_enabled = sp.enabled();

        // FIXME: Language?
        description = tr("Subtitles");
        break;
      }
      default:
        stacked_widget_->addWidget(new StreamProperties());
        description = tr("Unknown");
        break;
    }

    auto *item = new QListWidgetItem(description, track_list);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(is_enabled ? Qt::Checked : Qt::Unchecked);
    track_list->addItem(item);

    if (first_usable_stream == -1 && (reference.type() == Track::kVideo || reference.type() == Track::kAudio ||
                                      reference.type() == Track::kSubtitle)) {
      first_usable_stream = i;
    }
  }

  row++;

  auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  buttons->setCenterButtons(true);
  layout->addWidget(buttons, row, 0, 1, 2);

  connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

  connect(track_list, &QListWidget::currentRowChanged, stacked_widget_, &QStackedWidget::setCurrentIndex);

  // Auto-select first item that actually has properties
  if (first_usable_stream >= 0) {
    track_list->setCurrentRow(first_usable_stream);
  }
  track_list->setFocus();
}

void FootagePropertiesDialog::accept() {
  // Perform sanity check on all pages
  for (int i = 0; i < stacked_widget_->count(); i++) {
    if (!dynamic_cast<StreamProperties *>(stacked_widget_->widget(i))->SanityCheck()) {
      // Switch to the failed panel in question
      stacked_widget_->setCurrentIndex(i);

      // Do nothing (it's up to the property panel itself to throw the error message)
      return;
    }
  }

  auto *command = new MultiUndoCommand();

  if (footage_->GetLabel() != footage_name_field_->text()) {
    auto *nrc = new NodeRenameCommand();
    nrc->AddNode(footage_, footage_name_field_->text());
    command->add_child(nrc);
  }

  for (int i = 0; i < footage_->GetTotalStreamCount(); i++) {
    Track::Reference reference = footage_->GetReferenceFromRealIndex(i);
    bool new_stream_enabled = (track_list->item(i)->checkState() == Qt::Checked);
    bool old_stream_enabled = new_stream_enabled;

    switch (reference.type()) {
      case Track::kVideo:
        old_stream_enabled = footage_->GetVideoParams(reference.index()).enabled();
        break;
      case Track::kAudio:
        old_stream_enabled = footage_->GetAudioParams(reference.index()).enabled();
        break;
      case Track::kSubtitle:
        old_stream_enabled = footage_->GetSubtitleParams(reference.index()).enabled();
        break;
      case Track::kNone:
      case Track::kCount:
        break;
    }

    if (old_stream_enabled != new_stream_enabled) {
      command->add_child(
          new StreamEnableChangeCommand(footage_, reference.type(), reference.index(), new_stream_enabled));
    }
  }

  for (int i = 0; i < stacked_widget_->count(); i++) {
    dynamic_cast<StreamProperties *>(stacked_widget_->widget(i))->Accept(command);
  }

  Core::instance()->undo_stack()->push(command, tr("Set Footage \"%1\" Properties").arg(footage_->GetLabel()));

  QDialog::accept();
}

FootagePropertiesDialog::StreamEnableChangeCommand::StreamEnableChangeCommand(Footage *footage, Track::Type type,
                                                                              int index_in_type, bool enabled)
    : footage_(footage), type_(type), index_(index_in_type), new_enabled_(enabled) {}

Project *FootagePropertiesDialog::StreamEnableChangeCommand::GetRelevantProject() const { return footage_->project(); }

void FootagePropertiesDialog::StreamEnableChangeCommand::redo() {
  switch (type_) {
    case Track::kVideo: {
      VideoParams vp = footage_->GetVideoParams(index_);
      old_enabled_ = vp.enabled();
      vp.set_enabled(new_enabled_);
      footage_->SetVideoParams(vp, index_);
      break;
    }
    case Track::kAudio: {
      AudioParams ap = footage_->GetAudioParams(index_);
      old_enabled_ = ap.enabled();
      ap.set_enabled(new_enabled_);
      footage_->SetAudioParams(ap, index_);
      break;
    }
    case Track::kSubtitle: {
      SubtitleParams sp = footage_->GetSubtitleParams(index_);
      old_enabled_ = sp.enabled();
      sp.set_enabled(new_enabled_);
      footage_->SetSubtitleParams(sp, index_);
      break;
    }
    case Track::kNone:
    case Track::kCount:
      break;
  }
}

void FootagePropertiesDialog::StreamEnableChangeCommand::undo() {
  switch (type_) {
    case Track::kVideo: {
      VideoParams vp = footage_->GetVideoParams(index_);
      vp.set_enabled(old_enabled_);
      footage_->SetVideoParams(vp, index_);
      break;
    }
    case Track::kAudio: {
      AudioParams ap = footage_->GetAudioParams(index_);
      ap.set_enabled(old_enabled_);
      footage_->SetAudioParams(ap, index_);
      break;
    }
    case Track::kSubtitle: {
      SubtitleParams sp = footage_->GetSubtitleParams(index_);
      sp.set_enabled(old_enabled_);
      footage_->SetSubtitleParams(sp, index_);
      break;
    }
    case Track::kNone:
    case Track::kCount:
      break;
  }
}

}  // namespace olive
