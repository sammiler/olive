#ifndef RENDERCACHE_H // 防止头文件被重复包含的宏
#define RENDERCACHE_H // 定义 RENDERCACHE_H 宏

#include "codec/decoder.h" // 包含解码器 (Decoder) 相关的定义，特别是 DecoderPtr 和 Decoder::CodecStream

// 假设 QHash 和 QMutex 已通过标准方式被包含。
// QVariant 也可能需要，因为它用在 ShaderCache 中。

namespace olive { // olive 项目的命名空间

/**
 * @brief RenderCache 类是一个模板类，它继承自 QHash<K, V> 并为其添加了一个互斥锁 (QMutex)。
 *
 * 这个类的目的是提供一个线程安全的哈希表，用于在渲染相关的上下文中缓存各种类型的对象。
 * 互斥锁 (`mutex_`) 用于保护对哈希表的并发访问，确保在多线程环境中读写缓存的安全性。
 *
 * @tparam K 缓存中键 (Key) 的类型。
 * @tparam V 缓存中值 (Value) 的类型。
 */
template <typename K, typename V>
class RenderCache : public QHash<K, V> { // RenderCache 继承自 QHash<K, V>
public:
  /**
   * @brief 获取用于保护此缓存访问的互斥锁。
   * 外部代码在访问此缓存的内容之前，应该先获取并锁定这个互斥锁。
   * @return 返回指向 QMutex 对象的指针。
   */
  QMutex *mutex() { return &mutex_; }

private:
  QMutex mutex_; // 互斥锁，用于同步对 QHash 成员的访问
};

/**
 * @brief DecoderPair 结构体用于在解码器缓存 (DecoderCache) 中存储解码器实例及其相关信息。
 */
struct DecoderPair {
  DecoderPtr decoder = nullptr; // 指向解码器实例的共享指针 (DecoderPtr)
  qint64 last_modified = 0;   // 文件或流的最后修改时间戳，用于判断解码器是否需要更新
};

// 类型别名：DecoderCache 是一个 RenderCache 的特化实例，
// 用于缓存解码器对象。
// 键 (Decoder::CodecStream) 通常标识了一个特定的媒体流 (例如，文件名和流索引)。
// 值 (DecoderPair) 存储了对应的解码器实例和最后修改时间。
using DecoderCache = RenderCache<Decoder::CodecStream, DecoderPair>;

// 类型别名：ShaderCache 是一个 RenderCache 的特化实例，
// 用于缓存已编译的着色器程序。
// 键 (QString) 通常是着色器的唯一ID或源代码的哈希。
// 值 (QVariant) 存储了原生着色器句柄 (例如，OpenGL中的GLuint programID)。
using ShaderCache = RenderCache<QString, QVariant>;

}  // namespace olive

#endif  // RENDERCACHE_H