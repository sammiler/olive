

#ifndef SERIALIZEDDATA_H
#define SERIALIZEDDATA_H

#include <QHash>
#include <QVariant>

#include "node.h"

namespace olive {

class NodeGroup;

struct SerializedData {
  struct SerializedConnection {
    NodeInput input;
    quintptr output_node{};
  };

  struct BlockLink {
    Node *block;
    quintptr link;
  };

  struct GroupLink {
    NodeGroup *group{};
    QString passthrough_id;
    quintptr input_node{};
    QString input_id;
    int input_element{};
    QString custom_name;
    InputFlags custom_flags;
    NodeValue::Type data_type;
    QVariant default_val;
    QHash<QString, QVariant> custom_properties;
  };

  QMap<Node *, QMap<quintptr, Node::Position> > positions;
  QHash<quintptr, Node *> node_ptrs;
  QList<SerializedConnection> desired_connections;
  QList<BlockLink> block_links;
  QVector<GroupLink> group_input_links;
  QHash<NodeGroup *, quintptr> group_output_links;
};

}  // namespace olive

#endif  // SERIALIZEDDATA_H
