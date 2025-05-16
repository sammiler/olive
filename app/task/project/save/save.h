#ifndef PROJECTSAVEMANAGER_H  // 防止头文件被重复包含的预处理器指令
#define PROJECTSAVEMANAGER_H  // 定义 PROJECTSAVEMANAGER_H 宏

#include "node/project.h"                            // 包含了项目数据结构 (Project 类) 的定义
#include "task/task.h"                               // 包含了任务基类 (Task 类) 的定义
#include "window/mainwindow/mainwindowlayoutinfo.h"  // 包含了主窗口布局信息 (MainWindowLayoutInfo 类) 的定义

// olive 命名空间前向声明 (如果 ProjectSaveTask 中使用了其他 olive 命名空间下的类型作为指针或引用，
// 且这些类型的完整定义不需要在此头文件中，则可以在此进行前向声明。
// 但根据当前代码，Project, Task, MainWindowLayoutInfo 等类型预期从上述 #include 中获得。)

namespace olive {  // olive 项目的命名空间

/**
 * @brief ProjectSaveTask 类定义，继承自 Task 类。
 *
 * 此类负责执行将当前 Olive 项目保存到文件的任务。
 * 它可以将项目数据（包括节点、序列、素材等）以及可选的主窗口布局信息
 *序列化并写入磁盘。支持使用压缩来减小项目文件的大小。
 */
class ProjectSaveTask : public Task {
 Q_OBJECT  // Qt 对象的宏，用于启用信号和槽机制等 Qt 特性
     public :
     /**
      * @brief ProjectSaveTask 的构造函数。
      * @param project 指向要保存的 Project 对象的指针。此对象包含了所有项目数据。
      * @param use_compression 一个布尔值，指示在保存项目文件时是否应使用压缩。
      * 如果为 true，则会尝试压缩项目文件以减小其大小。
      */
     ProjectSaveTask(Project* project, bool use_compression);

  /**
   * @brief 获取当前任务正在处理的 Project 对象。
   * @return Project* 指向正在保存的 Project 对象的指针。
   */
  [[nodiscard]] Project* GetProject() const { return project_; }

  /**
   * @brief 设置用于保存项目的覆盖文件名。
   *
   * 如果设置了此文件名，则项目将保存到指定的文件路径，而不是使用项目对象内部的默认路径。
   * @param filename 要使用的覆盖文件名（包含路径）。
   */
  void SetOverrideFilename(const QString& filename) { override_filename_ = filename; }

  /**
   * @brief 设置要与项目一起保存的主窗口布局信息。
   * @param layout 包含主窗口布局信息的 MainWindowLayoutInfo 对象。
   */
  void SetLayout(const MainWindowLayoutInfo& layout) { layout_ = layout; }

 protected:
  /**
   * @brief 执行项目保存任务的核心逻辑。
   *
   * 此方法重写自基类 Task 的 Run 方法。当任务被调度执行时，此函数会被调用。
   * 它负责将 `project_`成员指向的项目对象序列化，并根据 `override_filename_`
   * (如果已设置) 或项目默认文件名将其写入磁盘。同时，也会保存 `layout_` 中的布局信息。
   * 压缩选项 `use_compression_` 会在写入文件时被考虑。
   * @return 如果项目成功保存，则返回 true；如果保存过程中发生任何错误（如文件写入失败），
   * 则返回 false。
   */
  bool Run() override;

 private:
  Project* project_;  ///< @brief 指向需要被保存的 Project 对象的指针。

  QString override_filename_;  ///< @brief 可选的覆盖文件名。如果为空，则使用项目内部定义的文件名。

  bool use_compression_;  ///< @brief 标志位，指示是否在保存项目文件时使用压缩。

  MainWindowLayoutInfo layout_;  ///< @brief 存储要与项目一起保存的主窗口布局信息。
};

}  // namespace olive

#endif  // PROJECTSAVEMANAGER_H // 结束预处理器指令 #ifndef PROJECTSAVEMANAGER_H
