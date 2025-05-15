#ifndef SERIALIZEDDATA_H // 防止头文件被重复包含的宏
#define SERIALIZEDDATA_H // 定义 SERIALIZEDDATA_H 宏

#include <QHash>    // Qt 哈希表容器
#include <QVariant> // Qt 通用数据类型 QVariant

#include "node.h"   // 包含 Node 类的定义 (可能为了 Node::Position 等)

namespace olive { // olive 项目的命名空间

class NodeGroup; // 向前声明 NodeGroup 类

/**
 * @brief SerializedData 结构体用于在项目加载和保存过程中临时存储和传递反序列化或待序列化的数据。
 *
 * 这个结构体通常在 XML 解析阶段填充，然后在所有节点对象被创建后，用于恢复节点间的连接、
 * 节点在上下文中的位置以及其他需要跨对象引用的数据。
 * quintptr 通常用于在序列化时存储一个唯一的标识符 (如临时ID或内存地址的某种表示)，
 * 在反序列化后再将其解析回实际的节点指针。
 */
struct SerializedData {
  // 用于存储序列化连接信息的结构体
  struct SerializedConnection {
    NodeInput input;      // 目标输入端口 (包含下游节点指针、输入ID和元素索引)
    quintptr output_node{}; // 连接到此输入的上游输出节点的临时标识符 (例如，在XML中读取的ID)
  };

  // 用于存储“块链接”(Block Link)信息的结构体
  // "块链接"可能是指一种特定类型的节点间关系，不同于标准的输入输出数据流连接
  struct BlockLink {
    Node *block;   // 发起链接的节点 (块)
    quintptr link; // 被链接的目标节点的临时标识符
  };

  // 用于存储节点组 (NodeGroup) 的透传链接 (Passthrough Link) 信息的结构体
  // 这通常用于将组外部的输入连接到组内部的某个节点，或将组内部的某个节点输出暴露到组外部
  struct GroupLink {
    NodeGroup *group{};         // 相关的节点组
    QString passthrough_id;     // 节点组上透传端口的ID
    quintptr input_node{};      // (对于输入透传) 组内部连接到的目标节点的临时标识符
                                // (对于输出透传) 组内部提供输出的源节点的临时标识符
    QString input_id;           // (对于输入透传) 组内部目标节点上的输入ID
                                // (对于输出透传) 组内部源节点上的输出ID (虽然变量名是input_id，但用途取决于上下文)
    int input_element{};        // 相关的元素索引
    QString custom_name;        // 透传端口的自定义名称
    InputFlags custom_flags;    // 透传端口的自定义标志
    NodeValue::Type data_type;  // 透传端口的数据类型
    QVariant default_val;       // 透传端口的默认值
    QHash<QString, QVariant> custom_properties; // 透传端口的自定义属性
  };

  // 存储节点在不同上下文中的位置信息
  // 外层 QMap: 上下文节点 (Node*) -> 内层 QMap
  // 内层 QMap: 目标节点的临时标识符 (quintptr) -> 目标节点在该上下文中的位置 (Node::Position)
  QMap<Node *, QMap<quintptr, Node::Position> > positions;

  // 存储从临时标识符到实际 Node 指针的映射，在所有节点对象创建后填充
  // quintptr (例如，从XML读取的ID) -> Node* (对应的节点对象指针)
  QHash<quintptr, Node *> node_ptrs;

  // 存储期望建立的连接列表，在所有节点指针解析完毕后用于实际连接操作
  QList<SerializedConnection> desired_connections;

  // 存储期望建立的“块链接”列表
  QList<BlockLink> block_links;

  // 存储节点组的输入透传链接信息列表
  QVector<GroupLink> group_input_links;

  // 存储节点组的输出透传链接信息
  // NodeGroup* (节点组) -> quintptr (组内部提供输出的节点的临时标识符)
  QHash<NodeGroup *, quintptr> group_output_links;
};

}  // namespace olive

#endif  // SERIALIZEDDATA_H