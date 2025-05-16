#ifndef NODETABLEVIEW_H
#define NODETABLEVIEW_H

#include <QMap>         // Qt 映射容器 (用于 top_level_item_map_)
#include <QTreeWidget>  // Qt 树形控件基类
#include <QVector>      // Qt 动态数组容器 (用于 SelectNodes/DeselectNodes 参数)

#include "node/node.h"        // 节点基类定义
#include "olive/core/core.h"  // 包含 rational 等核心类型 (通过 node/node.h 间接包含，或直接需要)

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QWidget;
// class QTreeWidgetItem;

// 前向声明项目内部类 (根据用户要求，不添加)
// class rational; // 已通过 olive/core/core.h 包含

namespace olive {

/**
 * @brief NodeTableView 类是一个用于以表格或树形结构显示节点信息的控件。
 *
 * 它继承自 QTreeWidget，可以展示项目中的节点，并可能根据时间更新节点相关的状态或值。
 * 该控件维护一个从 Node 指针到 QTreeWidgetItem 指针的映射，以便于管理和访问树中的项。
 */
class NodeTableView : public QTreeWidget {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit NodeTableView(QWidget* parent = nullptr);

  /**
   * @brief 在视图中选中指定的节点列表。
   * @param nodes 包含要选中的 Node 指针的 QVector。
   */
  void SelectNodes(const QVector<Node*>& nodes);

  /**
   * @brief 在视图中取消选中指定的节点列表。
   * @param nodes 包含要取消选中的 Node 指针的 QVector。
   */
  void DeselectNodes(const QVector<Node*>& nodes);

  /**
   * @brief 设置当前时间，可能会触发视图中节点信息的更新。
   * @param time 新的当前时间 (rational 类型)。
   */
  void SetTime(const rational& time);

 private:
  QMap<Node*, QTreeWidgetItem*> top_level_item_map_;  ///< 存储顶层节点与其对应的 QTreeWidgetItem 的映射。
  ///< 用于快速查找和更新特定节点的树形项。

  rational last_time_;  ///< 上一次设置的时间，可能用于检测时间变化并触发更新。
};

}  // namespace olive

#endif  // NODETABLEVIEW_H
