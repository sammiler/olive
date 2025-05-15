

#ifndef COLORDIFFERENCEKEYNODE_H
#define COLORDIFFERENCEKEYNODE_H

#include "node/node.h"

namespace olive {

class ColorDifferenceKeyNode : public Node {
 public:
  ColorDifferenceKeyNode();

  NODE_DEFAULT_FUNCTIONS(ColorDifferenceKeyNode)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest& request) const override;
  void Value(const NodeValueRow& value, const NodeGlobals& globals, NodeValueTable* table) const override;

  static const QString kTextureInput;
  static const QString kGarbageMatteInput;
  static const QString kCoreMatteInput;
  static const QString kColorInput;
  static const QString kShadowsInput;
  static const QString kHighlightsInput;
  static const QString kMaskOnlyInput;
};

}  // namespace olive

#endif  // COLORDIFFERENCEKEYNODE_H
