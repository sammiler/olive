#ifndef OCIOBASENODE_H
#define OCIOBASENODE_H

#include <utility>

#include "node/node.h"
#include "render/job/colortransformjob.h"

namespace olive {

class OCIOBaseNode : public Node {
  Q_OBJECT
 public:
  OCIOBaseNode();

  void AddedToGraphEvent(Project *p) override;
  void RemovedFromGraphEvent(Project *p) override;

  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  static const QString kTextureInput;

 protected slots:
  virtual void ConfigChanged() = 0;

 protected:
  [[nodiscard]] ColorManager *manager() const { return manager_; }

  [[nodiscard]] ColorProcessorPtr processor() const { return processor_; }
  void set_processor(ColorProcessorPtr p) { processor_ = std::move(p); }

 private:
  ColorManager *manager_;

  ColorProcessorPtr processor_;
};

}  // namespace olive

#endif  // OCIOBASENODE_H
