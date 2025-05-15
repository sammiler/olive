#ifndef RENDERTICKET_H // 防止头文件被重复包含的宏
#define RENDERTICKET_H // 定义 RENDERTICKET_H 宏

#include <QDateTime>     // Qt 日期时间类 (虽然未直接使用，但可能与任务时间戳或超时相关)
#include <QMutex>        // Qt 互斥锁类
#include <QWaitCondition> // Qt 等待条件变量类

#include "codec/frame.h"           // 包含 Frame (或 FramePtr) 相关的定义 (RenderTicket 可能返回帧)
#include "common/cancelableobject.h" // 包含 CancelableObject 基类的定义 (RenderTicket 可以被取消)
#include "node/output/viewer/viewer.h" // ViewerOutput 定义 (可能与票据关联的上下文相关)

// 假设 QObject, QVariant, std::shared_ptr 已通过其他方式被间接包含。

namespace olive { // olive 项目的命名空间

/**
 * @brief RenderTicket 类代表一个异步渲染操作的“票据”或句柄。
 *
 * 当一个渲染任务 (如生成一帧视频或一段音频) 被提交给 RenderManager 时，
 * RenderManager 会返回一个 RenderTicket。调用者可以使用这个票据来：
 * - 检查任务的运行状态 (IsRunning)。
 * - 等待任务完成 (WaitForFinished)。
 * - 获取任务的执行结果 (Get)。
 * - 取消任务 (通过继承自 CancelableObject 的方法)。
 *
 * RenderTicket 内部使用互斥锁和等待条件变量来确保线程安全，并实现阻塞等待。
 * 它也维护了任务的完成次数和结果。
 */
class RenderTicket : public QObject, public CancelableObject { // RenderTicket 继承自 QObject 和 CancelableObject
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  // 构造函数，初始化状态
  RenderTicket();

  /**
   * @brief 获取票据当前的状态 (是否正在运行)。
   *
   * 此函数是线程安全的，除非 `lock` 参数被设置为 false。如果 `lock` 为 false，
   * 调用者有责任在此函数调用前后锁定和解锁互斥锁。
   * @param lock (可选) 是否在此函数内部进行加锁，默认为 true。
   * @return 如果任务正在运行，返回 true。
   */
  bool IsRunning(bool lock = true);

  /**
   * @brief 获取票据已完成的次数。
   *
   * (注释同 IsRunning 关于线程安全和 `lock` 参数的说明)
   * @param lock (可选) 是否在此函数内部进行加锁，默认为 true。
   * @return 返回任务完成的次数。
   */
  int GetFinishCount(bool lock = true);

  /**
   * @brief 检查此票据是否已有结果。
   *
   * 如果此票据正在运行，此函数将始终返回 false。
   * @return 如果任务已完成并有结果，返回 true。
   */
  bool HasResult();

  /**
   * @brief 获取任务的结果 (如果有)。
   * @return 返回一个 QVariant，其中包含任务的结果。如果无结果或任务未完成，QVariant 可能无效。
   */
  QVariant Get();

  /**
   * @brief 等待票据代表的任务完成。
   *
   * 如果此票据当前未运行 (即已完成或从未开始)，此函数将立即返回。
   * 否则，当前线程将阻塞，直到任务完成。
   */
  void WaitForFinished();
  /**
   * @brief 等待票据完成，使用外部传入的互斥锁进行同步。
   * (这允许在更复杂的同步场景中使用)
   * @param mutex 指向外部 QMutex 的指针，在等待期间会临时解锁它。
   */
  void WaitForFinished(QMutex* mutex);

  /**
   * @brief 访问此票据的内部互斥锁。
   *
   * 当您需要对一个票据执行多个操作并确保这些操作期间的线程安全时使用。
   * 大多数情况下不需要这样做，因为所有函数默认都是线程安全的。
   * @return 返回指向内部 QMutex 的指针。
   */
  QMutex* lock() { return &lock_; }

  /**
   * @brief 通知票据它所代表的任务已开始运行。
   *
   * 如果之前设置了任何结果值，它将被清除。
   * 运行状态设置为 true，完成计数器递增 (通常在 Start 时也需要处理完成计数，或由 Finish 处理)。
   */
  void Start();

  /**
   * @brief 以无结果的方式完成票据。
   *
   * 将票据设置为不再运行，并假定它没有收到任何结果。
   * 完成计数器递增，并唤醒所有等待此票据的线程。
   */
  void Finish();

  /**
   * @brief 以有结果的方式完成票据。
   *
   * 将票据设置为不再运行，并提供由所请求操作生成的值。
   * @param result 任务的执行结果。
   */
  void Finish(QVariant result);

 signals: // Qt 信号声明
  /**
   * @brief 当通过任何方式 (无论是被取消还是带有结果) 调用了 finish 时发出的信号。
   */
  void Finished();

 private:
  /**
   * @brief 内部完成票据的函数。
   * @param has_result 标记是否有结果。
   * @param result 任务的结果 (如果 has_result 为 true)。
   */
  void FinishInternal(bool has_result, QVariant result);

  bool is_running_; // 标记任务是否正在运行

  QVariant result_; // 存储任务的结果

  bool has_result_; // 标记是否有有效的结果

  int finish_count_; // 记录任务完成的次数

  QMutex lock_; // 互斥锁，用于保护对票据状态的并发访问

  QWaitCondition wait_; // 等待条件变量，用于实现 WaitForFinished 的阻塞等待
};

// 类型别名：RenderTicketPtr 是一个指向 RenderTicket 对象的共享指针 (std::shared_ptr)
using RenderTicketPtr = std::shared_ptr<RenderTicket>;

/**
 * @brief RenderTicketWatcher 类是一个 QObject，用于“观察”一个 RenderTicket 的状态。
 *
 * 它提供了一种基于信号槽机制来响应 RenderTicket 完成事件的方式，
 * 而不是通过阻塞等待 (WaitForFinished)。
 * 当被观察的 RenderTicket 完成时，RenderTicketWatcher 会发出自己的 Finished 信号。
 *
 * 这对于在UI线程中异步地处理渲染结果非常有用，可以避免UI卡顿。
 */
class RenderTicketWatcher : public QObject { // RenderTicketWatcher 继承自 QObject
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  /**
   * @brief 构造函数。
   * @param parent 父对象指针，默认为 nullptr。
   */
  explicit RenderTicketWatcher(QObject* parent = nullptr);

  /**
   * @brief 获取当前正在观察的 RenderTicket。
   * @return 返回 RenderTicketPtr。
   */
  [[nodiscard]] RenderTicketPtr GetTicket() const { return ticket_; }

  /**
   * @brief 设置要观察的 RenderTicket。
   * 如果之前已观察其他票据，会断开与旧票据的连接。
   * 然后连接到新票据的 Finished 信号。
   * @param ticket 指向要观察的 RenderTicket 的共享指针。
   */
  void SetTicket(const RenderTicketPtr& ticket);

  /**
   * @brief 检查被观察的票据是否正在运行。
   * @return 如果票据存在且正在运行，返回 true。
   */
  bool IsRunning();

  /**
   * @brief 等待被观察的票据完成。
   * (注意：在UI线程中直接调用此方法仍可能导致阻塞，除非票据已完成)
   */
  void WaitForFinished();

  /**
   * @brief 获取被观察票据的结果。
   * @return 返回票据的结果 (QVariant)。
   */
  QVariant Get();

  /**
   * @brief 检查被观察的票据是否已有结果。
   * @return 如果票据存在且有结果，返回 true。
   */
  bool HasResult();

  /**
   * @brief 取消被观察的 RenderTicket。
   */
  void Cancel();

 signals: // Qt 信号声明
  /**
   * @brief 当被观察的 RenderTicket 完成时发出的信号。
   * @param watcher 指向发出此信号的 RenderTicketWatcher 实例的指针。
   *                (允许接收者知道是哪个观察者对应的票据完成了)
   */
  void Finished(RenderTicketWatcher* watcher);

 private:
  RenderTicketPtr ticket_; // 指向当前正在观察的 RenderTicket 的共享指针

 private slots: // Qt 私有槽函数
  /**
   * @brief 当被观察的 RenderTicket 发出 Finished 信号时调用的槽函数。
   * 此槽函数会进一步发出 RenderTicketWatcher 自身的 Finished 信号。
   */
  void TicketFinished();
};

}  // namespace olive

// 声明 RenderTicketPtr 类型为元类型，以便在 QVariant 中使用或在信号槽中传递
Q_DECLARE_METATYPE(olive::RenderTicketPtr)

#endif  // RENDERTICKET_H