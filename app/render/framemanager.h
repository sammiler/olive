#ifndef FRAMEMANAGER_H // 防止头文件被重复包含的宏
#define FRAMEMANAGER_H // 定义 FRAMEMANAGER_H 宏

#include <QMutex>  // Qt 互斥锁类，用于线程同步
#include <QObject> // Qt 对象模型基类
#include <QTimer>  // Qt 定时器类

// 假设 std::map 和 std::list 已通过标准库头文件被包含。

namespace olive { // olive 项目的命名空间

/**
 * @brief FrameManager 类是一个单例对象，负责管理视频帧数据缓冲区的内存分配和回收。
 *
 * 它实现了一个内存池机制，用于重用已分配的内存块，以减少频繁的内存分配和释放开销，
 * 从而提高性能，尤其是在处理大量视频帧时。
 * 当需要新的帧缓冲区时，可以向 FrameManager 请求；当缓冲区不再使用时，可以将其归还给 FrameManager。
 * FrameManager 会在内部维护一个空闲缓冲区池，并可能定期进行垃圾回收，释放长时间未使用的缓冲区。
 *
 * 所有分配和释放操作都设计为线程安全的。
 */
class FrameManager : public QObject { // FrameManager 继承自 QObject
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  // (静态) 创建 FrameManager 的单例实例
  static void CreateInstance();

  // (静态) 销毁 FrameManager 的单例实例
  static void DestroyInstance();

  // (静态) 获取 FrameManager 的单例实例指针
  static FrameManager* instance();

  /**
   * @brief (静态) 分配指定大小的内存缓冲区。
   * 这是一个便捷的静态方法，内部会调用单例实例的 AllocateFromPool 方法。
   * @param size 要分配的缓冲区大小 (字节)。
   * @return 返回指向分配的内存块的 char* 指针。调用者获得所有权。
   */
  static char* Allocate(int size);

  /**
   * @brief (静态) 释放之前通过 Allocate 分配的内存缓冲区。
   * 这是一个便捷的静态方法，内部会调用单例实例的 DeallocateToPool 方法。
   * @param size 缓冲区的大小 (用于内存池管理)。
   * @param buffer 指向要释放的内存块的指针。
   */
  static void Deallocate(int size, char* buffer);

 private:
  // 私有构造函数 (用于单例模式)
  FrameManager();

  // 私有析构函数 (用于单例模式)
  ~FrameManager() override;

  /**
   * @brief 从内存池中分配缓冲区。
   *
   * 调用者获得缓冲区的所有权，如果需要，可以自行删除它。
   * 也可以通过 DeallocateToPool 将其归还给管理器，以便将来可能被重用。
   *
   * 此方法是线程安全的。
   * @param size 要分配的缓冲区大小 (字节)。
   * @return 返回指向分配的内存块的 char* 指针。
   */
  char* AllocateFromPool(int size);

  /**
   * @brief 将缓冲区归还到内存池。
   *
   * 管理器将获得所有权，并且缓冲区将在一段时间内保持分配状态，以备重用。
   *
   * 此方法是线程安全的。
   * @param size 缓冲区的大小。
   * @param buffer 指向要归还的内存块的指针。
   */
  void DeallocateToPool(int size, char* buffer);

  static FrameManager* instance_; // FrameManager 的静态单例实例指针

  // 定义帧缓冲区在内存池中保持的最长生命周期 (可能是毫秒或其他时间单位)
  // 用于垃圾回收。
  static const int kFrameLifetime;

  // 内部结构体，用于在内存池中存储缓冲区及其相关信息
  struct Buffer {
    qint64 time; // 缓冲区被归还到池中的时间戳 (或最后使用时间)
    char* data;  // 指向实际内存块的指针
  };

  // 内存池，使用 std::map 将缓冲区大小映射到一个 Buffer 列表。
  // 键是缓冲区大小 (int)，值是包含该大小的所有空闲缓冲区的 std::list<Buffer>。
  std::map<int, std::list<Buffer> > pool_;

  QMutex mutex_; // 互斥锁，用于保护对内存池 `pool_` 的线程安全访问

  QTimer clear_timer_; // 定时器，用于定期触发垃圾回收操作

 private slots: // Qt 私有槽函数
  /**
   * @brief 执行垃圾回收的槽函数。
   * 定期检查内存池中是否有超过生命周期的空闲缓冲区，并释放它们。
   */
  void GarbageCollection();
};

}  // namespace olive

#endif  // FRAMEMANAGER_H