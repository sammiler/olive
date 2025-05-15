#ifndef CROSSDISSOLVETRANSITION_H
#define CROSSDISSOLVETRANSITION_H

#include "node/block/transition/transition.h"

namespace olive {

class CrossDissolveTransition : public TransitionBlock {
  Q_OBJECT
 public:
  CrossDissolveTransition();

  NODE_DEFAULT_FUNCTIONS(CrossDissolveTransition)

  [[nodiscard]] QString Name() const override;
  [[nodiscard]] QString id() const override;
  [[nodiscard]] QVector<CategoryID> Category() const override;
  [[nodiscard]] QString Description() const override;

  // virtual void Retranslate() override;

  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

 protected:
  void SampleJobEvent(const SampleBuffer &from_samples, const SampleBuffer &to_samples, SampleBuffer &out_samples,
                      double time_in) const override;
};

}  // namespace olive

#endif  // CROSSDISSOLVETRANSITION_H
