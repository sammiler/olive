

#ifndef MASKDISTORTNODE_H
#define MASKDISTORTNODE_H

#include "node/generator/polygon/polygon.h"

namespace olive {

class MaskDistortNode : public PolygonGenerator {
  Q_OBJECT
 public:
  MaskDistortNode();

  NODE_DEFAULT_FUNCTIONS(MaskDistortNode)

  [[nodiscard]] QString Name() const override { return tr("Mask"); }

  [[nodiscard]] QString id() const override { return QStringLiteral("org.olivevideoeditor.Olive.mask"); }

  [[nodiscard]] QVector<CategoryID> Category() const override { return {kCategoryDistort}; }

  [[nodiscard]] QString Description() const override { return tr("Apply a polygonal mask."); }

  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

  void Retranslate() override;

  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  static const QString kInvertInput;
  static const QString kFeatherInput;
};

}  // namespace olive

#endif  // MASKDISTORTNODE_H
