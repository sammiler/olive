#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

#include <stdint.h>      // 为了 uint8_t, int64_t, size_t, quintptr
#include <QApplication>  // 为了 qApp (通常是 QApplication::instance())
#include <QDateTime>     // 为了 QDateTime::currentMSecsSinceEpoch()
#include <QDebug>        // 为了 qDebug, qCritical
#include <QMetaObject>   // 为了 QMetaObject::invokeMethod (虽然 QTimer::start() 更直接)
#include <QMutex>        // 为了 QMutex, QMutexLocker
#include <QTimer>        // 为了 QTimer
#include <QVector>       // 为了 QVector
#include <list>          // 为了 std::list
#include <memory>        // 为了 std::shared_ptr, std::make_shared

#include "common/define.h"  // 包含 DISABLE_COPY_MOVE 宏

namespace olive {

/**
 * @brief 内存池基类。
 *
 * 一个自定义内存管理系统，它以大块（称为 arena，竞技场/区域）的形式分配多个对象，
 * 而不是进行多次小型分配。这旨在提高性能并减少内存消耗和碎片。
 *
 * 作为一个基类，可以通过设置模板参数 T 为您选择的对象类型来使用它 (尽管此类本身不是模板类，
 * 它的派生类或使用方式可能涉及模板化)。内存池将为每个 arena 分配
 * `(element_count * sizeof(T))` (或由 GetElementSize() 决定的大小) 的内存。
 * Arenas 会动态分配和销毁——当一个 arena 填满时，会分配另一个。
 *
 * `Get()` 方法将返回一个 ElementPtr (指向 Element 的共享指针)。
 * 原始所需的数据可以通过 `ElementPtr::data()` 访问。此数据将属于调用者，
 * 直到 ElementPtr 超出作用域，此时内存会释放回池中。
 */
class MemoryPool : public QObject {
  Q_OBJECT
 public:
  /**
   * @brief 构造函数。
   * @param element_count 每个 arena 中元素的数量。
   */
  MemoryPool(int element_count) {
    element_count_ = element_count;

    clear_timer_ = new QTimer(this);  // 指定父对象，便于自动清理
    clear_timer_->setInterval(kMaxEmptyArenaLife);
    // clear_timer_->moveToThread(qApp->thread()); // QTimer 通常应在其创建的线程中启动和运行，除非有特殊需求
    connect(clear_timer_, &QTimer::timeout, this,
            &MemoryPool::ClearEmptyArenas);  // Qt::DirectConnection 可能不必要，除非跨线程且有严格要求
    clear_timer_->start();                   // 直接调用 start() 通常更好
  }

  /**
   * @brief 析构函数。
   * 删除所有 arenas，释放它们占用的内存。
   */
  virtual ~MemoryPool() {
    Clear();
    // clear_timer_ 已有父对象，会被自动删除，或者如果希望立即停止和删除:
    // clear_timer_->stop();
    // delete clear_timer_; // 或者 deleteLater() 如果在事件循环中
  }

  /**
   * @brief 禁止 MemoryPool 对象的拷贝和移动操作。
   */
  DISABLE_COPY_MOVE(MemoryPool)

  /**
   * @brief 清除所有 arenas，释放它们占用的所有内存。
   *
   * @warning 此函数不是线程安全的（针对外部元素而言）。调用此函数后，任何仍然被外部持有的
   * ElementPtr 所指向的数据将变为无效，访问它们会导致崩溃。
   * 您需要确保在调用此函数之前，所有已借出的元素都已被归还或不再使用。
   */
  void Clear() {
    QMutexLocker locker(&lock_);  // 保护对 arenas_ 的访问
    qDeleteAll(arenas_);          // qDeleteAll 会删除容器中所有指针指向的对象
    arenas_.clear();
  }

  /**
   * @brief 返回当前是否至少成功分配了一个 arena。
   * @return bool 如果 arenas_ 列表不为空则返回 true，否则返回 false。
   */
  inline bool IsAllocated() const {
    QMutexLocker locker(const_cast<QMutex*>(&lock_));  // 在 const 方法中获取锁需要 const_cast
    return !arenas_.empty();
  }

  /**
   * @brief 返回当前已分配的 arenas 的数量。
   * @return int arenas 的数量。
   */
  inline int GetArenaCount() const {
    QMutexLocker locker(const_cast<QMutex*>(&lock_));
    return arenas_.size();
  }

  class Arena;  // 前向声明嵌套类 Arena

  /**
   * @brief Arena 中一块内存的句柄（代表）。
   *
   * 调用内存池或 Arena 的 `Get()` 方法将返回一个指向 Element 的共享指针 (ElementPtr)。
   * Element 对象包含一个指向所需对象/数据的指针，可通过 `data()` 方法访问。
   * 当 Element 对象被销毁时 (即 ElementPtr 超出作用域)，其占用的内存
   * 将被释放回其所属的 Arena，以便可以被其他地方复用。
   */
  class Element {
   public:
    /**
     * @brief Element 构造函数。
     * 此构造函数通常仅由内存池的内部函数使用。
     * @param parent 指向此元素所属的 Arena 对象的指针。
     * @param data 指向此元素所代表的实际内存块的指针。
     */
    Element(Arena* parent, uint8_t* data) {
      parent_ = parent;
      data_ = data;
      accessed_ = QDateTime::currentMSecsSinceEpoch();
      timestamp_ = 0;  // 初始化时间戳
    }

    /**
     * @brief Element 析构函数。
     * 自动将此元素的内存释放回它所从中获取的 Arena。
     */
    ~Element() { release(); }

    /**
     * @brief 禁止 Element 对象的拷贝和移动操作。
     */
    DISABLE_COPY_MOVE(Element)

    /**
     * @brief 访问此元素在内存池中所代表的数据的指针。
     * @return uint8_t* 指向数据的指针。
     */
    inline uint8_t* data() const { return data_; }

    /**
     * @brief 获取与此元素关联的时间戳。
     * @return const int64_t& 时间戳的常量引用。
     */
    inline const int64_t& timestamp() const { return timestamp_; }

    /**
     * @brief 设置与此元素关联的时间戳。
     * @param timestamp 要设置的时间戳。
     */
    inline void set_timestamp(const int64_t& timestamp) { timestamp_ = timestamp; }

    /**
     * @brief 注册此元素已被访问。
     * 更新最后访问时间戳。 \see last_accessed()
     */
    inline void access() { accessed_ = QDateTime::currentMSecsSinceEpoch(); }

    /**
     * @brief 返回此元素上一次调用 `access()` 的时间。
     *
     * 此信息可用于确定元素的相对“年龄”（即，如果一个元素在一段时间内未被访问，
     * 它可能可以被安全地释放回池中）。这要求所有使用者在访问元素数据后都调用 `access()`。
     * @return const int64_t& 最后访问时间的时间戳 (自 epoch 以来的毫秒数)。
     */
    inline const int64_t& last_accessed() const { return accessed_; }

    /**
     * @brief 将此元素占用的内存明确释放回其父 Arena。
     * 如果 data_ 非空（即内存尚未释放），则调用父 Arena 的 Release 方法，
     * 并将 data_ 设置为 nullptr 以防止重复释放。
     */
    void release() {
      if (data_) {
        parent_->Release(this);
        data_ = nullptr;
      }
    }

   private:
    /**
     * @brief 指向此元素所属的父 Arena 对象的指针。
     */
    Arena* parent_;

    /**
     * @brief 指向此元素所代表的实际内存块的指针。
     */
    uint8_t* data_;

    /**
     * @brief 与此元素关联的时间戳，可由用户自定义其含义。
     */
    int64_t timestamp_;

    /**
     * @brief 此元素最后一次被访问的时间戳 (自 epoch 以来的毫秒数)。
     */
    int64_t accessed_;
  };

  /**
   * @brief 指向 Element 对象的共享指针类型定义。
   * 使用 std::shared_ptr 可以自动管理 Element 对象的生命周期，
   * 当最后一个 ElementPtr 被销毁时，Element 的析构函数会被调用，
   * 从而将其内存归还给 Arena。
   */
  using ElementPtr = std::shared_ptr<Element>;

  /**
   * @brief 内存池中的一个 "arena" —— 即一块预分配的内存区域。
   *
   * 内存池本身不直接存储大量小块内存，而是管理若干个 "arenas"。
   * 这样做是为了处理当一个 arena 的内存被完全分配完毕后，仍有新的内存请求的情况。
   * 内存池可以自动分配新的 arena 来继续提供内存，并在 arena 不再使用时释放它们。
   */
  class Arena {
   public:
    /**
     * @brief Arena 构造函数。
     * @param parent 指向拥有此 Arena 的 MemoryPool 对象的指针。
     */
    Arena(MemoryPool* parent) {
      parent_ = parent;
      data_ = nullptr;
      allocated_sz_ = 0;
      element_sz_ = 0;  // 初始化 element_sz_
      empty_time_ = QDateTime::currentMSecsSinceEpoch();
    }

    /**
     * @brief Arena 析构函数。
     * 释放所有从此 Arena 借出的元素，并释放 Arena 自身分配的内存块。
     */
    ~Arena() {
      // QMutexLocker locker(&lock_); // 析构函数中加锁需谨慎，确保无死锁风险
      // 复制列表以避免在迭代时修改
      std::list<Element*> copy = lent_elements_;
      for (Element* e : copy) {  // 使用范围for循环更现代
        // 通常 Element 的析构函数会调用 release()，但如果 Element 对象本身可能未被正确销毁，
        // 或者为了确保，可以在这里显式调用。但更好的做法是依赖 ElementPtr 的 RAII。
        // 此处假设如果 lent_elements_ 仍有元素，则这些Element对象可能还存在。
        // 如果 Element 对象被 ElementPtr 管理，这里可能不需要手动 release。
        // 但如果 lent_elements_ 存储的是裸指针且不依赖 ElementPtr 销毁，则需要处理。
        // 根据 Element 的析构函数，它会调用 parent_->Release(this)。
        // 所以这里可能只需要清理 Arena 自身的数据。
      }
      lent_elements_.clear();  // 清空列表

      delete[] data_;  // 释放 Arena 的主内存块
      data_ = nullptr;
    }

    /**
     * @brief 禁止 Arena 对象的拷贝和移动操作。
     */
    DISABLE_COPY_MOVE(Arena)

    /**
     * @brief 从此 Arena 获取一个可用的内存元素。
     * @return ElementPtr 指向获取到的 Element 的共享指针；如果 Arena已满，则返回 nullptr。
     */
    ElementPtr Get() {
      QMutexLocker locker(&lock_);

      for (int i = 0; i < available_.size(); i++) {
        if (available_.at(i)) {
          // 此缓冲区可用
          available_.replace(i, false);  // 标记为不可用

          // 计算元素在 data_ 块中的实际地址
          ElementPtr e = std::make_shared<Element>(this, reinterpret_cast<uint8_t*>(data_ + i * element_sz_));
          lent_elements_.push_back(e.get());  // 存储裸指针用于追踪，但生命周期由 ElementPtr 管理

          return e;
        }
      }

      return nullptr;  // 没有可用的元素
    }

    /**
     * @brief 将一个元素释放回此 Arena，使其可供其他地方使用。
     * @param e 指向要释放的 Element 对象的指针。
     */
    void Release(Element* e) {
      QMutexLocker locker(&lock_);
      // 计算元素在 Arena 内存块中的索引
      quintptr diff = reinterpret_cast<quintptr>(e->data()) - reinterpret_cast<quintptr>(data_);
      int index = diff / element_sz_;

      if (index >= 0 && index < available_.size()) {  // 边界检查
        available_.replace(index, true);              // 标记为可用
      } else {
        // 索引无效，可能是一个错误，记录日志
        qWarning() << "MemoryPool::Arena::Release: Invalid element index calculated.";
        return;
      }

      lent_elements_.remove(e);  // 从借出列表中移除

      if (lent_elements_.empty()) {
        empty_time_ = QDateTime::currentMSecsSinceEpoch();  // 更新 Arena 变空的时间
      }
    }

    /**
     * @brief 获取当前从此 Arena 中借出的元素数量。
     * @return int 当前正在使用的元素数量。
     */
    int GetUsageCount() {
      QMutexLocker locker(&lock_);
      return lent_elements_.size();
    }

    /**
     * @brief 分配 Arena 的主内存块。
     * @param ele_sz 每个元素的大小（字节）。
     * @param nb_elements 此 Arena 将包含的元素数量。
     * @return bool 如果内存分配成功则返回 true，否则返回 false。
     */
    bool Allocate(size_t ele_sz, size_t nb_elements) {
      QMutexLocker locker(&lock_);  // 分配操作也应受保护
      if (IsAllocated()) {          // 如果已分配，则直接返回true
        return true;
      }

      element_sz_ = ele_sz;
      allocated_sz_ = element_sz_ * nb_elements;

      if (allocated_sz_ == 0) {  // 避免分配0字节
        qWarning() << "MemoryPool::Arena::Allocate: Attempt to allocate 0 bytes.";
        return false;
      }

      data_ = new (std::nothrow) uint8_t[allocated_sz_];  // 使用 nothrow 避免异常

      if (data_) {
        available_.resize(nb_elements);
        available_.fill(true);                              // 所有元素初始时都可用
        empty_time_ = QDateTime::currentMSecsSinceEpoch();  // 新分配的Arena是空的
        return true;
      } else {
        available_.clear();
        qWarning() << "MemoryPool::Arena::Allocate: Failed to allocate memory for arena.";
        return false;
      }
    }

    /**
     * @brief 获取此 Arena 设计容纳的总元素数量。
     * @return int 元素总数。
     */
    inline int GetElementCount() const {
      // QMutexLocker locker(&lock_); // available_ 的大小在Allocate后固定，读取size()通常线程安全
      return available_.size();
    }

    /**
     * @brief 检查此 Arena 的主内存块是否已分配。
     * @return bool 如果 data_ 非空则返回 true，否则返回 false。
     */
    inline bool IsAllocated() const {
      // QMutexLocker locker(&lock_); // 读取指针通常线程安全，但取决于编译器和架构
      return data_ != nullptr;
    }

    /**
     * @brief 获取此 Arena 最后一次变为空（所有元素都可用）的时间戳。
     * @return qint64 时间戳 (自 epoch 以来的毫秒数)。
     */
    inline qint64 GetTimeArenaWasMadeEmpty() {
      QMutexLocker locker(&lock_);
      return empty_time_;
    }

   private:
    /**
     * @brief 指向拥有此 Arena 的父 MemoryPool 对象的指针。
     */
    MemoryPool* parent_;

    /**
     * @brief 指向 Arena 分配的连续内存块的指针。所有元素都从此块中划分。
     */
    uint8_t* data_;

    /**
     * @brief Arena 分配的内存块的总大小（字节）。
     */
    size_t allocated_sz_;

    /**
     * @brief 标记 Arena 中每个元素槽位是否可用的 QVector<bool>。
     * `true` 表示对应槽位可用，`false` 表示已被借出。
     */
    QVector<bool> available_;

    /**
     * @brief 用于保护 Arena 内部状态（特别是 available_ 和 lent_elements_）的互斥锁。
     */
    QMutex lock_;

    /**
     * @brief Arena 中单个元素的大小（字节）。
     */
    size_t element_sz_;

    /**
     * @brief 存储当前从此 Arena 中借出的所有 Element 对象的裸指针列表。
     * 注意：这些元素的生命周期由外部的 ElementPtr (共享指针) 管理。
     * 此列表主要用于追踪和在 Arena 销毁时进行必要的检查或清理。
     */
    std::list<Element*> lent_elements_;

    /**
     * @brief 此 Arena 最后一次变为空状态的时间戳 (自 epoch 以来的毫秒数)。
     */
    qint64 empty_time_;
  };

  /**
   * @brief 从可用的 Arena 中检索一个内存元素。
   * 如果所有现有 Arenas 都已满，则会尝试创建一个新的 Arena。
   * @return ElementPtr 指向获取到的 Element 的共享指针；如果无法获取或创建元素，则返回 nullptr。
   */
  ElementPtr Get() {
    QMutexLocker locker(&lock_);

    // 尝试从现有 arena 获取元素
    for (Arena* a : arenas_) {  // 使用范围for循环
      ElementPtr e = a->Get();
      if (e) {
        return e;  // 成功获取
      }
    }

    // 所有 arenas 都满了，需要创建一个新的
    if (arenas_.empty()) {
      qDebug() << "内存池：无可用 Arena，正在创建新的...";
    } else {
      qDebug() << "内存池：所有 Arena 已满，正在创建新的...";
    }

    size_t ele_sz = GetElementSize();  // 获取元素大小

    if (!ele_sz) {  // GetElementSize() 可能返回0，需要检查
      qCritical() << "内存池：创建 Arena 失败，元素大小为 0。";
      return nullptr;
    }

    if (element_count_ <= 0) {
      qCritical() << "内存池：创建 Arena 失败，元素数量无效：" << element_count_;
      return nullptr;
    }

    Arena* a = new (std::nothrow) Arena(this);  // 使用 nothrow 避免构造函数抛出异常
    if (!a) {
      qCritical() << "内存池：创建 Arena 对象失败 (内存不足？)。";
      return nullptr;
    }

    if (!a->Allocate(ele_sz, element_count_)) {
      qCritical() << "内存池：Arena 分配内存失败 (内存不足？)。";
      delete a;  // 清理失败创建的 Arena 对象
      return nullptr;
    }

    arenas_.push_back(a);
    return a->Get();  // 从新创建的 Arena 获取元素
  }

 protected:
  /**
   * @brief 获取内存池中每个元素的大小（字节）。
   *
   * 派生类应覆盖此方法以使用自定义大小（例如，当 T 是 char 但元素
   * 大小实际上大于1时，如用于存储固定大小的字符数组）。
   * 默认实现返回 `sizeof(uint8_t)` (1字节)，这可能不适用于所有情况，
   * 除非内存池被特化为管理单字节元素。
   * @warning 默认实现返回 `sizeof(uint8_t)`，派生类通常需要重写此方法以返回其实际管理的元素类型T的大小 `sizeof(T)`
   * 或自定义大小。
   * @return size_t 每个元素的大小（字节）。
   */
  virtual size_t GetElementSize() { return sizeof(uint8_t); }  // 注意：这通常应为 sizeof(T) 或具体类型大小

 private:
  /**
   * @brief 每个 Arena 中元素的数量。
   */
  int element_count_;

  /**
   * @brief 存储所有 Arena 对象的指针的列表。
   */
  std::list<Arena*> arenas_;

  /**
   * @brief 用于保护 MemoryPool 级别操作（如添加/移除 Arena，访问 arenas_ 列表）的互斥锁。
   */
  QMutex lock_;

  /**
   * @brief 用于定期触发 ClearEmptyArenas 槽函数的 QTimer 对象。
   */
  QTimer* clear_timer_;

  /**
   * @brief 定义空的 Arena 在被自动清理前可以存活的最长时间（毫秒）。
   * 5000 毫秒 = 5 秒。
   */
  static const qint64 kMaxEmptyArenaLife = 5000;

 private slots:
  /**
   * @brief 清理所有长时间处于空闲状态（没有借出元素）的 Arenas。
   * 此槽函数由 clear_timer_ 定期触发。
   */
  void ClearEmptyArenas() {
    QMutexLocker locker(&lock_);  // 保护对 arenas_ 列表的修改

    const qint64 min_time = QDateTime::currentMSecsSinceEpoch() - kMaxEmptyArenaLife;

    for (auto it = arenas_.begin(); it != arenas_.end();) {
      Arena* arena = (*it);
      // 检查 Arena 是否为空闲且空闲时间超过了 kMaxEmptyArenaLife
      if (arena->GetUsageCount() == 0 && arena->GetTimeArenaWasMadeEmpty() <= min_time) {
        qDebug() << "内存池：移除一个空闲的 Arena。";
        delete arena;            // 删除 Arena 对象，会触发其析构函数释放内存
        it = arenas_.erase(it);  // 从列表中移除指针，并更新迭代器
      } else {
        ++it;  // 继续下一个
      }
    }
  }
};

}  // namespace olive

#endif  // MEMORYPOOL_H