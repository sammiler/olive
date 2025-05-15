#ifndef DROPSHADOWFILTER_H
#define DROPSHADOWFILTER_H

#include "node/node.h"

namespace olive {

class DropShadowFilter : public Node {
  Q_OBJECT
 public:
  DropShadowFilter();

  NODE_DEFAULT_FUNCTIONS(DropShadowFilter)

  [[nodiscard]] QString Name() const override { return tr("Drop Shadow"); }
  [[nodiscard]] QString id() const override { return QStringLiteral("org.olivevideoeditor.Olive.dropshadow"); }
  [[nodiscard]] QVector<CategoryID> Category() const override { return {kCategoryFilter}; }
  [[nodiscard]] QString Description() const override { return tr("Adds a drop shadow to an image."); }

  void Retranslate() override;

  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;
  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  static const QString kTextureInput;
  static const QString kColorInput;
  static const QString kDistanceInput;
  static const QString kAngleInput;
  static const QString kSoftnessInput;
  static const QString kOpacityInput;
  static const QString kFastInput;
};

}  // namespace olive

#endif  // DROPSHADOWFILTER_H
