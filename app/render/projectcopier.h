#ifndef PROJECTCOPIER_H  // 防止头文件被重复包含的宏
#define PROJECTCOPIER_H  // 定义 PROJECTCOPIER_H 宏

#include "node/project.h"  // 包含 Project 类的定义

// 假设 Node, NodeInput, JobTime, QObject, QHash, QVector, std::list 等类型
// 已通过 "node/project.h" 或其他方式被间接包含。

namespace olive {  // olive 项目的命名空间

/**
 * @brief ProjectCopier 类负责创建一个项目 (Project) 的副本，并管理该副本与原始项目之间的同步。
 *
 * 主要用途是在后台渲染线程中安全地访问项目数据。直接在渲染线程中访问主线程的项目对象
 * 是不安全的，可能导致数据竞争和崩溃。ProjectCopier 通过创建一个项目的深拷贝 (或部分深拷贝)，
 * 使得渲染线程可以操作这个副本，而不会影响主线程中的原始项目。
 *
 * 它会监听原始项目的变化 (如节点添加/删除、连接改变、参数值改变)，并将这些变化排队。
 * 当渲染线程空闲时 (例如，RenderManager 没有在读取副本时)，这些排队的变化会被应用到项目副本上，
 * 从而保持副本与原始项目在一定程度上的同步。
 *
 * ProjectCopier 还维护了一个从原始节点到其副本节点的映射 (`copy_map_`)，以便在需要时进行转换。
 */
class ProjectCopier : public QObject {  // ProjectCopier 继承自 QObject
 Q_OBJECT                               // 声明此类使用 Qt 的元对象系统

     public :
     /**
      * @brief 构造函数。
      * @param parent 父对象指针，默认为 nullptr。
      */
     explicit ProjectCopier(QObject *parent = nullptr);

  /**
   * @brief 设置要进行拷贝和同步的原始项目。
   * 调用此方法后，ProjectCopier 会创建 `project` 的一个副本。
   * @param project 指向原始 Project 的指针。
   */
  void SetProject(Project *project);

  /**
   * @brief (模板方法) 根据原始对象获取其在副本中的对应对象。
   * @tparam T 对象的类型。
   * @param original 指向原始项目中某个对象的指针。
   * @return 返回指向副本中对应对象的指针，如果未找到则可能返回 nullptr 或无效指针。
   *         (通常用于获取节点的副本)
   */
  template <typename T>
  T *GetCopy(T *original) {
    // 从 copy_map_ 中查找原始节点对应的副本节点，并进行静态类型转换
    return static_cast<T *>(copy_map_.value(original));
  }

  /**
   * @brief (模板方法) 根据副本中的对象获取其在原始项目中的对应对象。
   * @tparam T 对象的类型。
   * @param copy 指向项目副本中某个对象的指针。
   * @return 返回指向原始项目中对应对象的指针。
   */
  template <typename T>
  T *GetOriginal(T *copy) {
    // 从 copy_map_ 中根据副本节点反向查找原始节点
    return static_cast<T *>(copy_map_.key(copy));
  }

  /**
   * @brief 获取内部维护的项目副本。
   * @return 返回指向 Project 副本的指针。
   */
  [[nodiscard]] Project *GetCopiedProject() const { return copy_; }

  /**
   * @brief 获取从原始节点到其副本节点的映射表。
   * @return 返回一个 QHash<Node *, Node *> 的常量引用。
   */
  [[nodiscard]] const QHash<Node *, Node *> &GetNodeMap() const { return copy_map_; }

  /**
   * @brief 获取原始项目图 (NodeGraph) 上次发生结构性更改的时间。
   * @return 返回 JobTime 对象的常量引用。
   */
  [[nodiscard]] const JobTime &GetGraphChangeTime() const { return graph_changed_time_; }
  /**
   * @brief 获取项目副本上次与原始项目同步更新的时间。
   * @return 返回 JobTime 对象的常量引用。
   */
  [[nodiscard]] const JobTime &GetLastUpdateTime() const { return last_update_time_; }

  /**
   * @brief 检查当前是否有待处理的图更新在队列中。
   * @return 如果更新队列不为空，则返回 true。
   */
  [[nodiscard]] bool HasUpdatesInQueue() const { return !graph_update_queue_.empty(); }

  /**
   * @brief 处理所有排队的更改，并将其应用到内部的节点图副本上。
   *
   * PreviewAutoCacher 会错开对其内部节点图副本的更新，仅当 RenderManager
   * 没有在读取该副本时才应用这些更新。当这样的机会出现时，会调用此函数。
   */
  void ProcessUpdateQueue();

 signals:  // Qt 信号声明
  /**
   * @brief 当一个节点被添加到项目副本时发出的信号。
   * @param n 被添加到副本中的节点指针 (指向副本中的节点)。
   */
  void AddedNode(Node *n);
  /**
   * @brief 当一个节点从项目副本中移除时发出的信号。
   * @param n 从副本中移除的节点指针 (指向副本中的节点)。
   */
  void RemovedNode(Node *n);

 private:
  // --- 私有方法，用于在项目副本上实际执行排队的操作 ---
  void DoNodeAdd(Node *node);                                             // 在副本中添加节点
  void DoNodeRemove(Node *node);                                          // 从副本中移除节点
  void DoEdgeAdd(Node *output, const NodeInput &input);                   // 在副本中添加边连接
  void DoEdgeRemove(Node *output, const NodeInput &input);                // 在副本中移除边连接
  void DoValueChange(const NodeInput &input);                             // 在副本中更新参数值
  void DoValueHintChange(const NodeInput &input);                         // 在副本中更新值提示
  void DoProjectSettingChange(const QString &key, const QString &value);  // 在副本中更新项目设置

  // 将原始节点与其副本节点之间的映射关系插入到 copy_map_ 中
  void InsertIntoCopyMap(Node *node, Node *copy);

  // 更新图结构更改时间戳
  void UpdateGraphChangeValue();
  // 更新上次同步时间戳
  void UpdateLastSyncedValue();

  Project *original_;  // 指向原始项目的指针
  Project *copy_;      // 指向项目副本的指针 (由 ProjectCopier 创建和管理)

  // 内部类，用于表示一个排队的更新作业
  class QueuedJob {
   public:
    // 定义更新作业的类型枚举
    enum Type {
      kNodeAdded,             // 节点添加
      kNodeRemoved,           // 节点移除
      kEdgeAdded,             // 边连接添加
      kEdgeRemoved,           // 边连接移除
      kValueChanged,          // 参数值改变
      kValueHintChanged,      // 值提示改变
      kProjectSettingChanged  // 项目设置改变
    };

    Type type;        // 作业类型
    Node *node;       // 相关的节点 (例如，被添加/移除的节点，或参数所属的节点)
    NodeInput input;  // 相关的输入端口 (例如，值改变的输入，或连接的输入端)
    Node *output;     // 相关的输出节点 (例如，连接的输出端)

    QString key;    // (用于项目设置) 设置的键
    QString value;  // (用于项目设置) 设置的值
  };

  // 存储待处理的图更新作业的队列
  std::list<QueuedJob> graph_update_queue_;
  // 映射表：原始节点指针 -> 副本节点指针
  QHash<Node *, Node *> copy_map_;
  // 映射表：原始项目指针 -> 副本项目指针 (通常只包含一对)
  QHash<Project *, Project *> graph_map_;
  // 存储在拷贝过程中新创建的节点 (副本中的节点)
  QVector<Node *> created_nodes_;

  JobTime graph_changed_time_;  // 记录原始图结构上次更改的时间
  JobTime last_update_time_;    // 记录副本上次与原始图同步的时间

 private slots:  // Qt 私有槽函数
  // --- 当原始项目发生变化时，以下槽函数会被调用，将变化加入到更新队列中 ---
  void QueueNodeAdd(Node *node);
  void QueueNodeRemove(Node *node);
  void QueueEdgeAdd(Node *output, const NodeInput &input);
  void QueueEdgeRemove(Node *output, const NodeInput &input);
  void QueueValueChange(const NodeInput &input);
  void QueueValueHintChange(const NodeInput &input);
  void QueueProjectSettingChange(const QString &key, const QString &value);
};

}  // namespace olive

#endif  // PROJECTCOPIER_H