#ifndef JOBTIME_H
#define JOBTIME_H

#include <stdint.h>   // 为了 uint64_t
#include <QDebug>     // 为了 QDebug 流操作符重载
#include <QMetaType>  // 为了 Q_DECLARE_METATYPE

namespace olive {

/**
 * @brief 表示一个作业时间或唯一作业标识符的类。
 *
 * 此类封装一个 `uint64_t` 值，用于表示某个作业（例如渲染任务、缓存任务）
 * 的“时间”或一个单调递增的唯一ID。它主要用于比较不同作业
 * 的新旧或顺序。
 */
class JobTime {
 public:
  /**
   * @brief 默认构造函数。
   * 初始化 JobTime 的内部值为0。
   */
  JobTime();

  /**
   * @brief 获取（或更新）当前的作业时间值。
   * 此方法通常会从一个全局的、单调递增的计数器获取新的时间值，
   * 并赋给内部的 value_ 成员。
   */
  void Acquire();

  /**
   * @brief 获取内部存储的作业时间值。
   * @return uint64_t 作业时间值。
   */
  [[nodiscard]] uint64_t value() const { return value_; }

  /**
   * @brief 比较两个 JobTime 对象是否相等。
   * @param rhs 要比较的另一个 JobTime 对象。
   * @return bool 如果它们的内部值相等，则返回 true；否则返回 false。
   */
  bool operator==(const JobTime &rhs) const { return value_ == rhs.value_; }

  /**
   * @brief 比较两个 JobTime 对象是否不相等。
   * @param rhs 要比较的另一个 JobTime 对象。
   * @return bool 如果它们的内部值不相等，则返回 true；否则返回 false。
   */
  bool operator!=(const JobTime &rhs) const { return value_ != rhs.value_; }

  /**
   * @brief 比较此 JobTime 对象是否小于另一个 JobTime 对象。
   * @param rhs 要比较的另一个 JobTime 对象。
   * @return bool 如果此对象的内部值小于另一个对象的内部值，则返回 true；否则返回 false。
   */
  bool operator<(const JobTime &rhs) const { return value_ < rhs.value_; }

  /**
   * @brief 比较此 JobTime 对象是否大于另一个 JobTime 对象。
   * @param rhs 要比较的另一个 JobTime 对象。
   * @return bool 如果此对象的内部值大于另一个对象的内部值，则返回 true；否则返回 false。
   */
  bool operator>(const JobTime &rhs) const { return value_ > rhs.value_; }

  /**
   * @brief 比较此 JobTime 对象是否小于或等于另一个 JobTime 对象。
   * @param rhs 要比较的另一个 JobTime 对象。
   * @return bool 如果此对象的内部值小于或等于另一个对象的内部值，则返回 true；否则返回 false。
   */
  bool operator<=(const JobTime &rhs) const { return value_ <= rhs.value_; }

  /**
   * @brief 比较此 JobTime 对象是否大于或等于另一个 JobTime 对象。
   * @param rhs 要比较的另一个 JobTime 对象。
   * @return bool 如果此对象的内部值大于或等于另一个对象的内部值，则返回 true；否则返回 false。
   */
  bool operator>=(const JobTime &rhs) const { return value_ >= rhs.value_; }

 private:
  /**
   * @brief 存储实际作业时间或ID的64位无符号整数值。
   * 使用 `{}` 进行值初始化，确保在构造时为0。
   */
  uint64_t value_{};
};

}  // namespace olive

/**
 * @brief 为 olive::JobTime 类型重载 QDebug 流输出操作符。
 * 允许直接将 JobTime 对象输出到 QDebug 流，通常会输出其内部的 value_ 值。
 * @param debug QDebug 流对象。
 * @param jt 要输出的 JobTime 对象。
 * @return QDebug& 对 QDebug 流对象的引用，以便链式操作。
 */
QDebug operator<<(QDebug debug, const olive::JobTime &jt);  // 通常在 .cpp 文件中实现

/**
 * @brief 将 olive::JobTime 类型注册到 Qt 元对象系统。
 * 使得此类型可以在 QVariant 中使用，以及用于信号槽连接等。
 */
Q_DECLARE_METATYPE(olive::JobTime)

#endif  // JOBTIME_H