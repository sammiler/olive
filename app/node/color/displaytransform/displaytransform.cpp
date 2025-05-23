#include "displaytransform.h"

#include "node/color/colormanager/colormanager.h"

namespace olive {

const QString DisplayTransformNode::kDisplayInput = QStringLiteral("display_in");
const QString DisplayTransformNode::kViewInput = QStringLiteral("view_in");
const QString DisplayTransformNode::kDirectionInput = QStringLiteral("dir_in");

#define super OCIOBaseNode

DisplayTransformNode::DisplayTransformNode() {
  AddInput(kDisplayInput, NodeValue::kCombo, 0, InputFlags(kInputFlagNotKeyframable | kInputFlagNotConnectable));

  AddInput(kViewInput, NodeValue::kCombo, 0, InputFlags(kInputFlagNotKeyframable | kInputFlagNotConnectable));

  AddInput(kDirectionInput, NodeValue::kCombo, 0, InputFlags(kInputFlagNotKeyframable | kInputFlagNotConnectable));
}

QString DisplayTransformNode::Name() const { return tr("Display Transform"); }

QString DisplayTransformNode::id() const { return QStringLiteral("org.olivevideoeditor.Olive.displaytransform"); }

QVector<Node::CategoryID> DisplayTransformNode::Category() const { return {kCategoryColor}; }

QString DisplayTransformNode::Description() const { return tr("Converts an image to or from a display color space."); }

void DisplayTransformNode::Retranslate() {
  super::Retranslate();

  SetInputName(kTextureInput, tr("Input"));
  SetInputName(kDisplayInput, tr("Display"));
  SetInputName(kViewInput, tr("View"));
  SetInputName(kDirectionInput, tr("Direction"));
  SetComboBoxStrings(kDirectionInput, {tr("Forward"), tr("Inverse")});
}

void DisplayTransformNode::InputValueChangedEvent(const QString &input, int element) {
  Q_UNUSED(element);
  if (input == kDisplayInput || input == kDirectionInput || input == kViewInput) {
    if (input == kDisplayInput) {
      UpdateViews();
    }
    GenerateProcessor();
  }
}

QString DisplayTransformNode::GetDisplay() const {
  if (manager()) {
    int index = GetStandardValue(kDisplayInput).toInt();
    if (index < manager()->ListAvailableDisplays().size()) {
      return manager()->ListAvailableDisplays().at(index);
    }
  }
  return {};
}

QString DisplayTransformNode::GetView() const {
  if (manager()) {
    QString display = GetDisplay();
    if (!display.isEmpty()) {
      int index = GetStandardValue(kViewInput).toInt();
      QStringList views = manager()->ListAvailableViews(display);
      if (index < views.size()) {
        return views.at(index);
      }
    }
  }
  return {};
}

ColorProcessor::Direction DisplayTransformNode::GetDirection() const {
  return static_cast<ColorProcessor::Direction>(GetStandardValue(kDirectionInput).toInt());
  ;
}

void DisplayTransformNode::UpdateDisplays() {
  if (manager()) {
    SetComboBoxStrings(kDisplayInput, manager()->ListAvailableDisplays());
  }
}

void DisplayTransformNode::UpdateViews() {
  if (manager()) {
    SetComboBoxStrings(kViewInput, manager()->ListAvailableViews(GetDisplay()));
  }
}

void DisplayTransformNode::ConfigChanged() {
  UpdateDisplays();
  UpdateViews();
  GenerateProcessor();
}

void DisplayTransformNode::GenerateProcessor() {
  if (manager()) {
    ColorTransform transform(GetDisplay(), GetView(), QString());
    set_processor(ColorProcessor::Create(manager(), manager()->GetReferenceColorSpace(), transform, GetDirection()));
  }
}

}  // namespace olive
