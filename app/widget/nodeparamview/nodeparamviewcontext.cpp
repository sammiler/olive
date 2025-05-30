#include "nodeparamviewcontext.h"

#include <QMessageBox>

#include "node/block/clip/clip.h"
#include "node/factory.h"
#include "node/nodeundo.h"

namespace olive {

#define super NodeParamViewItemBase

NodeParamViewContext::NodeParamViewContext(QWidget *parent) : super(parent), type_(Track::kNone) {
  auto *body = new QWidget();
  auto *body_layout = new QHBoxLayout(body);
  SetBody(body);

  dock_area_ = new NodeParamViewDockArea();
  body_layout->addWidget(dock_area_);

  setBackgroundRole(QPalette::Base);

  Retranslate();

  connect(title_bar(), &NodeParamViewItemTitleBar::AddEffectButtonClicked, this,
          &NodeParamViewContext::AddEffectButtonClicked);
}

NodeParamViewItem *NodeParamViewContext::GetItem(Node *node, Node *ctx) {
  for (auto item : items_) {
    if (item->GetNode() == node && item->GetContext() == ctx) {
      return item;
    }
  }

  return nullptr;
}

void NodeParamViewContext::AddNode(NodeParamViewItem *item) {
  items_.append(item);
  dock_area_->AddItem(item);
}

void NodeParamViewContext::RemoveNode(Node *node, Node *ctx) {
  for (auto it = items_.begin(); it != items_.end();) {
    NodeParamViewItem *item = *it;

    if (item->GetNode() == node && item->GetContext() == ctx) {
      emit AboutToDeleteItem(item);
      delete item;
      it = items_.erase(it);
    } else {
      it++;
    }
  }
}

void NodeParamViewContext::RemoveNodesWithContext(Node *ctx) {
  for (auto it = items_.begin(); it != items_.end();) {
    NodeParamViewItem *item = *it;

    if (item->GetContext() == ctx) {
      emit AboutToDeleteItem(item);
      delete item;
      it = items_.erase(it);
    } else {
      it++;
    }
  }
}

void NodeParamViewContext::SetInputChecked(const NodeInput &input, bool e) {
  foreach (NodeParamViewItem *item, items_) {
    if (item->GetNode() == input.node()) {
      item->SetInputChecked(input, e);
    }
  }
}

void NodeParamViewContext::SetTimebase(const rational &timebase) {
  foreach (NodeParamViewItem *item, items_) {
    item->SetTimebase(timebase);
  }
}

void NodeParamViewContext::SetTimeTarget(ViewerOutput *n) {
  foreach (NodeParamViewItem *item, items_) {
    item->SetTimeTarget(n);
  }
}

void NodeParamViewContext::SetEffectType(Track::Type type) { type_ = type; }

void NodeParamViewContext::Retranslate() {}

void NodeParamViewContext::AddEffectButtonClicked() {
  Node::Flag flag = Node::kNone;

  if (type_ == Track::kVideo) {
    flag = Node::kVideoEffect;
  } else {
    flag = Node::kAudioEffect;
  }

  if (flag == Node::kNone) {
    return;
  }

  Menu *m = NodeFactory::CreateMenu(this, false, Node::kCategoryUnknown, flag);
  connect(m, &Menu::triggered, this, &NodeParamViewContext::AddEffectMenuItemTriggered);
  m->exec(QCursor::pos());
  delete m;
}

void NodeParamViewContext::AddEffectMenuItemTriggered(QAction *a) {
  Node *n = NodeFactory::CreateFromMenuAction(a);

  if (n) {
    NodeInput new_node_input = n->GetEffectInput();
    auto *command = new MultiUndoCommand();

    QVector<Project *> graphs_added_to;

    foreach (Node *ctx, contexts_) {
      NodeInput ctx_input = ctx->GetEffectInput();

      if (!graphs_added_to.contains(ctx->parent())) {
        command->add_child(new NodeAddCommand(ctx->parent(), n));
        graphs_added_to.append(ctx->parent());
      }

      command->add_child(new NodeSetPositionCommand(n, ctx, Node::Position(ctx->GetNodePositionInContext(ctx))));
      command->add_child(
          new NodeSetPositionCommand(ctx, ctx, Node::Position(ctx->GetNodePositionInContext(ctx) + QPointF(1, 0))));

      if (ctx_input.IsConnected()) {
        Node *prev_output = ctx_input.GetConnectedOutput();

        command->add_child(new NodeEdgeRemoveCommand(prev_output, ctx_input));
        command->add_child(new NodeEdgeAddCommand(prev_output, new_node_input));
      }

      command->add_child(new NodeEdgeAddCommand(n, ctx_input));
    }

    Core::instance()->undo_stack()->push(command, tr("Added %1 to Node Chain").arg(n->Name()));
  }
}

}  // namespace olive
