#ifndef TASKMANAGER_PANEL_H // 防止头文件被重复包含的宏 (文件名似乎应为 TASKMANAGERPANEL_H 以匹配类名)
#define TASKMANAGER_PANEL_H // 定义 TASKMANAGER_PANEL_H 宏

#include "panel/panel.h"          // 包含 PanelWidget 基类的定义
#include "widget/taskview/taskview.h" // 包含 TaskView 控件的定义

namespace olive { // olive 项目的命名空间

/**
 * @brief TaskManagerPanel 类是一个 PanelWidget 的包装器，用于封装和管理一个任务视图 (TaskView) 控件。
 *
 * 这个面板用于显示和管理后台任务的执行情况，例如渲染任务、文件导入/导出任务等。
 * 用户可以通过这个面板查看任务的进度、状态 (运行中、已完成、失败等)，
 * 以及可能的操作 (如取消任务)。
 */
class TaskManagerPanel : public PanelWidget { // TaskManagerPanel 继承自 PanelWidget
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  // 构造函数
  TaskManagerPanel();

private:
  /**
   * @brief 重写基类的 Retranslate 方法，用于在语言更改时更新此面板内UI元素的文本。
   * 例如，可能会更新面板标题或任务列表的列标题。
   */
  void Retranslate() override;

  TaskView* view_; // 指向内部的任务视图控件 (TaskView) 的指针。
  // TaskView 负责实际显示任务列表和它们的状态。
};

}  // namespace olive

#endif  // TASKMANAGER_H (或应为 TASKMANAGERPANEL_H)