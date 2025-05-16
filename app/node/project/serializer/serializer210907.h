#ifndef SERIALIZER210907_H  // 防止头文件被多次包含的宏定义开始 (文件名是 SERIALIZER210907_H)
#define SERIALIZER210907_H

#include "serializer.h"  // 引入基类 ProjectSerializer 的定义 (文件名可能是 "projectserializer.h")

// 可能需要的前向声明 (与 ProjectSerializer210528.h 类似)
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

namespace olive {  // Olive 编辑器的命名空间

/**
 * @brief 特定版本的项目序列化器，版本号为 210907。
 * 这个类继承自 ProjectSerializer，并实现了针对版本号为 210907 的项目文件的
 * 具体加载逻辑。它与 ProjectSerializer210528 的结构非常相似，
 * 可能仅在某些细节的加载方式或支持的特性上有所不同。
 */
class ProjectSerializer210907 : public ProjectSerializer {
 public:
  /**
   * @brief ProjectSerializer210907 默认构造函数。
   */
  ProjectSerializer210907() = default;

 protected:
  /**
   * @brief 实现从 XML 流加载版本号为 210907 的项目数据的具体逻辑。
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
   * @return uint 版本号，固定为 210907。
   */
  [[nodiscard]] uint Version() const override { return 210907; }  // 此序列化器处理版本 210907

 private:
  /**
   * @brief 在加载过程中临时存储从 XML 解析出的节点相关数据的内部结构体。
   *  结构与 ProjectSerializer210528::XMLNodeData 相同或非常相似。
   */
  struct XMLNodeData {
    /** @brief 存储一个待建立的节点连接的信息。注意：与210528版本相比，SerializedConnection 中移除了 output_param。 */
    struct SerializedConnection {
      NodeInput input;         ///< 目标节点的输入参数。
      quintptr output_node{};  ///< 源节点的指针占位符。
    };

    /** @brief 存储一个媒体块（Block）与其链接对象之间的链接信息。 */
    struct BlockLink {
      Node *block;    ///< 指向媒体块的指针。
      quintptr link;  ///< 指向链接对象的指针占位符。
    };

    /** @brief 存储节点组（NodeGroup）的输入与其内部节点输入之间的链接信息。 */
    struct GroupLink {
      NodeGroup *group{};     ///< 指向节点组的指针。
      quintptr input_node{};  ///< 节点组内部目标节点的指针占位符。
      QString input_id;       ///< 节点组内部目标节点输入参数的ID。
      int input_element{};    ///< 节点组内部目标节点输入参数的元素索引。
    };

    QHash<quintptr, Node *> node_ptrs;                ///< 节点指针占位符到实际 Node 指针的映射。
    QList<SerializedConnection> desired_connections;  ///< 待建立的节点连接列表。
    QList<BlockLink> block_links;                     ///< 待建立的媒体块链接列表。
    QVector<GroupLink> group_input_links;             ///< 待建立的节点组输入链接列表。
    QHash<NodeGroup *, quintptr> group_output_links;  ///< 节点组输出链接信息。
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
   * @brief (私有静态成员函数) 从 XML 读取器加载一个“立即值”。
   * @param reader XML 读取器。
   * @param node 目标节点。
   * @param input 输入参数的名称。
   * @param element 输入参数的元素索引。
   * @param xml_node_data XMLNodeData 对象。
   */
  static void LoadImmediate(QXmlStreamReader *reader, Node *node, const QString &input, int element,
                            XMLNodeData &xml_node_data);

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
   * @brief (私有静态成员函数) 从 XML 读取器加载与 ViewerOutput 相关的时间线点。
   * @param reader XML 读取器。
   * @param points 指向 ViewerOutput 对象的指针。
   */
  static void LoadTimelinePoints(QXmlStreamReader *reader, ViewerOutput *points);

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

#endif  // SERIALIZER211228_H // 头文件宏定义结束 (注意：此处的宏名称 SERIALIZER211228_H 与文件名 SERIALIZER210907_H
        // 不匹配)