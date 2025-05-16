#ifndef SERIALIZER220403_H  // 防止头文件被多次包含的宏定义开始
#define SERIALIZER220403_H

#include "serializer.h"  // 引入基类 ProjectSerializer 的定义 (文件名可能是 "projectserializer.h")

// 可能需要的前向声明 (与 ProjectSerializer211228.h 类似，并可能新增)
// class Project;
// class QXmlStreamReader;
// enum class LoadType;
// class LoadData;
// class Node;
// struct NodeInput;
// class NodeGroup;
// struct TimelineMarker;
// class ViewerOutput;
// class TimelineWorkArea;
// class TimelineMarkerList;
// namespace Node { struct Position; struct ValueHint; }
// class QUuid;
// enum class InputFlags; // 新增
// namespace NodeValue { enum class Type; } // 新增
// class QVariant; // 新增
// class NodeKeyframe; // 新增

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 特定版本的项目序列化器，版本号为 220403。
 * 这个类继承自 ProjectSerializer，并实现了针对版本号为 220403 的项目文件的
 * 具体加载逻辑。与 211228 版本相比，此版本在 `XMLNodeData::GroupLink` 结构体中
 * 增加了更多关于节点组输入直通的自定义属性，并且引入了加载关键帧的特定方法。
 */
class ProjectSerializer220403 : public ProjectSerializer {
 public:
  /**
   * @brief ProjectSerializer220403 默认构造函数。
   */
  ProjectSerializer220403() = default;

 protected:
  /**
   * @brief 实现从 XML 流加载版本号为 220403 的项目数据的具体逻辑。
   *  重写自 ProjectSerializer 基类。
   * @param project 目标项目对象，加载的数据将被填充到此项目中。
   * @param reader 指向 QXmlStreamReader 的指针，用于读取 XML 数据。
   * @param load_type 加载类型/范围 (例如，加载整个项目、仅节点等)。
   * @param reserved 保留参数，未来可能使用，当前未使用。
   * @return LoadData 包含从 XML 中解析出的项目数据的 LoadData 对象。
   */
  LoadData Load(Project *project, QXmlStreamReader *reader, LoadType load_type, void *reserved) const override;

  /**
   * @brief 返回此序列化器支持的项目文件版本号。
   * @return uint 版本号，固定为 220403。
   */
  [[nodiscard]] uint Version() const override { return 220403; }  // 此序列化器处理版本 220403

 private:
  /**
   * @brief 在加载过程中临时存储从 XML 解析出的节点相关数据的内部结构体。
   *  与 211228 版本相比，`GroupLink` 结构体增加了更多字段。
   */
  struct XMLNodeData {
    /** @brief 存储一个待建立的节点连接的信息。 */
    struct SerializedConnection {
      NodeInput input;         ///< 目标节点的输入参数。
      quintptr output_node{};  ///< 源节点的指针占位符。
    };

    /** @brief 存储一个媒体块（Block）与其链接对象之间的链接信息。 */
    struct BlockLink {
      Node *block;    ///< 指向媒体块的指针。
      quintptr link;  ///< 指向链接对象的指针占位符。
    };

    /** @brief 存储节点组（NodeGroup）的输入与其内部节点输入之间的链接信息，包含更多自定义属性。 */
    struct GroupLink {
      NodeGroup *group{};                          ///< 指向节点组的指针。
      QString passthrough_id;                      ///< 节点组输入直通的外部 ID。
      quintptr input_node{};                       ///< 节点组内部目标节点的指针占位符。
      QString input_id;                            ///< 节点组内部目标节点输入参数的ID。
      int input_element{};                         ///< 节点组内部目标节点输入参数的元素索引。
      QString custom_name;                         ///< 节点组输入直通的自定义名称。
      InputFlags custom_flags;                     ///< 节点组输入直通的自定义标志。
      NodeValue::Type data_type;                   ///< 节点组输入直通的数据类型。
      QVariant default_val;                        ///< 节点组输入直通的默认值。
      QHash<QString, QVariant> custom_properties;  ///< 节点组输入直通的其他自定义属性。
    };

    QHash<quintptr, Node *> node_ptrs;                ///< 节点指针占位符到实际 Node 指针的映射。
    QList<SerializedConnection> desired_connections;  ///< 待建立的节点连接列表。
    QList<BlockLink> block_links;                     ///< 待建立的媒体块链接列表。
    QVector<GroupLink> group_input_links;             ///< 待建立的节点组输入链接列表。
    QHash<NodeGroup *, quintptr> group_output_links;  ///< 节点组输出链接信息。
    QHash<Node *, QUuid> node_uuids;                  ///< 存储已加载节点的 Node 指针到其 UUID 的映射。
  };

  /**
   * @brief (私有成员函数) 从 XML 读取器加载单个节点的数据。
   * @param node 指向要填充数据的 Node 对象的指针。
   * @param xml_node_data 用于存储和解析节点间链接信息的 XMLNodeData 对象。
   * @param reader 指向 QXmlStreamReader 的指针。
   */
  void LoadNode(Node *node, XMLNodeData &xml_node_data, QXmlStreamReader *reader) const;

  /**
   * @brief (私有静态成员函数) 从 XML 读取器加载色彩管理器 (ColorManager) 的设置。
   * @param reader 指向 QXmlStreamReader 的指针。
   * @param project 指向目标 Project 对象的指针。
   */
  static void LoadColorManager(QXmlStreamReader *reader, Project *project);

  /**
   * @brief (私有静态成员函数) 从 XML 读取器加载项目级别的设置。
   * @param reader 指向 QXmlStreamReader 的指针。
   * @param project 指向目标 Project 对象的指针。
   */
  static void LoadProjectSettings(QXmlStreamReader *reader, Project *project);

  /**
   * @brief (私有成员函数) 从 XML 读取器加载节点的单个输入参数。
   * @param node 目标节点。
   * @param reader XML 读取器。
   * @param xml_node_data 用于存储连接信息的 XMLNodeData 对象。
   */
  void LoadInput(Node *node, QXmlStreamReader *reader, XMLNodeData &xml_node_data) const;

  /**
   * @brief (私有成员函数) 从 XML 读取器加载一个“立即值”。
   *  此版本与基类或之前版本的静态方法不同，变为成员函数，可能因为需要访问 this 指针相关的状态或方法。
   * @param reader XML 读取器。
   * @param node 目标节点。
   * @param input 输入参数的名称。
   * @param element 输入参数的元素索引。
   * @param xml_node_data XMLNodeData 对象。
   */
  void LoadImmediate(QXmlStreamReader *reader, Node *node, const QString &input, int element,
                     XMLNodeData &xml_node_data) const;

  /**
   * @brief (私有静态成员函数) 从 XML 读取器加载单个关键帧 (NodeKeyframe) 的数据。
   * @param reader XML 读取器。
   * @param key 指向 NodeKeyframe 对象的指针，用于存储加载的数据。
   * @param data_type 关键帧数据的值类型。
   */
  static void LoadKeyframe(QXmlStreamReader *reader, NodeKeyframe *key, NodeValue::Type data_type);

  /**
   * @brief (私有静态成员函数) 从 XML 读取器加载节点的位置信息。
   * @param reader XML 读取器。
   * @param node_ptr (输出参数) 指向存储节点指针占位符的指针。
   * @param pos (输出参数) 指向存储节点位置的指针。
   * @return 如果成功加载位置信息则返回 true，否则返回 false。
   */
  static bool LoadPosition(QXmlStreamReader *reader, quintptr *node_ptr, Node::Position *pos);

  /**
   * @brief (私有静态成员函数) 在所有节点加载完毕后建立节点间的连接。
   * @param xml_node_data 包含待连接信息和节点指针映射的 XMLNodeData 对象。
   */
  static void PostConnect(const XMLNodeData &xml_node_data);

  /**
   * @brief (私有成员函数) 从 XML 读取器加载特定节点类型的自定义数据。
   * @param reader XML 读取器。
   * @param node 目标节点。
   * @param xml_node_data XMLNodeData 对象。
   */
  void LoadNodeCustom(QXmlStreamReader *reader, Node *node, XMLNodeData &xml_node_data) const;

  /**
   * @brief (私有成员函数) 从 XML 读取器加载与 ViewerOutput 相关的时间线点。
   *  与基类或之前版本的静态方法不同，变为成员函数。
   * @param reader XML 读取器。
   * @param viewer 指向 ViewerOutput 对象的指针。
   */
  void LoadTimelinePoints(QXmlStreamReader *reader, ViewerOutput *viewer) const;  // 注意：参数名从 points 改为 viewer

  /**
   * @brief (私有静态成员函数) 从 XML 读取器加载单个时间线标记 (TimelineMarker) 的数据。
   * @param reader XML 读取器。
   * @param marker 指向 TimelineMarker 对象的指针，用于存储加载的数据。
   */
  static void LoadMarker(QXmlStreamReader *reader, TimelineMarker *marker);

  /**
   * @brief (私有静态成员函数) 从 XML 读取器加载时间线工作区的数据。
   * @param reader XML 读取器。
   * @param workarea 指向 TimelineWorkArea 对象的指针。
   */
  static void LoadWorkArea(QXmlStreamReader *reader, TimelineWorkArea *workarea);

  /**
   * @brief (私有静态成员函数) 从 XML 读取器加载时间线标记列表的数据。
   * @param reader XML 读取器。
   * @param markers 指向 TimelineMarkerList 对象的指针。
   */
  static void LoadMarkerList(QXmlStreamReader *reader, TimelineMarkerList *markers);

  /**
   * @brief (私有静态成员函数) 从 XML 读取器加载节点参数的值提示信息。
   * @param hint 指向 Node::ValueHint 对象的指针，用于存储加载的数据。
   * @param reader XML 读取器。
   */
  static void LoadValueHint(Node::ValueHint *hint, QXmlStreamReader *reader);
};

}  // namespace olive

#endif  // SERIALIZER220403_H // 头文件宏定义结束