#ifndef LIBOLIVECORE_TIMERANGE_H
#define LIBOLIVECORE_TIMERANGE_H

#include <list>       // 引入 std::list (虽然在此文件中 TimeRange::Split 返回它，但 TimeRangeList 使用 std::vector)
#include <vector>     // 引入 std::vector，用作 TimeRangeList 内部的存储容器
#include <initializer_list> // 引入 std::initializer_list，用于 TimeRangeList 的构造
#include <algorithm>  // 引入 STL 算法，例如用于 TimeRangeList::util_remove

#include "rational.h" // 引入 rational 类，用于精确表示时间点和长度

namespace olive::core { // Olive 核心功能命名空间

/**
 * @brief 表示一个时间范围，由入点 (in) 和出点 (out) 定义。
 *
 * TimeRange 类用于精确地表示一段时间区间。它内部使用 rational 类型来存储
 * 时间点，以支持非整数帧速率和精确的时间计算。
 * 长度 (length) 是根据入点和出点计算得出的。
 * 此类提供了比较、合并、相交、偏移等操作。
 */
class TimeRange {
 public:
  /**
   * @brief 默认构造函数。
   *
   * 创建一个空的或无效的时间范围 (通常入点、出点和长度都为0或未定义)。
   */
  TimeRange() = default;

  /**
   * @brief 构造函数，使用指定的入点和出点创建时间范围。
   * @param in 时间范围的入点 (rational 类型)。
   * @param out 时间范围的出点 (rational 类型)。
   * 创建后会调用 normalize() 来确保 in <= out 并计算长度。
   */
  TimeRange(const rational& in, const rational& out);

  /**
   * @brief 拷贝构造函数。
   * @param r 要拷贝的另一个 TimeRange 对象。
   */
  TimeRange(const TimeRange& r) : TimeRange(r.in(), r.out()) {} // 委托给 (in, out) 构造函数

  /**
   * @brief 赋值运算符。
   * @param r 要赋给当前对象的另一个 TimeRange 对象。
   * @return 返回对当前对象的引用。
   */
  TimeRange& operator=(const TimeRange& r) {
    set_range(r.in(), r.out()); // 使用 set_range 来确保 normalize 被调用
    return *this;
  }

  /**
   * @brief 获取时间范围的入点。
   * @return 返回入点时间的常量引用 (rational 类型)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const rational& in() const;

  /**
   * @brief 获取时间范围的出点。
   * @return 返回出点时间的常量引用 (rational 类型)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const rational& out() const;

  /**
   * @brief 获取时间范围的长度 (out - in)。
   * @return 返回长度的常量引用 (rational 类型)。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const rational& length() const;

  /**
   * @brief 设置时间范围的入点。
   * 设置后会调用 normalize()。
   * @param in 新的入点时间。
   */
  void set_in(const rational& in);

  /**
   * @brief 设置时间范围的出点。
   * 设置后会调用 normalize()。
   * @param out 新的出点时间。
   */
  void set_out(const rational& out);

  /**
   * @brief 同时设置时间范围的入点和出点。
   * 设置后会调用 normalize()。
   * @param in 新的入点时间。
   * @param out 新的出点时间。
   */
  void set_range(const rational& in, const rational& out);

  /**
   * @brief 等于运算符。
   * @param r 要比较的另一个 TimeRange 对象。
   * @return 如果两个时间范围的入点和出点都相同，则返回 true。
   */
  bool operator==(const TimeRange& r) const;

  /**
   * @brief 不等于运算符。
   * @param r 要比较的另一个 TimeRange 对象。
   * @return 如果两个时间范围的入点或出点不同，则返回 true。
   */
  bool operator!=(const TimeRange& r) const;

  /**
   * @brief 检查当前时间范围是否与另一个时间范围重叠。
   * @param a 要比较的另一个 TimeRange 对象。
   * @param in_inclusive 是否包含入点进行比较 (默认为 true)。
   * @param out_inclusive 是否包含出点进行比较 (默认为 true)。
   * @return 如果两个时间范围有重叠部分，则返回 true。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool OverlapsWith(const TimeRange& a, bool in_inclusive = true, bool out_inclusive = true) const;

  /**
   * @brief 检查当前时间范围是否完全包含另一个时间范围。
   * @param compare 要比较的另一个 TimeRange 对象。
   * @param in_inclusive 比较入点时是否包含边界 (默认为 true)。
   * @param out_inclusive 比较出点时是否包含边界 (默认为 true)。
   * @return 如果当前时间范围完全包含 compare 时间范围，则返回 true。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool Contains(const TimeRange& compare, bool in_inclusive = true, bool out_inclusive = true) const;

  /**
   * @brief 检查当前时间范围是否包含一个特定的时间点。
   * 默认情况下，包含入点，不包含出点 (即 [in, out) )。
   * @param r 要检查的时间点 (rational 类型)。
   * @return 如果时间点在当前时间范围内，则返回 true。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool Contains(const rational& r) const;

  /**
   * @brief 返回当前时间范围与另一个时间范围合并后的新时间范围。
   * 合并结果是包含这两个范围的最小连续范围。
   * @param a 要合并的另一个 TimeRange 对象。
   * @return 返回一个新的 TimeRange 对象，表示合并后的范围。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] TimeRange Combined(const TimeRange& a) const;

  /**
   * @brief 静态方法，合并两个时间范围。
   * @param a 第一个时间范围。
   * @param b 第二个时间范围。
   * @return 返回一个新的 TimeRange 对象，表示合并后的范围。
   */
  static TimeRange Combine(const TimeRange& a, const TimeRange& b);

  /**
   * @brief 返回当前时间范围与另一个时间范围相交部分的新时间范围。
   * @param a 要相交的另一个 TimeRange 对象。
   * @return 返回一个新的 TimeRange 对象，表示相交的范围；如果没有交集，则可能返回一个无效或零长度的范围。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] TimeRange Intersected(const TimeRange& a) const;

  /**
   * @brief 静态方法，计算两个时间范围的交集。
   * @param a 第一个时间范围。
   * @param b 第二个时间范围。
   * @return 返回一个新的 TimeRange 对象，表示相交的范围。
   */
  static TimeRange Intersect(const TimeRange& a, const TimeRange& b);

  /**
   * @brief 将时间范围整体向右平移指定的量。
   * @param rhs 要平移的量 (rational 类型)。
   * @return 返回一个新的 TimeRange 对象，表示平移后的范围。
   */
  TimeRange operator+(const rational& rhs) const;

  /**
   * @brief 将时间范围整体向左平移指定的量。
   * @param rhs 要平移的量 (rational 类型)。
   * @return 返回一个新的 TimeRange 对象，表示平移后的范围。
   */
  TimeRange operator-(const rational& rhs) const;

  /**
   * @brief 将时间范围整体向右平移指定的量 (原地修改)。
   * @param rhs 要平移的量。
   * @return 返回对当前对象的引用。
   */
  const TimeRange& operator+=(const rational& rhs);

  /**
   * @brief 将时间范围整体向左平移指定的量 (原地修改)。
   * @param rhs 要平移的量。
   * @return 返回对当前对象的引用。
   */
  const TimeRange& operator-=(const rational& rhs);

  /**
   * @brief 将当前时间范围分割成多个指定长度的小块。
   * @param chunk_size 每个小块的期望长度 (int 类型，单位可能取决于上下文，例如帧数)。
   * @return 返回一个 std::list<TimeRange>，包含分割后的小时间范围块。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] std::list<TimeRange> Split(const int& chunk_size) const;

 private:
  /**
   * @brief 规范化时间范围。
   *
   * 确保 in_ <= out_，并根据 in_ 和 out_ 更新 length_。
   * 通常在构造函数和修改入点/出点的 setter 中调用。
   */
  void normalize();

  rational in_;     ///< 时间范围的入点。
  rational out_;    ///< 时间范围的出点。
  rational length_; ///< 时间范围的长度 (out_ - in_)。
};

/**
 * @brief 管理一个 TimeRange 对象列表的类。
 *
 * TimeRangeList 用于存储和操作一组不一定连续或有序的时间范围。
 * 它提供了插入、移除、合并、检查包含/重叠等操作。
 */
class TimeRangeList {
 public:
  /**
   * @brief 默认构造函数。
   */
  TimeRangeList() = default;

  /**
   * @brief 使用初始化列表构造 TimeRangeList。
   * @param r 一个 std::initializer_list<TimeRange>，用于初始化内部的 TimeRange 向量。
   */
  TimeRangeList(std::initializer_list<TimeRange> r) : array_(r) {}

  /**
   * @brief 向当前列表中插入另一个 TimeRangeList 中的所有时间范围。
   * 插入后通常会进行合并重叠范围的操作。
   * @param list_to_add 要插入的 TimeRangeList。
   */
  void insert(const TimeRangeList& list_to_add);

  /**
   * @brief 向当前列表中插入一个单独的时间范围。
   * 插入后通常会进行合并重叠范围的操作。
   * @param range_to_add 要插入的 TimeRange。
   */
  void insert(TimeRange range_to_add);

  /**
   * @brief 从当前列表中移除一个指定的时间范围。
   *
   * 此操作可能会导致现有范围被裁切或分割。
   * @param remove 要移除的 TimeRange。
   */
  void remove(const TimeRange& remove);

  /**
   * @brief 从当前列表中移除另一个 TimeRangeList 中包含的所有时间范围。
   * @param list 包含要移除的多个 TimeRange 的 TimeRangeList。
   */
  void remove(const TimeRangeList& list);

  /**
   * @brief 静态工具模板函数，从一个 std::vector<T> (其中 T 类似于 TimeRange) 中移除一个时间范围。
   *
   * 此函数处理多种情况：
   * 1. 如果列表中的某个元素完全被 `remove` 范围包含，则移除该元素。
   * 2. 如果 `remove` 范围完全包含在列表中的某个元素内，则将该元素分割成两部分。
   * 3. 如果列表中的某个元素的出点与 `remove` 范围的入点重叠，则裁切该元素的出点。
   * 4. 如果列表中的某个元素的入点与 `remove` 范围的出点重叠，则裁切该元素的入点。
   * @tparam T 列表中元素的类型，应具有类似 TimeRange 的接口 (Contains, in, out, set_in, set_out)。
   * @param list 指向要修改的 std::vector<T> 的指针。
   * @param remove 要移除的时间范围。
   */
  template <typename T>
  static void util_remove(std::vector<T>* list, const TimeRange& remove) {
    std::vector<T> additions; // 存储因分割而产生的新范围

    for (auto it = list->begin(); it != list->end();) { // 遍历列表
      T& compare = *it; // 当前比较的范围

      if (remove.Contains(compare)) { // 情况1: 当前范围完全被移除范围包含
        it = list->erase(it); // 移除当前范围，迭代器指向下一个元素
      } else {
        if (compare.Contains(remove, false, false)) { // 情况2: 移除范围在当前范围内 (不含边界)
          // 移除范围在当前元素内部，需要将当前元素分割成两部分
          T new_range = compare;          // 创建新范围作为后半部分
          new_range.set_in(remove.out()); // 新范围的入点是移除范围的出点
          compare.set_out(remove.in());   // 当前范围（前半部分）的出点是移除范围的入点

          additions.push_back(new_range); // 将新创建的后半部分添加到待添加列表
          break; // 假设一个移除范围最多只分割一个现有范围
        } else {
          // 情况3和4: 部分重叠
          if (compare.in() < remove.in() && compare.out() > remove.in()) {
            // 当前范围的出点与移除范围的入点重叠，裁切当前范围的出点
            compare.set_out(remove.in());
          } else if (compare.in() < remove.out() && compare.out() > remove.out()) {
            // 当前范围的入点与移除范围的出点重叠，裁切当前范围的入点
            compare.set_in(remove.out());
          }
          it++; // 继续下一个元素
        }
      }
    }
    // 将因分割而产生的新范围添加到列表末尾
    // 注意：这之后可能需要对列表进行排序和合并重叠的操作，但此函数本身不执行
    list->insert(list->end(), additions.begin(), additions.end());
  }

  /**
   * @brief 检查此列表是否完全包含给定的时间范围。
   * @param range 要检查的时间范围。
   * @param in_inclusive 比较入点时是否包含边界 (默认为 true)。
   * @param out_inclusive 比较出点时是否包含边界 (默认为 true)。
   * @return 如果列表中的范围组合起来完全覆盖了给定的 range，则返回 true。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool contains(const TimeRange& range, bool in_inclusive = true, bool out_inclusive = true) const;

  /**
   * @brief 检查此列表是否包含给定的时间点。
   * @param r 要检查的时间点 (rational 类型)。
   * @return 如果列表中有任何 TimeRange 包含该时间点，则返回 true。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool contains(const rational& r) const {
    for (const TimeRange& range : array_) { // 遍历列表中的每个范围
      if (range.Contains(r)) { // 如果当前范围包含该时间点
        return true;
      }
    }
    return false; // 列表中没有范围包含该时间点
  }

  /**
   * @brief 检查此列表中的任何时间范围是否与给定的时间范围重叠。
   * @param r 要检查的时间范围。
   * @param in_inclusive 比较入点时是否包含边界 (默认为 true)。
   * @param out_inclusive 比较出点时是否包含边界 (默认为 true)。
   * @return 如果列表中至少有一个 TimeRange 与 r 重叠，则返回 true。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool OverlapsWith(const TimeRange& r, bool in_inclusive = true, bool out_inclusive = true) const {
    for (const TimeRange& range : array_) { // 遍历列表中的每个范围
      if (range.OverlapsWith(r, in_inclusive, out_inclusive)) { // 如果当前范围与r重叠
        return true;
      }
    }
    return false; // 列表中没有范围与r重叠
  }

  /**
   * @brief 检查列表是否为空。
   * @return 如果列表不包含任何 TimeRange，则返回 true。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool isEmpty() const { return array_.empty(); }

  /**
   * @brief 清空列表中的所有时间范围。
   */
  void clear() { array_.clear(); }

  /**
   * @brief 获取列表中的时间范围数量。
   * @return 返回列表中 TimeRange 对象的数量。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int size() const { return array_.size(); }

  /**
   * @brief 将列表中的所有时间范围整体平移指定的量。
   * @param diff 平移的量 (rational 类型)。正值向右平移，负值向左平移。
   */
  void shift(const rational& diff);

  /**
   * @brief 从列表中的每个时间范围的入点裁切掉指定的长度。
   * @param diff 要裁切掉的长度。
   */
  void trim_in(const rational& diff);

  /**
   * @brief 从列表中的每个时间范围的出点裁切掉指定的长度。
   * @param diff 要裁切掉的长度。
   */
  void trim_out(const rational& diff);

  /**
   * @brief 返回一个新的 TimeRangeList，其中包含当前列表中与给定范围相交的部分。
   * @param range 用于相交的参考时间范围。
   * @return 返回包含所有交集结果的 TimeRangeList。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] TimeRangeList Intersects(const TimeRange& range) const;

  /// @brief 定义常量迭代器类型。
  using const_iterator = std::vector<TimeRange>::const_iterator;

  /** @brief 返回指向列表第一个元素的常量迭代器。 [[nodiscard]] */
  [[nodiscard]] const_iterator begin() const { return array_.cbegin(); }
  /** @brief 返回指向列表末尾之后位置的常量迭代器。 [[nodiscard]] */
  [[nodiscard]] const_iterator end() const { return array_.cend(); }
  /** @brief 返回指向列表第一个元素的常量迭代器 (同 begin())。 [[nodiscard]] */
  [[nodiscard]] const_iterator cbegin() const { return begin(); }
  /** @brief 返回指向列表末尾之后位置的常量迭代器 (同 end())。 [[nodiscard]] */
  [[nodiscard]] const_iterator cend() const { return end(); }

  /** @brief 获取列表中的第一个时间范围。调用前应确保列表不为空。 @return 第一个 TimeRange 的常量引用。 [[nodiscard]] */
  [[nodiscard]] const TimeRange& first() const { return array_.front(); }
  /** @brief 获取列表中的最后一个时间范围。调用前应确保列表不为空。 @return 最后一个 TimeRange 的常量引用。 [[nodiscard]] */
  [[nodiscard]] const TimeRange& last() const { return array_.back(); }
  /** @brief 获取列表中指定索引处的时间范围。 @param index 索引。 @return 指定索引处 TimeRange 的常量引用。 [[nodiscard]] */
  [[nodiscard]] const TimeRange& at(int index) const { return array_.at(index); }

  /**
   * @brief 获取内部存储时间范围的 std::vector 的常量引用。
   * @return 返回内部 std::vector<TimeRange> 的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const std::vector<TimeRange>& internal_array() const { return array_; }

  /**
   * @brief 等于运算符，比较两个 TimeRangeList 是否相同。
   * @param rhs 要比较的另一个 TimeRangeList。
   * @return 如果两个列表内部的 TimeRange 向量完全相同，则返回 true。
   */
  bool operator==(const TimeRangeList& rhs) const { return array_ == rhs.array_; }

 private:
  std::vector<TimeRange> array_; ///< 内部使用 std::vector 存储 TimeRange 对象。
};

/**
 * @brief 用于按帧遍历 TimeRangeList 的迭代器。
 *
 * TimeRangeListFrameIterator 允许根据给定的时间基准，
 * 逐帧地迭代一个 TimeRangeList 中包含的所有时间点。
 * 它能够处理不连续的时间范围，并正确地跳过空白区域。
 */
class TimeRangeListFrameIterator {
 public:
  /**
   * @brief 默认构造函数。
   * 创建一个无效或空的迭代器。
   */
  TimeRangeListFrameIterator();

  /**
   * @brief 构造函数。
   * @param list 要迭代的 TimeRangeList。
   * @param timebase 帧的时间基准 (例如，1/24 表示24fps)。
   */
  TimeRangeListFrameIterator(TimeRangeList list, const rational& timebase);

  /**
   * @brief 将给定的时间点吸附（对齐）到迭代器的时间基准。
   * @param r 要吸附的时间点 (rational 类型)。
   * @return 返回吸附到时间基准后的时间点。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] rational Snap(const rational& r) const;

  /**
   * @brief 获取迭代序列中的下一个帧时间点。
   * @param out 指向 rational 对象的指针，用于存储获取到的下一个帧时间点。
   * @return 如果成功获取到下一个帧，则返回 true；如果已到达序列末尾，则返回 false。
   */
  bool GetNext(rational* out);

  /**
   * @brief 检查是否还有更多帧可以迭代。
   * @return 如果存在下一个帧，则返回 true；否则返回 false。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool HasNext() const;

  /**
   * @brief 将迭代器将要产生的所有帧时间点收集到一个向量中。
   *
   * 注意：此方法会创建一个迭代器的副本并完整迭代一次，可能会有性能开销。
   * @return 返回一个包含所有帧时间点的 std::vector<rational>。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] std::vector<rational> ToVector() const {
    TimeRangeListFrameIterator copy(list_, timebase_); // 创建迭代器副本
    std::vector<rational> times;
    rational r;
    while (copy.GetNext(&r)) { // 迭代副本
      times.push_back(r);     // 将每个帧时间添加到向量
    }
    return times;
  }

  /**
   * @brief 获取迭代器将产生的总帧数。
   *
   * 注意：此方法可能需要完整遍历一次（或预计算）才能得到准确值。
   * @return 返回总帧数。
   */
  int size();

  /**
   * @brief 重置迭代器到初始状态。
   *
   * 之后调用 GetNext() 将从第一个帧开始。
   */
  void reset() { *this = TimeRangeListFrameIterator(); } // 重新赋一个默认构造的迭代器

  /**
   * @brief 向迭代器内部的 TimeRangeList 中插入一个新的时间范围。
   *
   * 注意：在迭代过程中修改列表可能会导致未定义行为，除非迭代器设计为能处理此情况。
   * @param range 要插入的 TimeRange。
   */
  void insert(const TimeRange& range) { list_.insert(range); }

  /**
   * @brief 向迭代器内部的 TimeRangeList 中插入另一个列表的所有时间范围。
   * @param list 要插入的 TimeRangeList。
   */
  void insert(const TimeRangeList& list) { list_.insert(list); }

  /**
   * @brief 检查当前迭代是否基于自定义范围（可能与原始列表不同）。
   * @return 如果是自定义范围，则返回 true。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool IsCustomRange() const { return custom_range_; }

  /**
   * @brief 设置是否使用自定义范围进行迭代。
   * @param e true 表示使用自定义范围。
   */
  void SetCustomRange(bool e) { custom_range_ = e; }

  /**
   * @brief 获取当前迭代到的帧的索引（相对于整个迭代序列）。
   * @return 返回当前帧的索引。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int frame_index() const { return frame_index_; }

 private:
  /**
   * @brief 内部辅助函数，在需要时更新迭代器的内部索引以指向下一个有效范围。
   */
  void UpdateIndexIfNecessary();

  TimeRangeList list_; ///< 迭代器所基于的时间范围列表。

  rational timebase_; ///< 帧的时间基准。

  rational current_; ///< 当前迭代到的时间点。

  int range_index_; ///< 当前正在处理的 list_ 中 TimeRange 的索引。
  int size_;        ///< 缓存的总帧数。
  int frame_index_; ///< 当前已迭代的帧的计数器。

  bool custom_range_; ///< 标记是否使用了自定义范围进行迭代。
};

}  // namespace olive::core

#endif  // LIBOLIVECORE_TIMERANGE_H
