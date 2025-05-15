

#include "diptocolortransition.h"

namespace olive {

const QString DipToColorTransition::kColorInput = QStringLiteral("color_in");

#define super TransitionBlock

DipToColorTransition::DipToColorTransition() {
  AddInput(kColorInput, NodeValue::kColor, QVariant::fromValue(Color(0, 0, 0)));
}

QString DipToColorTransition::Name() const { return tr("Dip To Color"); }

QString DipToColorTransition::id() const { return QStringLiteral("org.olivevideoeditor.Olive.diptocolor"); }

QVector<Node::CategoryID> DipToColorTransition::Category() const { return {kCategoryTransition}; }

QString DipToColorTransition::Description() const { return tr("Transition between clips by dipping to a color."); }

ShaderCode DipToColorTransition::GetShaderCode(const ShaderRequest &request) const {
  Q_UNUSED(request)

  return ShaderCode(FileFunctions::ReadFileAsString(":/shaders/diptoblack.frag"), QString());
}

void DipToColorTransition::Retranslate() {
  super::Retranslate();

  SetInputName(kColorInput, tr("Color"));
}

void DipToColorTransition::ShaderJobEvent(const NodeValueRow &value, ShaderJob *job) const {
  job->Insert(kColorInput, value);
}

}  // namespace olive
