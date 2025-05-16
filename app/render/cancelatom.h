#ifndef CANCELATOM_H  // 防止头文件被重复包含的宏
#define CANCELATOM_H  // 定义 CANCELATOM_H 宏

#include <QMutex>  // Qt 互斥锁类，用于线程同步

namespace olive {  // olive 项目的命名空间

/**
 * @brief CancelAtom 类提供了一个简单的线程安全的取消机制。
 *
 * 它通常用于在耗时操作 (如渲染、文件处理) 中，允许一个线程向另一个正在执行操作的线程
 * 发出取消请求，并让执行操作的线程能够安全地检查这个取消状态。
 *
 * 主要特点：
 * - `cancelled_`: 一个布尔标志，表示是否已请求取消。
 * - `heard_`: 一个布尔标志，表示执行操作的线程是否已经“听到”或注意到取消请求。
 *   这可以用于某些情况下，即使操作已被取消，也希望它能完成当前一小步迭代或进行一些清理。
 * - `QMutex`: 用于保护对 `cancelled_` 和 `heard_` 标志的并发访问，确保线程安全。
 *
 * 使用场景：
 * - 渲染线程在每个处理步骤开始前调用 `IsCancelled()` 来检查是否需要提前终止。
 * - 主线程或其他控制线程可以通过调用 `Cancel()` 来请求渲染线程停止。
 */
class CancelAtom {
 public:
  // 构造函数，初始化取消状态为 false，听到取消状态也为 false
  CancelAtom() : cancelled_(false), heard_(false) {}

  /**
   * @brief 检查操作是否已被请求取消。
   * 如果已被取消，此方法还会将 "heard_" 标志设置为 true，表示取消请求已被注意到。
   * 此方法是线程安全的。
   * @return 如果操作已被取消，则返回 true；否则返回 false。
   */
  bool IsCancelled() {
    QMutexLocker locker(&mutex_);  // 获取互斥锁，确保独占访问
    if (cancelled_) {              // 如果已被取消
      heard_ = true;               // 将 "heard_" 标志设置为 true
    }
    return cancelled_;  // 返回取消状态
  }  // 锁在此处自动释放 (QMutexLocker 析构)

  /**
   * @brief 请求取消当前操作。
   * 此方法是线程安全的。
   */
  void Cancel() {
    QMutexLocker locker(&mutex_);  // 获取互斥锁
    cancelled_ = true;             // 将取消标志设置为 true
  }  // 锁在此处自动释放

  /**
   * @brief 检查取消请求是否已被“听到” (即 IsCancelled() 是否在 cancelled_ 为 true 后被调用过)。
   * 此方法是线程安全的。
   * @return 如果取消请求已被注意到，则返回 true；否则返回 false。
   */
  bool HeardCancel() {
    QMutexLocker locker(&mutex_);  // 获取互斥锁
    return heard_;                 // 返回 "heard_" 状态
  }  // 锁在此处自动释放

 private:
  QMutex mutex_;  // 互斥锁，用于保护成员变量的线程安全访问

  bool cancelled_;  // 标记操作是否已被请求取消
  bool heard_;      // 标记取消请求是否已被 IsCancelled() 方法注意到
};

}  // namespace olive

#endif  // CANCELATOM_H