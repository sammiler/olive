#ifndef NODEVIEWITEMCONNECTOR_H
#define NODEVIEWITEMCONNECTOR_H

#include <QGraphicsPolygonItem>

#include "nodeviewcommon.h"

namespace olive {

class NodeViewItemConnector : public QGraphicsPolygonItem {
 public:
  explicit NodeViewItemConnector(bool is_output, QGraphicsItem *parent = nullptr);

  void SetFlowDirection(NodeViewCommon::FlowDirection dir);

  [[nodiscard]] bool IsOutput() const { return output_; }

  [[nodiscard]] QPainterPath shape() const override;
  [[nodiscard]] QRectF boundingRect() const override;

 private:
  bool output_;
};

}  // namespace olive

#endif  // NODEVIEWITEMCONNECTOR_H
