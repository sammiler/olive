#ifndef OTIODECODER_H  // 防止头文件被重复包含的预处理器指令
#define OTIODECODER_H  // 定义 OTIODECODER_H 宏

#include "common/otioutils.h"                // 包含了 OpenTimelineIO (OTIO) 相关的实用工具函数或类
#include "node/project.h"                    // 包含了项目数据结构 (Project 类) 的定义
#include "task/project/load/loadbasetask.h"  // 包含了项目加载任务基类 (ProjectLoadBaseTask) 的定义

namespace olive {  // olive 项目的命名空间

/**
 * @brief LoadOTIOTask 类定义，继承自 ProjectLoadBaseTask 类。
 *
 * 此类专门用于处理从 OpenTimelineIO (OTIO) 格式的文件加载项目信息的任务。
 * OTIO 是一种用于交换编辑时间线信息的开放标准格式。这个任务会解析 OTIO 文件，
 * 并将其内容转换为 Olive 项目内部的数据结构。
 */
class LoadOTIOTask : public ProjectLoadBaseTask {
 Q_OBJECT  // Qt 对象的宏，用于启用信号和槽机制等 Qt 特性
     public :
     /**
      * @brief LoadOTIOTask 的构造函数。
      * @param s 要加载的 OTIO 文件的完整路径字符串。
      */
     explicit LoadOTIOTask(const QString& s);

 protected:
  /**
   * @brief 执行从 OTIO 文件加载项目任务的核心逻辑。
   *
   * 此方法重写自 ProjectLoadBaseTask 的 Run 方法。当任务被调度执行时，此函数会被调用。
   * 它负责打开并解析指定的 OTIO 文件 (路径存储在基类的 filename_ 成员中，通过构造函数传递)，
   * 然后将 OTIO 数据结构转换为 Olive 的项目结构 (Project 对象，存储在基类的 project_ 成员中)。
   * @return 如果 OTIO 文件成功加载并转换为 Olive 项目，则返回 true；
   * 如果加载过程中发生任何错误（如文件不存在、文件格式错误、转换失败等），则返回 false。
   */
  bool Run() override;
};

}  // namespace olive

#endif  // OTIODECODER_H // 结束预处理器指令 #ifndef OTIODECODER_H
