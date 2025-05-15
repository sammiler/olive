

#include "subtitle.h"

namespace olive {

#define super ClipBlock

const QString SubtitleBlock::kTextIn = QStringLiteral("text_in");

SubtitleBlock::SubtitleBlock() {
  AddInput(kTextIn, NodeValue::kText, InputFlags(kInputFlagNotConnectable | kInputFlagNotKeyframable));

  SetInputFlag(kBufferIn, kInputFlagHidden);
  SetInputFlag(kLengthInput, kInputFlagHidden);
  SetInputFlag(kMediaInInput, kInputFlagHidden);
  SetInputFlag(kSpeedInput, kInputFlagHidden);
  SetInputFlag(kReverseInput, kInputFlagHidden);
  SetInputFlag(kMaintainAudioPitchInput, kInputFlagHidden);

  // Undo block flag that hides in param view
  SetFlag(kDontShowInParamView, false);
}

QString SubtitleBlock::Name() const {
  if (GetText().isEmpty()) {
    return tr("Subtitle");
  } else {
    return GetText();
  }
}

QString SubtitleBlock::id() const { return QStringLiteral("org.olivevideoeditor.Olive.subtitle"); }

QString SubtitleBlock::Description() const {
  return tr("A time-based node representing a single subtitle element for a certain period of time.");
}

void SubtitleBlock::Retranslate() {
  super::Retranslate();

  SetInputName(kTextIn, tr("Text"));
}

}  // namespace olive
