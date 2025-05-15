

#ifndef SERIALIZER211228_H
#define SERIALIZER211228_H

#include "serializer.h"

namespace olive {

class ProjectSerializer211228 : public ProjectSerializer {
 public:
  ProjectSerializer211228() = default;

 protected:
  LoadData Load(Project *project, QXmlStreamReader *reader, LoadType load_type, void *reserved) const override;

  [[nodiscard]] uint Version() const override { return 211228; }

 private:
  struct XMLNodeData {
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
      quintptr input_node{};
      QString input_id;
      int input_element{};
    };

    QHash<quintptr, Node *> node_ptrs;
    QList<SerializedConnection> desired_connections;
    QList<BlockLink> block_links;
    QVector<GroupLink> group_input_links;
    QHash<NodeGroup *, quintptr> group_output_links;
    QHash<Node *, QUuid> node_uuids;
  };

  void LoadNode(Node *node, XMLNodeData &xml_node_data, QXmlStreamReader *reader) const;

  static void LoadColorManager(QXmlStreamReader *reader, Project *project);

  static void LoadProjectSettings(QXmlStreamReader *reader, Project *project);

  void LoadInput(Node *node, QXmlStreamReader *reader, XMLNodeData &xml_node_data) const;

  static void LoadImmediate(QXmlStreamReader *reader, Node *node, const QString &input, int element,
                            XMLNodeData &xml_node_data);

  static bool LoadPosition(QXmlStreamReader *reader, quintptr *node_ptr, Node::Position *pos);

  static void PostConnect(const XMLNodeData &xml_node_data);

  void LoadNodeCustom(QXmlStreamReader *reader, Node *node, XMLNodeData &xml_node_data) const;

  static void LoadTimelinePoints(QXmlStreamReader *reader, ViewerOutput *points);

  static void LoadWorkArea(QXmlStreamReader *reader, TimelineWorkArea *workarea);

  static void LoadMarkerList(QXmlStreamReader *reader, TimelineMarkerList *markers);

  static void LoadValueHint(Node::ValueHint *hint, QXmlStreamReader *reader);
};

}  // namespace olive

#endif  // SERIALIZER211228_H
