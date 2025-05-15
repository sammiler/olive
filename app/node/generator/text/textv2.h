#ifndef TEXTGENERATORV2_H
#define TEXTGENERATORV2_H

#include "node/generator/shape/shapenodebase.h"

namespace olive {

class TextGeneratorV2 : public ShapeNodeBase {
  Q_OBJECT
 public:
  TextGeneratorV2();

  NODE_DEFAULT_FUNCTIONS(TextGeneratorV2)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  void GenerateFrame(FramePtr frame, const GenerateJob &job) const override;

  static const QString kTextInput;
  static const QString kHtmlInput;
  static const QString kVAlignInput;
  static const QString kFontInput;
  static const QString kFontSizeInput;
};

}  // namespace olive

#endif  // TEXTGENERATORV2_H
