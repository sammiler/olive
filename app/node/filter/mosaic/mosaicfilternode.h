

#ifndef MOSAICFILTERNODE_H
#define MOSAICFILTERNODE_H

#include "node/node.h"

namespace olive {

class MosaicFilterNode : public Node {
  Q_OBJECT
 public:
  MosaicFilterNode();

  NODE_DEFAULT_FUNCTIONS(MosaicFilterNode)

  [[nodiscard]] QString Name() const override { return tr("Mosaic"); }

  [[nodiscard]] QString id() const override { return QStringLiteral("org.olivevideoeditor.Olive.mosaicfilter"); }

  [[nodiscard]] QVector<CategoryID> Category() const override { return {kCategoryFilter}; }

  [[nodiscard]] QString Description() const override { return tr("Apply a pixelated mosaic filter to video."); }

  void Retranslate() override;

  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;
  [[nodiscard]] ShaderCode GetShaderCode(const ShaderRequest &request) const override;

  static const QString kTextureInput;
  static const QString kHorizInput;
  static const QString kVertInput;
};

}  // namespace olive

#endif  // MOSAICFILTERNODE_H
