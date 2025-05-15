

#ifndef DISPLAYTRANSFORMNODE_H
#define DISPLAYTRANSFORMNODE_H

#include "node/color/ociobase/ociobase.h"
#include "render/colorprocessor.h"

namespace olive {

class DisplayTransformNode : public OCIOBaseNode {
  Q_OBJECT
 public:
  DisplayTransformNode();

  NODE_DEFAULT_FUNCTIONS(DisplayTransformNode)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  void Retranslate() override;
  void InputValueChangedEvent(const QString &input, int element) override;

  [[nodiscard]] QString GetDisplay() const;
  [[nodiscard]] QString GetView() const;
  [[nodiscard]] ColorProcessor::Direction GetDirection() const;

  static const QString kDisplayInput;
  static const QString kViewInput;
  static const QString kDirectionInput;

 protected slots:
  void ConfigChanged() override;

 private:
  void GenerateProcessor();

  void UpdateDisplays();

  void UpdateViews();
};

}  // namespace olive

#endif  // DISPLAYTRANSFORMNODE_H
