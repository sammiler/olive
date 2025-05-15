#ifndef DIPTOCOLORTRANSITION_H
#define DIPTOCOLORTRANSITION_H

#include "node/block/transition/transition.h"

namespace olive {

class DipToColorTransition : public TransitionBlock {
  Q_OBJECT
 public:
  DipToColorTransition();

  NODE_DEFAULT_FUNCTIONS(DipToColorTransition)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

  void Retranslate() override;

  static const QString kColorInput;

 protected:
  void ShaderJobEvent(const NodeValueRow &value, ShaderJob *job) const override;
};

}  // namespace olive

#endif  // DIPTOCOLORTRANSITION_H
