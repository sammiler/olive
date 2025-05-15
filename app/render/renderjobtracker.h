#ifndef RENDERJOBTRACKER_H // 防止头文件被重复包含的宏
#define RENDERJOBTRACKER_H // 定义 RENDERJOBTRACKER_H 宏

#include <olive/core/core.h> // 包含 Olive 核心定义 (可能包含 TimeRange, rational, TimeRangeList)

#include "common/jobtime.h"  // 包含 JobTime 结构体或类的定义 (用于标记作业的时间戳或版本)

// 假设 std::vector 已通过其他方式被间接包含。

namespace olive { // olive 项目的命名空间

using namespace core; // 使用 olive::core 命名空间中的类型 (如 TimeRange, rational)

/**
 * @brief RenderJobTracker 类用于跟踪已提交的渲染作业 (Job) 及其相关的时间信息。
 *
 * 当一个时间范围被渲染后，这个类会记录该范围以及完成该渲染作业时的时间戳 (JobTime)。
 * 这使得系统可以判断某个时间点或时间范围的数据是否“最新”，即它是否由具有最新 JobTime
 * 的作业所生成。如果某个时间点的数据是由一个较旧的 JobTime 生成的，那么它可能已经失效，
 * 需要重新渲染。
 *
 * 这对于管理缓存的有效性非常重要，特别是在节点图或参数发生变化后，
 * 需要确定哪些已缓存的范围需要被重新计算。
 */
class RenderJobTracker {
 public:
  // 默认构造函数
  RenderJobTracker() = default;

  /**
   * @brief 插入一条记录，表示指定的时间范围已由具有特定 JobTime 的作业处理。
   * @param range 已处理的时间范围。
   * @param job_time 完成此范围渲染作业的时间戳/版本。
   */
  void insert(const TimeRange &range, JobTime job_time);
  /**
   * @brief 插入多条记录，表示指定的多个时间范围已由具有特定 JobTime 的作业处理。
   * @param ranges 包含多个已处理时间范围的 TimeRangeList。
   * @param job_time 完成这些范围渲染作业的时间戳/版本。
   */
  void insert(const TimeRangeList &ranges, JobTime job_time);

  /**
   * @brief 清除所有已跟踪的渲染作业记录。
   */
  void clear();

  /**
   * @brief 检查指定时间点的数据是否是由具有给定 JobTime (或更新的) 作业生成的，即是否“最新”。
   * @param time 要检查的时间点。
   * @param job_time 用于比较的 JobTime (通常是当前最新的图更改时间或同步时间)。
   * @return 如果 `time` 对应的数据是由 `job_time` 或更新的作业生成的，则返回 true。
   */
  [[nodiscard]] bool isCurrent(const rational &time, JobTime job_time) const;

  /**
   * @brief 获取在给定 `range` 内，所有由具有最新 `job_time` (或更新的) 作业生成的子时间范围。
   * 这可以用于找出在一个较大范围内哪些部分是“最新”的。
   * @param range 要查询的总体时间范围。
   * @param job_time 用于比较的 JobTime。
   * @return 返回一个 TimeRangeList，包含所有满足条件的“最新”子范围。
   */
  [[nodiscard]] TimeRangeList getCurrentSubRanges(const TimeRange &range, const JobTime &job_time) const;

 private:
  /**
   * @brief TimeRangeWithJob 内部类，继承自 TimeRange，并额外存储了一个 JobTime。
   * 用于将一个时间范围与其对应的渲染作业时间戳关联起来。
   */
  class TimeRangeWithJob : public TimeRange { // 继承自 TimeRange
   public:
    // 默认构造函数
    TimeRangeWithJob() = default;
    /**
     * @brief 构造函数。
     * @param range 时间范围。
     * @param job_time 与此时间范围关联的渲染作业时间戳。
     */
    TimeRangeWithJob(const TimeRange &range, const JobTime &job_time) {
      set_range(range.in(), range.out()); // 设置时间范围的入点和出点
      job_time_ = job_time;              // 存储作业时间戳
    }

    /**
     * @brief 获取与此时间范围关联的渲染作业时间戳。
     */
    [[nodiscard]] JobTime GetJobTime() const { return job_time_; }
    /**
     * @brief 设置与此时间范围关联的渲染作业时间戳。
     */
    void SetJobTime(JobTime jt) { job_time_ = jt; }

   private:
    JobTime job_time_; // 存储渲染作业的时间戳/版本
  };

  // 存储所有已记录的渲染作业及其时间范围的列表
  // 每个元素都是一个 TimeRangeWithJob 对象，包含了时间范围和完成该范围渲染的 JobTime。
  std::vector<TimeRangeWithJob> jobs_;
};

}  // namespace olive

#endif  // RENDERJOBTRACKER_H