#ifndef TIMELINEMARKER_H
#define TIMELINEMARKER_H

#include <olive/core/core.h>  // 引入 Olive 核心库的头文件，提供基础数据结构如 TimeRange 和 rational
#include <QPainter>           // 引入 QPainter 类，用于自定义绘制
#include <QString>            // 引入 QString 类，用于字符串操作
#include <QXmlStreamReader>   // 引入 QXmlStreamReader 类，用于读取 XML 数据
#include <QXmlStreamWriter>   // 引入 QXmlStreamWriter 类，用于写入 XML 数据

#include "undo/undocommand.h"  // 引入 UndoCommand 基类，用于实现撤销/重做功能

using namespace olive::core;  // 使用 olive::core 命名空间，方便直接访问其成员，如 TimeRange, rational

namespace olive {

/**
 * @brief 表示时间轴上的一个标记点。
 *
 * TimelineMarker 对象用于在时间轴上标记特定的时间点或时间范围，
 * 可以附带颜色、名称等信息，方便用户进行导航和组织。
 */
class TimelineMarker : public QObject {
  Q_OBJECT
 public:
  /**
   * @brief 显式构造函数。
   * @param parent 父 QObject 对象，默认为 nullptr。
   */
  explicit TimelineMarker(QObject* parent = nullptr);

  /**
   * @brief 构造函数，使用颜色、时间范围和名称初始化标记。
   * @param color 标记的颜色标识 (通常是一个整数，可能映射到具体的颜色值)。
   * @param time 标记所覆盖的时间范围 (TimeRange 类型)。
   * @param name 标记的名称 (QString 类型)，默认为空字符串。
   * @param parent 父 QObject 对象，默认为 nullptr。
   */
  TimelineMarker(int color, const TimeRange& time, QString name = QString(), QObject* parent = nullptr);

  /**
   * @brief 获取标记的时间范围。
   * @return 返回标记的时间范围的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const TimeRange& time() const { return time_; }

  /**
   * @brief 设置标记的时间范围。
   * @param time 新的时间范围。
   */
  void set_time(const TimeRange& time);

  /**
   * @brief 设置标记的时间点（创建一个零长度的时间范围）。
   * @param time 新的时间点 (rational 类型)。
   */
  void set_time(const rational& time);

  /**
   * @brief 检查在指定时间点是否存在同级（兄弟）标记。
   *
   * 这个方法可能用于检查在同一个父级（例如 TimelineMarkerList）中，
   * 是否有其他标记与给定的时间点重合。
   * @param t 要检查的时间点。
   * @return 如果存在同级标记在指定时间点，则返回 true；否则返回 false。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool has_sibling_at_time(const rational& t) const;

  /**
   * @brief 获取标记的名称。
   * @return 返回标记名称的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const QString& name() const { return name_; }

  /**
   * @brief 设置标记的名称。
   * @param name 新的标记名称。
   */
  void set_name(const QString& name);

  /**
   * @brief 获取标记的颜色标识。
   * @return 返回标记的颜色整数值。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] int color() const { return color_; }

  /**
   * @brief 设置标记的颜色标识。
   * @param c 新的颜色整数值。
   */
  void set_color(int c);

  /**
   * @brief 获取标记在时间轴上绘制时的高度。
   *
   * 高度可能依赖于字体信息，以确保文本能够正确显示。
   * @param fm 字体度量对象，用于计算文本尺寸。
   * @return 返回标记的绘制高度（像素）。
   */
  static int GetMarkerHeight(const QFontMetrics& fm);

  /**
   * @brief 绘制标记。
   *
   * @param p QPainter 指针，用于进行绘制操作。
   * @param pt 标记绘制的起始点（左上角）。
   * @param max_right 绘制区域的最大右边界 x 坐标。
   * @param scale 时间轴的缩放比例，用于计算标记在屏幕上的宽度。
   * @param selected 标记是否被选中，选中的标记可能有不同的绘制样式。
   * @return 返回标记绘制区域的 QRect。
   */
  QRect Draw(QPainter* p, const QPoint& pt, int max_right, double scale, bool selected);

  /**
   * @brief 从 XML 流中加载标记数据。
   * @param reader 指向 QXmlStreamReader 对象的指针，用于读取 XML。
   * @return 如果加载成功返回 true，否则返回 false。
   */
  bool load(QXmlStreamReader* reader);

  /**
   * @brief 将标记数据保存到 XML 流中。
   * @param writer 指向 QXmlStreamWriter 对象的指针，用于写入 XML。
   */
  void save(QXmlStreamWriter* writer) const;

 signals:
  /**
   * @brief 当标记的时间范围发生改变时发出的信号。
   * @param time 新的时间范围。
   */
  void TimeChanged(const TimeRange& time);

  /**
   * @brief 当标记的名称发生改变时发出的信号。
   * @param name 新的标记名称。
   */
  void NameChanged(const QString& name);

  /**
   * @brief 当标记的颜色发生改变时发出的信号。
   * @param c 新的颜色整数值。
   */
  void ColorChanged(int c);

 private:
  TimeRange time_;  // 标记的时间范围
  QString name_;    // 标记的名称
  int color_;       // 标记的颜色标识
};

/**
 * @brief 管理 TimelineMarker 对象列表的类。
 *
 * TimelineMarkerList 负责存储、管理和操作一组时间轴标记。
 * 它还处理标记的添加、删除和修改事件，并支持撤销/重做操作。
 */
class TimelineMarkerList : public QObject {
  Q_OBJECT
 public:
  /**
   * @brief 显式构造函数。
   * @param parent 父 QObject 对象，默认为 nullptr。
   */
  explicit TimelineMarkerList(QObject* parent = nullptr) : QObject(parent) {}

  /**
   * @brief 检查标记列表是否为空。
   * @return 如果列表为空则返回 true，否则返回 false。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] inline bool empty() const { return markers_.empty(); }

  /**
   * @brief 返回指向列表第一个元素的迭代器。
   * @return std::vector<TimelineMarker*>::iterator 类型的迭代器。
   */
  inline std::vector<TimelineMarker*>::iterator begin() { return markers_.begin(); }

  /**
   * @brief 返回指向列表末尾之后位置的迭代器。
   * @return std::vector<TimelineMarker*>::iterator 类型的迭代器。
   */
  inline std::vector<TimelineMarker*>::iterator end() { return markers_.end(); }

  /**
   * @brief 返回指向列表第一个元素的常量迭代器。
   * @return std::vector<TimelineMarker*>::const_iterator 类型的常量迭代器。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] inline std::vector<TimelineMarker*>::const_iterator cbegin() const { return markers_.cbegin(); }

  /**
   * @brief 返回指向列表末尾之后位置的常量迭代器。
   * @return std::vector<TimelineMarker*>::const_iterator 类型的常量迭代器。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] inline std::vector<TimelineMarker*>::const_iterator cend() const { return markers_.cend(); }

  /**
   * @brief 返回指向列表第一个元素的常量迭代器 (const 重载版本)。
   * @return std::vector<TimelineMarker*>::const_iterator 类型的常量迭代器。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] inline std::vector<TimelineMarker*>::const_iterator begin() const { return markers_.begin(); }

  /**
   * @brief 返回指向列表末尾之后位置的常量迭代器 (const 重载版本)。
   * @return std::vector<TimelineMarker*>::const_iterator 类型的常量迭代器。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] inline std::vector<TimelineMarker*>::const_iterator end() const { return markers_.end(); }

  /**
   * @brief 返回列表中最后一个标记的指针。
   * 调用此函数前应确保列表不为空。
   * @return 指向最后一个 TimelineMarker 对象的指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] inline TimelineMarker* back() const { return markers_.back(); }

  /**
   * @brief 返回列表中第一个标记的指针。
   * 调用此函数前应确保列表不为空。
   * @return 指向第一个 TimelineMarker 对象的指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] inline TimelineMarker* front() const { return markers_.front(); }

  /**
   * @brief 返回列表中的标记数量。
   * @return 列表中 TimelineMarker 对象的数量。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] inline size_t size() const { return markers_.size(); }

  /**
   * @brief 从 XML 流中加载标记列表数据。
   * @param reader 指向 QXmlStreamReader 对象的指针，用于读取 XML。
   * @return 如果加载成功返回 true，否则返回 false。
   */
  bool load(QXmlStreamReader* reader);

  /**
   * @brief 将标记列表数据保存到 XML 流中。
   * @param writer 指向 QXmlStreamWriter 对象的指针，用于写入 XML。
   */
  void save(QXmlStreamWriter* writer) const;

  /**
   * @brief 获取在指定时间点开始的标记。
   *
   * 遍历列表查找其时间范围入点与给定时间点匹配的第一个标记。
   * @param t 要查找的时间点。
   * @return 如果找到匹配的标记，则返回其指针；否则返回 nullptr。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] TimelineMarker* GetMarkerAtTime(const rational& t) const {
    for (auto m : markers_) {     // 遍历所有标记
      if (m->time().in() == t) {  // 如果标记的入点时间与指定时间相同
        return m;                 // 返回该标记
      }
    }
    return nullptr;  // 未找到则返回空指针
  }

  /**
   * @brief 获取距离指定时间点最近的标记。
   *
   * 假设标记列表已按时间排序。
   * @param t 参考时间点。
   * @return 返回距离指定时间点最近的 TimelineMarker 指针；如果列表为空，则返回 nullptr。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] TimelineMarker* GetClosestMarkerToTime(const rational& t) const {
    TimelineMarker* closest = nullptr;  // 初始化最近标记为空

    for (auto m : markers_) {                                   // 遍历所有标记
      rational this_diff = rational::qAbs(m->time().in() - t);  // 计算当前标记与目标时间的差值绝对值

      if (closest) {                                                      // 如果已经有一个最近标记的候选者
        rational stored_diff = rational::qAbs(closest->time().in() - t);  // 计算已存储的最近标记与目标时间的差值绝对值

        if (this_diff > stored_diff) {  // 如果当前标记的差值更大
          // 由于列表按时间组织，如果差值开始增加，
          // 假设我们只会离目标时间越来越远，无需继续检查。
          break;
        }
      }
      closest = m;  // 更新最近标记为当前标记
    }
    return closest;  // 返回找到的最近标记
  }

 signals:
  /**
   * @brief 当有新标记被添加到列表时发出的信号。
   * @param marker 被添加的 TimelineMarker 对象的指针。
   */
  void MarkerAdded(TimelineMarker* marker);

  /**
   * @brief 当有标记从列表中移除时发出的信号。
   * @param marker 被移除的 TimelineMarker 对象的指针。
   */
  void MarkerRemoved(TimelineMarker* marker);

  /**
   * @brief 当列表中的某个标记被修改时发出的信号。
   * @param marker 被修改的 TimelineMarker 对象的指针。
   */
  void MarkerModified(TimelineMarker* marker);

 protected:
  /**
   * @brief 处理子对象事件（例如子对象被添加或移除）。
   *
   * QObject 的标准虚函数重写，用于响应子对象的变化。
   * @param e QChildEvent 事件对象。
   */
  void childEvent(QChildEvent* e) override;

 private:
  /**
   * @brief 将标记插入到内部列表中，并保持排序（通常按时间）。
   * @param m 要插入的 TimelineMarker 指针。
   */
  void InsertIntoList(TimelineMarker* m);

  /**
   * @brief 从内部列表中移除指定的标记。
   * @param m 要移除的 TimelineMarker 指针。
   * @return 如果成功移除返回 true，否则返回 false（例如标记不在列表中）。
   */
  bool RemoveFromList(TimelineMarker* m);

  /**
   * @brief 存储 TimelineMarker 指针的向量。
   *
   * 这是标记列表的实际数据容器。
   */
  std::vector<TimelineMarker*> markers_;

 private slots:
  /**
   * @brief 处理标记修改的槽函数。
   *
   * 当列表中的某个标记发出内容改变的信号时（例如名称、颜色改变，非时间改变），
   * 此槽函数会被调用，进而发出 MarkerModified 信号。
   */
  void HandleMarkerModification();

  /**
   * @brief 处理标记时间改变的槽函数。
   *
   * 当列表中的某个标记的时间范围发生改变时，此槽函数会被调用。
   * 它负责从列表中移除旧的标记，重新插入以保持排序，并发出 MarkerModified 信号。
   */
  void HandleMarkerTimeChange();
};

/**
 * @brief 添加标记的撤销命令。
 *
 * 继承自 UndoCommand，用于在撤销/重做框架中实现添加标记的操作。
 */
class MarkerAddCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数，通过参数创建新的标记并添加到列表。
   * @param marker_list 目标标记列表。
   * @param range 新标记的时间范围。
   * @param name 新标记的名称。
   * @param color 新标记的颜色。
   */
  MarkerAddCommand(TimelineMarkerList* marker_list, const TimeRange& range, const QString& name, int color);

  /**
   * @brief 构造函数，添加一个已存在的标记到列表（通常用于重做操作）。
   * @param marker_list 目标标记列表。
   * @param marker 要添加的 TimelineMarker 指针。
   */
  MarkerAddCommand(TimelineMarkerList* marker_list, TimelineMarker* marker);

  /**
   * @brief 获取与此命令相关的项目对象。
   *
   * UndoCommand 的虚函数，用于确定命令作用的项目上下文。
   * @return 返回相关的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行添加标记的操作。
   */
  void redo() override;

  /**
   * @brief 撤销添加标记的操作。
   */
  void undo() override;

 private:
  TimelineMarkerList* marker_list_;  // 指向目标标记列表的指针
  TimelineMarker* added_marker_;     // 指向被添加的标记的指针
  QObject memory_manager_;  // 用于管理 added_marker_ 的生命周期，确保在命令销毁时标记也被正确处理（如果是由命令创建的）
};

/**
 * @brief 移除标记的撤销命令。
 *
 * 继承自 UndoCommand，用于在撤销/重做框架中实现移除标记的操作。
 */
class MarkerRemoveCommand : public UndoCommand {
 public:
  /**
   * @brief 显式构造函数。
   * @param marker 要移除的 TimelineMarker 对象的指针。
   */
  explicit MarkerRemoveCommand(TimelineMarker* marker);

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回相关的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行移除标记的操作。
   */
  void redo() override;

  /**
   * @brief 撤销移除标记的操作（即重新添加标记）。
   */
  void undo() override;

 private:
  TimelineMarker* marker_;  // 指向被移除的标记的指针
  QObject* marker_list_{};  // 指向标记所属列表的指针（在构造时获取）
  QObject memory_manager_;  // 用于管理 marker_ 的生命周期，确保在撤销后重新添加时标记仍然有效
};

/**
 * @brief 更改标记颜色的撤销命令。
 */
class MarkerChangeColorCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param marker 要修改颜色的标记。
   * @param new_color 新的颜色值。
   */
  MarkerChangeColorCommand(TimelineMarker* marker, int new_color);

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回相关的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行更改颜色的操作。
   */
  void redo() override;

  /**
   * @brief 撤销更改颜色的操作。
   */
  void undo() override;

 private:
  TimelineMarker* marker_;  // 指向被修改的标记
  int old_color_{};         // 标记的旧颜色值
  int new_color_;           // 标记的新颜色值
};

/**
 * @brief 更改标记名称的撤销命令。
 */
class MarkerChangeNameCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param marker 要修改名称的标记。
   * @param name 新的名称。
   */
  MarkerChangeNameCommand(TimelineMarker* marker, QString name);

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回相关的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行更改名称的操作。
   */
  void redo() override;

  /**
   * @brief 撤销更改名称的操作。
   */
  void undo() override;

 private:
  TimelineMarker* marker_;  // 指向被修改的标记
  QString old_name_;        // 标记的旧名称
  QString new_name_;        // 标记的新名称
};

/**
 * @brief 更改标记时间的撤销命令。
 */
class MarkerChangeTimeCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param marker 要修改时间的标记。
   * @param time 新的时间范围。
   * @param old_time 旧的时间范围（用于撤销）。
   */
  MarkerChangeTimeCommand(TimelineMarker* marker, const TimeRange& time, const TimeRange& old_time);

  /**
   * @brief 构造函数重载，自动获取标记的当前时间作为旧时间。
   * @param marker 要修改时间的标记。
   * @param time 新的时间范围。
   */
  MarkerChangeTimeCommand(TimelineMarker* marker, const TimeRange& time)
      : MarkerChangeTimeCommand(marker, time, marker->time()) {}  // 调用主构造函数

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回相关的 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  /**
   * @brief 执行更改时间的操作。
   */
  void redo() override;

  /**
   * @brief 撤销更改时间的操作。
   */
  void undo() override;

 private:
  TimelineMarker* marker_;  // 指向被修改的标记
  TimeRange old_time_;      // 标记的旧时间范围
  TimeRange new_time_;      // 标记的新时间范围
};

}  // namespace olive

#endif  // TIMELINEMARKER_H
