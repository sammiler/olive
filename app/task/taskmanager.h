#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QUndoCommand> // 引入 QUndoCommand 类，用于撤销/重做操作，但在此文件中似乎未直接使用，可能是上下文依赖或未来用途
#include <QVector>      // 引入 QVector 类，一种动态数组容器
#include <QtConcurrent/QtConcurrent> // 引入 QtConcurrent 模块，用于简化多线程编程

#include "task/task.h" // 引入 Task 类的定义，TaskManager 管理的对象类型

namespace olive {

/**
 * @brief 任务管理器类
 *
 * TaskManager 负责管理后台 Task 对象的生命周期，处理它们的启动和结束。
 * 创建新的 Task 后，应通过 AddTask() 方法将其提交给 TaskManager。
 * TaskManager 将获得任务的所有权，并将其添加到一个队列中，直到系统资源允许其运行。
 * 目前，TaskManager 运行的任务数量不会超过系统上的线程数（每个线程一个任务）。
 * 当任务完成时，TaskManager 将启动队列中的下一个任务。
 */
class TaskManager : public QObject {
  Q_OBJECT
 public:
  /**
   * @brief TaskManager 构造函数
   */
  TaskManager();

  /**
   * @brief TaskManager 析构函数
   *
   * 确保所有 Task 对象都被删除。
   */
  ~TaskManager() override;

  /**
   * @brief 创建 TaskManager 的单例实例
   *
   * 如果实例尚不存在，则创建它。
   */
  static void CreateInstance();

  /**
   * @brief 销毁 TaskManager 的单例实例
   *
   * 释放单例实例占用的资源。
   */
  static void DestroyInstance();

  /**
   * @brief 获取 TaskManager 的单例实例
   *
   * @return 返回 TaskManager 的静态单例指针。
   */
  static TaskManager* instance();

  /**
   * @brief 获取当前管理的任务数量
   *
   * @return 返回正在运行或排队等待的任务总数。
   */
  [[nodiscard]] int GetTaskCount() const;

  /**
   * @brief 获取队列中的第一个任务
   *
   * @return 返回任务队列中的第一个 Task 对象指针，如果队列为空则可能返回 nullptr。
   * 注意：具体行为（例如是否移除任务）取决于实现。
   */
  [[nodiscard]] Task* GetFirstTask() const;

  /**
   * @brief 取消指定任务并等待其完成
   *
   * 该方法会请求取消一个任务，并阻塞当前线程直到该任务实际结束（无论是正常完成还是被取消）。
   * @param t 要取消并等待的任务指针。
   */
  void CancelTaskAndWait(Task* t);

 public slots:
  /**
   * @brief 添加一个新任务到队列中
   *
   * 将新的 Task 对象添加到任务队列。如果当前有可用的线程资源，任务将立即运行。
   * 否则，它将被放入队列中，并在资源可用时运行。
   *
   * 注意：此函数非线程安全，目前设计为仅在主线程/GUI线程中使用。
   *
   * 注意：一个 Task 对象只应被添加一次。多次添加同一个 Task 对象将导致未定义行为。
   *
   * @param t 要添加和运行的任务指针。TaskManager 将获得此 Task 的所有权，并负责释放它。
   */
  void AddTask(Task* t);

  /**
   * @brief 取消一个指定的任务
   *
   * 请求取消一个正在运行或在队列中的任务。
   * @param t 要取消的任务指针。
   */
  void CancelTask(Task* t);

 signals:
  /**
   * @brief 当通过 AddTask() 添加任务时发出的信号
   *
   * @param t 被添加的任务的指针。
   */
  void TaskAdded(Task* t);

  /**
   * @brief 当运行中的任务列表发生任何更改时发出的信号
   *
   * 例如，任务开始、结束、被添加或被移除时。
   */
  void TaskListChanged();

  /**
   * @brief 当一个任务被删除时发出的信号
   *
   * @param t 被删除的任务的指针。
   */
  void TaskRemoved(Task* t);

  /**
   * @brief 当一个任务执行失败时发出的信号
   *
   * @param t 执行失败的任务的指针。
   */
  void TaskFailed(Task* t);

 private:
  /**
   * @brief 存储活动任务的哈希表
   *
   * 使用 QFutureWatcher 作为键，关联到对应的 Task 对象。
   * QFutureWatcher 用于监视异步任务的执行状态。
   */
  QHash<QFutureWatcher<bool>*, Task*> tasks_;

  /**
   * @brief 存储执行失败的任务列表
   */
  std::list<Task*> failed_tasks_;

  /**
   * @brief 用于执行任务的线程池
   *
   * QThreadPool 管理一组线程，用于并发执行任务。
   */
  QThreadPool thread_pool_;

  /**
   * @brief TaskManager 的静态单例实例指针
   */
  static TaskManager* instance_;

 private slots:
  /**
   * @brief 任务完成时调用的槽函数
   *
   * 当一个由 QFutureWatcher 监视的任务完成时，此槽函数会被触发。
   * 它负责处理任务完成后的清理工作，并可能启动队列中的下一个任务。
   */
  void TaskFinished();
};

}  // namespace olive

#endif  // TASKMANAGER_H
