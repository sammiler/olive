#include "folder.h"

#include "common/xmlutils.h"
#include "node/nodeundo.h"
#include "node/project/footage/footage.h"
#include "node/project/sequence/sequence.h"
#include "ui/icons/icons.h"

namespace olive {

#define super Node

const QString Folder::kChildInput = QStringLiteral("child_in");

Folder::Folder() {
  SetFlag(kIsItem);

  AddInput(kChildInput, NodeValue::kNone, InputFlags(kInputFlagArray | kInputFlagNotKeyframable));
}

QVariant Folder::data(const DataType &d) const {
  if (d == ICON) {
    return icon::Folder;
  }

  return super::data(d);
}

void Folder::Retranslate() {
  super::Retranslate();

  SetInputName(kChildInput, tr("Children"));
}

Node *GetChildWithNameInternal(const Folder *n, const QString &s) {
  for (int i = 0; i < n->item_child_count(); i++) {
    Node *child = n->item_child(i);

    if (child->GetLabel() == s) {
      return child;
    } else if (auto *subfolder = dynamic_cast<Folder *>(child)) {
      if (Node *n2 = GetChildWithNameInternal(subfolder, s)) {
        return n2;
      }
    }
  }

  return nullptr;
}

Node *Folder::GetChildWithName(const QString &s) const { return GetChildWithNameInternal(this, s); }

bool Folder::HasChildRecursive(Node *child) const {
  for (Node *i : item_children_) {
    if (i == child) {
      return true;
    } else if (auto *f = dynamic_cast<Folder *>(i)) {
      if (f->HasChildRecursive(child)) {
        return true;
      }
    }
  }

  return false;
}

int Folder::index_of_child_in_array(Node *item) const {
  int index_of_item = item_children_.indexOf(item);

  if (index_of_item == -1) {
    return -1;
  }

  return item_element_index_.at(index_of_item);
}

void Folder::InputConnectedEvent(const QString &input, int element, Node *output) {
  if (input == kChildInput && element != -1) {
    Node *item = output;

    // The insert index is always our "count" because we only support appending in our internal
    // model. For sorting/organizing, a QSortFilterProxyModel is used instead.
    emit BeginInsertItem(item, item_child_count());
    item_children_.append(item);
    item_element_index_.append(element);
    item->SetFolder(this);
    emit EndInsertItem();
  }
}

void Folder::InputDisconnectedEvent(const QString &input, int element, Node *output) {
  if (input == kChildInput && element != -1) {
    Node *item = output;

    int child_index = item_children_.indexOf(item);
    emit BeginRemoveItem(item, child_index);
    item_children_.removeAt(child_index);
    item_element_index_.removeAt(child_index);
    item->SetFolder(nullptr);
    emit EndRemoveItem();
  }
}

FolderAddChild::FolderAddChild(Folder *folder, Node *child) : folder_(folder), child_(child) {}

Project *FolderAddChild::GetRelevantProject() const { return folder_->project(); }

void FolderAddChild::redo() {
  int array_index = folder_->InputArraySize(Folder::kChildInput);
  folder_->InputArrayAppend(Folder::kChildInput);
  Node::ConnectEdge(child_, NodeInput(folder_, Folder::kChildInput, array_index));
}

void FolderAddChild::undo() {
  Node::DisconnectEdge(child_,
                       NodeInput(folder_, Folder::kChildInput, folder_->InputArraySize(Folder::kChildInput) - 1));
  folder_->InputArrayRemoveLast(Folder::kChildInput);
}

void Folder::RemoveElementCommand::redo() {
  if (!subcommand_) {
    remove_index_ = folder_->index_of_child_in_array(child_);
    if (remove_index_ != -1) {
      NodeInput connected_input(folder_, Folder::kChildInput, remove_index_);
      subcommand_ = new MultiUndoCommand();
      subcommand_->add_child(new NodeEdgeRemoveCommand(folder_->GetConnectedOutput(connected_input), connected_input));
      subcommand_->add_child(new NodeArrayRemoveCommand(folder_, Folder::kChildInput, remove_index_));
    }
  }

  if (subcommand_) {
    subcommand_->redo_now();
  }
}

}  // namespace olive
