#ifndef TIMELINEUNDOWORKAREA_H
#define TIMELINEUNDOWORKAREA_H

#include "node/project.h"  // 引入 Project 类的定义，工作区操作通常与项目关联
// 前向声明 TimelineWorkArea，因为头文件可能只依赖其指针或引用
// 实际的 TimelineWorkArea 定义应该在其他地方，这里可能只需要知道它是一个类
// (如果 TimelineWorkArea 是在此项目中定义的，确保其头文件被包含，
//  但从上下文看，Project.h 可能已经间接包含了它，或者这里只需要前向声明)
// 假设 TimelineWorkArea 是一个已知的类型，例如通过 Project.h 间接引入或在其他地方定义

namespace olive {

// 通常 TimelineWorkArea 类的定义会在这里或者被包含进来
// class TimelineWorkArea; // 如果 Project.h 没有包含它，可能需要这样的前向声明

/**
 * @brief 设置时间轴工作区（TimelineWorkArea）启用状态的撤销命令。
 *
 * 时间轴工作区可能是一个用户定义的区域，用于标记特定的编辑范围或导出范围。
 * 此命令允许撤销/重做对该工作区启用/禁用状态的更改。
 */
class WorkareaSetEnabledCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数。
   * @param project 指向相关的 Project 对象。
   * @param points 指向要操作的 TimelineWorkArea 对象。
   * @param enabled 工作区的新启用状态 (true 为启用, false 为禁用)。
   */
  WorkareaSetEnabledCommand(Project* project, TimelineWorkArea* points, bool enabled)
      : project_(project), points_(points), old_enabled_(points_->enabled()), new_enabled_(enabled) {}

  /**
   * @brief 获取与此命令相关的项目对象。
   * @return 返回 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return project_; }

 protected:
  /**
   * @brief 执行设置工作区启用状态的操作。
   */
  void redo() override { points_->set_enabled(new_enabled_); }

  /**
   * @brief 撤销设置工作区启用状态的操作。
   */
  void undo() override { points_->set_enabled(old_enabled_); }

 private:
  Project* project_;          // 指向相关的项目对象
  TimelineWorkArea* points_;  // 指向被操作的时间轴工作区对象
  bool old_enabled_;          // 工作区原始的启用状态
  bool new_enabled_;          // 工作区新的启用状态
};

/**
 * @brief 设置时间轴工作区（TimelineWorkArea）时间范围的撤销命令。
 *
 * 此命令允许撤销/重做对工作区时间范围（入点和出点）的更改。
 */
class WorkareaSetRangeCommand : public UndoCommand {
 public:
  /**
   * @brief 构造函数，明确指定旧的时间范围。
   * @param workarea 指向要操作的 TimelineWorkArea 对象。
   * @param range 新的时间范围。
   * @param old_range 旧的时间范围，用于撤销。
   */
  WorkareaSetRangeCommand(TimelineWorkArea* workarea, const TimeRange& range, const TimeRange& old_range)
      : workarea_(workarea), old_range_(old_range), new_range_(range) {}

  /**
   * @brief 构造函数重载，自动获取工作区当前的范围作为旧范围。
   * @param workarea 指向要操作的 TimelineWorkArea 对象。
   * @param range 新的时间范围。
   */
  WorkareaSetRangeCommand(TimelineWorkArea* workarea, const TimeRange& range)
      : WorkareaSetRangeCommand(workarea, range, workarea->range()) {}  // 调用主构造函数，并获取当前范围作为旧范围

  /**
   * @brief 获取与此命令相关的项目对象。
   *
   * 通过 TimelineWorkArea 对象来获取其所属的 Project。
   * @return 返回 Project 指针。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetRelevantProject() const override { return Project::GetProjectFromObject(workarea_); }

 protected:
  /**
   * @brief 执行设置工作区时间范围的操作。
   */
  void redo() override { workarea_->set_range(new_range_); }

  /**
   * @brief 撤销设置工作区时间范围的操作。
   */
  void undo() override { workarea_->set_range(old_range_); }

 private:
  TimelineWorkArea* workarea_;  // 指向被操作的时间轴工作区对象
  TimeRange old_range_;         // 工作区原始的时间范围
  TimeRange new_range_;         // 工作区新的时间范围
};

}  // namespace olive

#endif  // TIMELINEUNDOWORKAREA_H
