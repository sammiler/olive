#ifndef CACHEJOB_H  // 防止头文件被重复包含的宏
#define CACHEJOB_H  // 定义 CACHEJOB_H 宏

#include <QString>   // Qt 字符串类
#include <QVariant>  // Qt 通用数据类型 QVariant (虽然 NodeValue 内部使用，但此处可能间接相关)

#include "node/value.h"                 // 包含 NodeValue 的定义
#include "render/job/acceleratedjob.h"  // 包含 AcceleratedJob 基类的定义

namespace olive {  // olive 项目的命名空间

/**
 * @brief CacheJob 类代表一个与缓存相关的加速任务。
 *
 * 它继承自 AcceleratedJob，因此可以携带执行任务所需的参数值。
 * CacheJob 特有的属性是 `filename_` 和 `fallback_`。
 *
 * - `filename_`: 通常指定了缓存文件的路径或标识符。当执行此任务时，
 *   渲染器可能会尝试从这个文件加载预先计算好的结果。
 * - `fallback_`: 如果缓存文件不存在或加载失败，`fallback_` 指定了一个备用的 NodeValue。
 *   这个备用值可能是另一个需要计算的 Job (例如一个 ShaderJob 来重新生成数据)，
 *   或者是一个可以直接使用的默认值。
 *
 * 这个类用于实现 Olive 的渲染缓存机制，以避免重复计算昂贵的节点操作。
 */
class CacheJob : public AcceleratedJob {  // CacheJob 继承自 AcceleratedJob
 public:
  // 默认构造函数
  CacheJob() = default;

  /**
   * @brief 构造函数，初始化缓存文件名和可选的回退值。
   * @param filename 缓存文件的名称或路径。
   * @param fallback (可选) 如果缓存未命中或无效，则使用的回退 NodeValue。默认为一个空的 NodeValue。
   */
  explicit CacheJob(const QString &filename, const NodeValue &fallback = NodeValue())
      : fallback_(fallback) {  // 修正：确保 fallback_ 被初始化
    filename_ = filename;
    // fallback_ = fallback; // 已在成员初始化列表中完成
  }

  /**
   * @brief 获取缓存文件的名称或路径。
   * @return 返回缓存文件名字符串的常量引用。
   */
  [[nodiscard]] const QString &GetFilename() const { return filename_; }
  /**
   * @brief 设置缓存文件的名称或路径。
   * @param s 新的缓存文件名。
   */
  void SetFilename(const QString &s) { filename_ = s; }

  /**
   * @brief 获取回退值 (NodeValue)。
   * 当缓存查找失败时，将使用此值。
   * @return 返回回退 NodeValue 的常量引用。
   */
  [[nodiscard]] const NodeValue &GetFallback() const { return fallback_; }
  /**
   * @brief 设置回退值。
   * @param val 新的回退 NodeValue。
   */
  void SetFallback(const NodeValue &val) { fallback_ = val; }

 private:
  QString filename_;  // 存储缓存文件的名称或路径

  NodeValue fallback_;  // 存储在缓存未命中时的回退 NodeValue
};

}  // namespace olive

#endif  // CACHEJOB_H