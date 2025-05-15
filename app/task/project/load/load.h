#ifndef PROJECTLOADMANAGER_H // 防止头文件被重复包含的预处理器指令
#define PROJECTLOADMANAGER_H // 定义 PROJECTLOADMANAGER_H 宏

#include "loadbasetask.h"                         // 包含了项目加载任务基类的定义
#include "window/mainwindow/mainwindowlayoutinfo.h" // 包含了主窗口布局信息的定义，加载项目时可能需要恢复布局

namespace olive { // olive 项目的命名空间

/**
 * @brief ProjectLoadTask 类定义，继承自 ProjectLoadBaseTask 类。
 *
 * 此类专门用于执行从文件加载整个 Olive 项目的任务。
 * 它负责解析项目文件，重建项目结构、节点、序列以及相关的设置，
 * 并可能恢复用户界面的布局信息。
 */
class ProjectLoadTask : public ProjectLoadBaseTask {
  Q_OBJECT // Qt 对象的宏，用于启用信号和槽机制等 Qt 特性
 public:
  /**
   * @brief ProjectLoadTask 的构造函数。
   * @param filename 要加载的项目文件的完整路径。
   */
  explicit ProjectLoadTask(const QString& filename);

protected:
  /**
   * @brief 执行项目加载任务的核心逻辑。
   *
   * 此方法重写自 ProjectLoadBaseTask 的 Run 方法。当任务被调度执行时，此函数会被调用。
   * 它负责打开并解析指定的项目文件，根据文件内容重建项目的数据结构，
   * 包括序列、素材、节点图以及可能的窗口布局等。
   * @return 如果项目成功加载，则返回 true；如果加载过程中发生任何错误（如文件不存在、文件格式错误等），
   * 则返回 false。
   */
  bool Run() override;
};

}  // namespace olive // 结束 olive 命名空间

#endif  // PROJECTLOADMANAGER_H // 结束预处理器指令 #ifndef PROJECTLOADMANAGER_H
