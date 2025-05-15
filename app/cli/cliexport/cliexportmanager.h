

#ifndef CLIEXPORTMANAGER_H
#define CLIEXPORTMANAGER_H

#include "task/export/export.h" // 假设 Export 类定义在此

namespace olive {

/**
 * @brief 管理命令行界面 (CLI) 导出操作的类。
 *
 * 此类负责处理通过命令行启动的导出任务的逻辑和流程。
 * 它通常会与导出任务 (Export) 类交互，以执行实际的导出工作。
 * 作为 QObject 的子类，它可以利用 Qt 的信号和槽机制。
 */
class CLIExportManager : public QObject {
  Q_OBJECT // Qt元对象系统宏，如果需要信号槽等特性
 public:
  /**
   * @brief 构造一个新的 CLIExportManager 对象。
   */
  CLIExportManager();

  // 根据实际的功能，这里可能会有更多的方法，例如：
  // - 初始化导出参数的方法
  // - 开始导出过程的方法
  // - 处理导出状态和进度的方法
  // - 清理资源的方法
  // 由于当前代码中只有构造函数，所以只注释构造函数。
  // 如果未来添加其他成员（方法、变量、信号等），我会为它们添加相应的注释。
};

}  // namespace olive

#endif  // CLIEXPORTMANAGER_H