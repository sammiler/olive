#ifndef PROJECTLOADBASETASK_H  // 防止头文件被重复包含的预处理器指令
#define PROJECTLOADBASETASK_H  // 定义 PROJECTLOADBASETASK_H 宏

#include "node/project.h"                            // 包含了项目数据结构 (Project 类) 的定义
#include "task/task.h"                               // 包含了任务基类 (Task 类) 的定义
#include "window/mainwindow/mainwindowlayoutinfo.h"  // 包含了主窗口布局信息 (MainWindowLayoutInfo 类) 的定义

// olive 命名空间前向声明 (如果 ProjectLoadBaseTask 中使用了其他 olive 命名空间下的类型作为指针或引用，
// 且这些类型的完整定义不需要在此头文件中，则可以在此进行前向声明。
// 但根据当前代码，Project, Task, MainWindowLayoutInfo 等类型预期从上述 #include 中获得。)

namespace olive {  // olive 项目的命名空间

/**
 * @brief ProjectLoadBaseTask 类定义，继承自 Task 类。
 *
 * 此类是项目加载任务的基类，提供了加载项目文件所需的基础功能和数据成员。
 * 具体的加载逻辑由派生类（如 ProjectLoadTask）实现。
 * 它负责存储加载的项目对象、文件名以及相关的布局信息。
 */
class ProjectLoadBaseTask : public Task {
 Q_OBJECT  // Qt 对象的宏，用于启用信号和槽机制等 Qt 特性
     public :
     /**
      * @brief ProjectLoadBaseTask 的构造函数。
      * @param filename 要加载的项目文件的完整路径字符串。
      */
     explicit ProjectLoadBaseTask(const QString& filename);

  /**
   * @brief 获取加载完成的 Project 对象指针。
   *
   * 在任务成功执行后，可以通过此方法获取到加载并构建的项目对象。
   * @return Project* 指向已加载 Project 对象的指针。如果项目尚未加载或加载失败，可能返回 nullptr。
   */
  [[nodiscard]] Project* GetLoadedProject() const { return project_; }

  /**
   * @brief 获取正在加载的项目的文件名。
   * @return const QString& 对项目文件名的常量引用。
   */
  [[nodiscard]] const QString& GetFilename() const { return filename_; }

  /**
   * @brief 获取从项目文件中加载的主窗口布局信息。
   * @return const MainWindowLayoutInfo& 对 MainWindowLayoutInfo 对象的常量引用。
   */
  [[nodiscard]] const MainWindowLayoutInfo& GetLoadedLayout() const { return layout_; }

 protected:
  Project* project_;  ///< @brief 指向加载后创建的 Project 对象的指针。派生类在 Run() 方法中负责创建和填充此对象。

  MainWindowLayoutInfo layout_;  ///< @brief 存储从项目文件中解析出的主窗口布局信息。

 private:
  QString filename_;  ///< @brief 存储要加载的项目文件的路径和名称。
};

}  // namespace olive

#endif  // LOADBASETASK_H // 结束预处理器指令 #ifndef PROJECTLOADBASETASK_H
        // (注意：这里的宏定义与ifndef不完全匹配，但按用户要求不修改代码)
