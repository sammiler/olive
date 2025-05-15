

#include "nodeviewscene.h"

#include "core.h"
#include "node/project/sequence/sequence.h"
#include "nodeviewedge.h"
#include "nodeviewitem.h"

namespace olive {

NodeViewScene::NodeViewScene(QObject *parent)
    : QGraphicsScene(parent), direction_(NodeViewCommon::kLeftToRight), curved_edges_(true) {}

void NodeViewScene::SetFlowDirection(NodeViewCommon::FlowDirection direction) {
  direction_ = direction;

  foreach (NodeViewContext *ctx, context_map_) {
    ctx->SetFlowDirection(direction_);
  }
}

void NodeViewScene::SelectAll() {
  foreach (QGraphicsItem *i, items()) {
    i->setSelected(true);
  }
}

void NodeViewScene::DeselectAll() {
  foreach (QGraphicsItem *i, items()) {
    i->setSelected(false);
  }
}

QVector<NodeViewItem *> NodeViewScene::GetSelectedItems() const {
  QVector<NodeViewItem *> items;

  foreach (NodeViewContext *ctx, context_map_) {
    items.append(ctx->GetSelectedItems());
  }

  return items;
}

NodeViewContext *NodeViewScene::AddContext(Node *node) {
  NodeViewContext *context_item = context_map_.value(node);

  if (!context_item) {
    context_item = new NodeViewContext(node);

    context_item->SetFlowDirection(GetFlowDirection());
    context_item->SetCurvedEdges(GetEdgesAreCurved());

    QPointF pos(0, 0);
    QRectF item_rect = context_item->rect();
    while (!items(item_rect).isEmpty()) {
      pos.setY(pos.y() + item_rect.height());
      item_rect = context_item->rect().translated(pos);
    }
    context_item->setPos(pos);

    addItem(context_item);

    context_map_.insert(node, context_item);
  }

  return context_item;
}

void NodeViewScene::RemoveContext(Node *node) { delete context_map_.take(node); }

Qt::Orientation NodeViewScene::GetFlowOrientation() const { return NodeViewCommon::GetFlowOrientation(direction_); }

void NodeViewScene::SetEdgesAreCurved(bool curved) {
  if (curved_edges_ != curved) {
    curved_edges_ = curved;

    foreach (NodeViewContext *ctx, context_map_) {
      ctx->SetCurvedEdges(curved_edges_);
    }
  }
}

}  // namespace olive
