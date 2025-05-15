#ifndef TEXTGENERATORV1_H
#define TEXTGENERATORV1_H

#include "node/node.h"

namespace olive {

class TextGeneratorV1 : public Node {
  Q_OBJECT
 public:
  TextGeneratorV1();

  NODE_DEFAULT_FUNCTIONS(TextGeneratorV1)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;

  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  void GenerateFrame(FramePtr frame, const GenerateJob &job) const override;

  static const QString kTextInput;
  static const QString kHtmlInput;
  static const QString kColorInput;
  static const QString kVAlignInput;
  static const QString kFontInput;
  static const QString kFontSizeInput;
};

}  // namespace olive

#endif  // TEXTGENERATORV1_H
