

#ifndef THREADSAFEMAP_H
#define THREADSAFEMAP_H

#include <QMap>   // Qt 的映射容器
#include <QMutex> // Qt 的互斥锁，用于线程同步
#include <QMutexLocker> // (可选) 为了更安全的锁管理，但当前代码直接使用 lock/unlock

// namespace olive { // 如果此类应属于特定命名空间

/**
 * @brief 一个简单的线程安全 QMap 封装模板类。
 *
 * 此类通过使用 QMutex 来为 QMap 的操作提供线程安全性。
 * 它旨在允许多个线程并发地访问和修改映射，而不会产生数据竞争。
 * 注意：当前实现仅封装了 `insert` 操作。要完全线程安全地使用 QMap，
 * 所有其他访问或修改 QMap 的方法 (如 `value`, `remove`, `contains`, `clear`,
 * 迭代器访问等) 也需要通过互斥锁进行保护。
 *
 * @tparam K QMap 的键 (Key) 类型。
 * @tparam V QMap 的值 (Value) 类型。
 */
template <typename K, typename V>
class ThreadSafeMap {
 public:
  /**
   * @brief 默认构造函数。
   * 初始化一个空的线程安全映射。
   */
  ThreadSafeMap() = default;

  // 默认析构函数即可，QMutex 和 QMap 会自动销毁。
  // 如果需要自定义清理（例如，如果 V 是指针且需要手动删除），则需要显式析构函数。
  // virtual ~ThreadSafeMap() = default; // 如果需要虚析构（例如作为基类时）

  // 通常，线程安全容器会禁用拷贝和移动构造/赋值，以避免复杂的同步问题或意外行为。
  // 如果需要支持这些操作，必须非常小心地处理互斥锁的状态。
  ThreadSafeMap(const ThreadSafeMap&) = delete;
  ThreadSafeMap& operator=(const ThreadSafeMap&) = delete;
  ThreadSafeMap(ThreadSafeMap&&) = delete; // 移动构造也可能复杂
  ThreadSafeMap& operator=(ThreadSafeMap&&) = delete; // 移动赋值


  /**
   * @brief 向映射中插入一个键值对。
   * 此操作是线程安全的。
   * @param key 要插入的键。
   * @param value 与键关联的值。
   */
  void insert(K key, V value) {
    QMutexLocker locker(&mutex_); // 使用 QMutexLocker 可以在作用域结束时自动解锁
    map_.insert(key, value);
    // mutex_.lock(); // 旧的手动加锁方式
    // map_.insert(key, value);
    // mutex_.unlock(); // 旧的手动解锁方式
  }

  // TODO: 为 QMap 的其他常用方法 (如 value(), remove(), contains(), isEmpty(), size(), clear() 等)
  //       添加线程安全的封装。例如：
  //
  // /**
  //  * @brief 获取与指定键关联的值。
  //  * 此操作是线程安全的。
  //  * @param key 要查找的键。
  //  * @param defaultValue 如果键不存在，则返回此默认值。
  //  * @return V 与键关联的值；如果键不存在，则返回 defaultValue。
  //  */
  // V value(const K &key, const V &defaultValue = V()) const {
  //   QMutexLocker locker(const_cast<QMutex*>(&mutex_)); // const 方法中访问非 const mutex_
  //   return map_.value(key, defaultValue);
  // }
  //
  // /**
  //  * @brief 检查映射是否包含指定的键。
  //  * 此操作是线程安全的。
  //  * @param key 要检查的键。
  //  * @return bool 如果映射包含此键，则返回 true；否则返回 false。
  //  */
  // bool contains(const K &key) const {
  //   QMutexLocker locker(const_cast<QMutex*>(&mutex_));
  //   return map_.contains(key);
  // }
  //
  // // ... 其他方法的封装 ...

 private:
  /**
   * @brief 用于保护对内部 QMap 访问的互斥锁。
   * 可重入的互斥锁 (QMutex::Recursive) 可能在某些复杂场景下需要，但通常非递归锁更推荐。
   * 此处默认为非递归。
   */
  mutable QMutex mutex_; // 标记为 mutable 允许在 const 方法中使用 QMutexLocker

  /**
   * @brief 内部存储键值对的 QMap 对象。
   */
  QMap<K, V> map_;
};

// } // namespace olive (如果适用)

#endif  // THREADSAFEMAP_H