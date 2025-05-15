#ifndef OCIOGRADINGTRANSFORMLINEARNODE_H
#define OCIOGRADINGTRANSFORMLINEARNODE_H

#include "node/color/ociobase/ociobase.h"
#include "render/colorprocessor.h"

namespace olive {

class OCIOGradingTransformLinearNode : public OCIOBaseNode {
  Q_OBJECT
 public:
  OCIOGradingTransformLinearNode();

  NODE_DEFAULT_FUNCTIONS(OCIOGradingTransformLinearNode)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;
  void InputValueChangedEvent(const QString &input, int element) override;
  void GenerateProcessor();

  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  static const QString kContrastInput;
  static const QString kOffsetInput;
  static const QString kExposureInput;
  static const QString kSaturationInput;
  static const QString kPivotInput;
  static const QString kClampBlackEnableInput;
  static const QString kClampBlackInput;
  static const QString kClampWhiteEnableInput;
  static const QString kClampWhiteInput;

 protected slots:
  void ConfigChanged() override;

 private:
  void SetVec4InputColors(const QString &input);
};

}  // namespace olive

#endif
