

#include "shapenode.h"

namespace olive {

#define super ShapeNodeBase

QString ShapeNode::kTypeInput = QStringLiteral("type_in");
QString ShapeNode::kRadiusInput = QStringLiteral("radius_in");

ShapeNode::ShapeNode() {
  PrependInput(kTypeInput, NodeValue::kCombo);

  AddInput(kRadiusInput, NodeValue::kFloat, 20.0);
  SetInputProperty(kRadiusInput, QStringLiteral("min"), 0.0);
}

QString ShapeNode::Name() const { return tr("Shape"); }

QString ShapeNode::id() const { return QStringLiteral("org.olivevideoeditor.Olive.shape"); }

QVector<Node::CategoryID> ShapeNode::Category() const { return {kCategoryGenerator}; }

QString ShapeNode::Description() const { return tr("Generate a 2D primitive shape."); }

void ShapeNode::Retranslate() {
  super::Retranslate();

  SetInputName(kTypeInput, tr("Type"));
  SetInputName(kRadiusInput, tr("Radius"));

  // Coordinate with Type enum
  SetComboBoxStrings(kTypeInput, {tr("Rectangle"), tr("Ellipse"), tr("Rounded Rectangle")});
}

ShaderCode ShapeNode::GetShaderCode(const ShaderRequest &request) const {
  if (request.id == QStringLiteral("shape")) {
    return ShaderCode(FileFunctions::ReadFileAsString(QStringLiteral(":/shaders/shape.frag")));
  } else {
    return super::GetShaderCode(request);
  }
}

void ShapeNode::Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const {
  TexturePtr base = value[kBaseInput].toTexture();

  ShaderJob job(value);

  job.Insert(QStringLiteral("resolution_in"),
             NodeValue(NodeValue::kVec2, base ? base->virtual_resolution() : globals.square_resolution(), this));
  job.SetShaderID(QStringLiteral("shape"));

  PushMergableJob(value, Texture::Job(base ? base->params() : globals.vparams(), job), table);
}

void ShapeNode::InputValueChangedEvent(const QString &input, int element) {
  if (input == kTypeInput) {
    SetInputFlag(kRadiusInput, kInputFlagHidden, (GetStandardValue(kTypeInput).toInt() != kRoundedRectangle));
  }
  super::InputValueChangedEvent(input, element);
}

}  // namespace olive
