

#ifndef CHROMAKEYNODE_H
#define CHROMAKEYNODE_H

#include "node/color/ociobase/ociobase.h"

namespace olive {

class ChromaKeyNode : public OCIOBaseNode {
  Q_OBJECT
 public:
  ChromaKeyNode();

  NODE_DEFAULT_FUNCTIONS(ChromaKeyNode)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  void InputValueChangedEvent(const QString& input, int element) override;

  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest& request) const override;
  void Value(const NodeValueRow& value, const NodeGlobals& globals, NodeValueTable* table) const override;

  void ConfigChanged() override;

  static const QString kColorInput;
  static const QString kInvertInput;
  static const QString kMaskOnlyInput;
  static const QString kUpperToleranceInput;
  static const QString kLowerToleranceInput;
  static const QString kGarbageMatteInput;
  static const QString kCoreMatteInput;
  static const QString kShadowsInput;
  static const QString kHighlightsInput;

 private:
  void GenerateProcessor();
};

}  // namespace olive

#endif  // CHROMAKEYNODE_H
