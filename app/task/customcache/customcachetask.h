#ifndef CUSTOMCACHETASK_H  // 防止头文件被重复包含的预处理器指令
#define CUSTOMCACHETASK_H  // 定义 CUSTOMCACHETASK_H 宏

#include <QMutex>          // 包含了 Qt 的互斥锁类，用于线程同步
#include <QWaitCondition>  // 包含了 Qt 的等待条件类，用于线程间的条件变量同步

#include "task/task.h"  // 包含了任务基类的头文件

namespace olive {  // olive 项目的命名空间

/**
 * @brief CustomCacheTask 类定义，继承自 Task 类。
 *
 * 此类专门用于处理自定义缓存相关的任务。它管理着缓存操作的执行、
 * 取消以及线程间的同步。通常用于后台执行可能耗时的缓存生成或更新操作，
 * 以避免阻塞用户界面。
 */
class CustomCacheTask : public Task {
 Q_OBJECT  // Qt 对象的宏，用于启用信号和槽机制等 Qt 特性
     public :
     /**
      * @brief CustomCacheTask 的构造函数。
      * @param sequence_name 要进行缓存操作的序列名称。这个名称可能用于标识或定位相关的缓存数据。
      */
     explicit CustomCacheTask(const QString &sequence_name);

  /**
   * @brief 标记任务完成并唤醒可能正在等待的线程。
   *
   * 这个方法用于从外部通知任务其操作已完成（或应被视为完成），
   * 特别是在任务的取消逻辑中，用于确保任务能正确地结束并释放资源。
   * 它会设置一个标志位并唤醒等待条件。
   */
  void Finish();

 signals:
  /**
   * @brief 当任务被取消时发射此信号。
   *
   * 连接到此信号的槽函数可以在任务被取消时执行清理操作或更新UI状态。
   */
  void Cancelled();

 protected:
  /**
   * @brief 执行自定义缓存任务的核心逻辑。
   *
   * 此方法重写自基类 Task 的 Run 方法。当任务被调度执行时，此函数会被调用。
   * 它负责执行实际的缓存操作，并会等待直到任务被取消或通过 Finish() 方法完成。
   * @return 如果任务是由于调用 Finish() 而非外部取消事件正常结束，则返回 true；否则返回 false。
   */
  bool Run() override;

  /**
   * @brief 处理任务取消事件。
   *
   * 此方法重写自基类 Task 的 CancelEvent 方法。当任务接收到取消请求时，
   * 此函数会被调用。它负责设置取消标志，发射 Cancelled() 信号，
   * 并唤醒可能因等待条件而阻塞的 Run() 方法。
   */
  void CancelEvent() override;

 private:
  QMutex mutex_;  ///< @brief 互斥锁，用于保护共享资源和确保线程安全访问。

  QWaitCondition wait_cond_;  ///< @brief 等待条件变量，用于在 Run() 方法中等待任务完成或取消的信号。

  bool cancelled_through_finish_;  ///< @brief 标志位，指示任务是否是通过调用 Finish() 方法来结束的。
  ///< 如果为 true，表示任务是正常完成（或被外部标记为完成）；
  ///< 如果为 false，表示任务可能是因为外部的 CancelEvent 而结束。
};

}  // namespace olive

#endif  // CUSTOMCACHETASK_H // 结束预处理器指令 #ifndef CUSTOMCACHETASK_H