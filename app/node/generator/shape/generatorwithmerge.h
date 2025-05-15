

#ifndef GENERATORWITHMERGE_H
#define GENERATORWITHMERGE_H

#include "node/node.h"

namespace olive {

class GeneratorWithMerge : public Node {
  Q_OBJECT
 public:
  GeneratorWithMerge();

  void Retranslate() override;

  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

  static const QString kBaseInput;

 protected:
  void PushMergableJob(const NodeValueRow &value, const TexturePtr &job, NodeValueTable *table) const;
};

}  // namespace olive

#endif  // GENERATORWITHMERGE_H
