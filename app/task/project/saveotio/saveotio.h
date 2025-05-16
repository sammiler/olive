#ifndef PROJECTSAVEASOTIOTASK_H  // 防止头文件被重复包含的预处理器指令
#define PROJECTSAVEASOTIOTASK_H  // 定义 PROJECTSAVEASOTIOTASK_H 宏

#include <opentimelineio/timeline.h>  // 包含了 OpenTimelineIO (OTIO) 的时间线定义
#include <opentimelineio/track.h>     // 包含了 OpenTimelineIO (OTIO) 的轨道定义

#include "common/otioutils.h"  // 包含了项目中与 OTIO 相关的实用工具函数或类
#include "node/project.h"      // 包含了项目数据结构 (Project 类) 的定义
#include "task/task.h"         // 包含了任务基类 (Task 类) 的定义

// 前向声明 (如果 SaveOTIOTask 中使用了其他 olive 命名空间下的类型作为指针或引用，
// 且这些类型的完整定义不需要在此头文件中，则可以在此进行前向声明。
// 例如，Sequence, Track, TrackList 等类型预期从 "node/project.h" 或其他相关头文件中获得。)
// 注意：OTIO 命名空间下的类型已通过上面的 #include 引入。

namespace olive {  // olive 项目的命名空间

/**
 * @brief SaveOTIOTask 类定义，继承自 Task 类。
 *
 * 此类负责将 Olive 项目（或其一部分，如序列）序列化并保存为
 * OpenTimelineIO (OTIO) 格式的文件。OTIO 是一种用于交换编辑决策列表（EDL）、
 * 时间线等信息的开放标准格式，便于在不同的视频编辑软件之间进行互操作。
 */
class SaveOTIOTask : public Task {
 Q_OBJECT  // Qt 对象的宏，用于启用信号和槽机制等 Qt 特性
     public :
     /**
      * @brief SaveOTIOTask 的构造函数。
      * @param project 指向要保存为 OTIO 格式的 Project 对象的指针。
      */
     explicit SaveOTIOTask(Project* project);

 protected:
  /**
   * @brief 执行将项目保存为 OTIO 文件的核心逻辑。
   *
   * 此方法重写自基类 Task 的 Run 方法。当任务被调度执行时，此函数会被调用。
   * 它负责遍历项目中的序列 (Sequence)，将每个序列转换为 OTIO 时间线 (Timeline)，
   * 然后将这些 OTIO 时间线写入到指定的文件中（文件名可能由项目属性或用户指定）。
   * @return 如果项目成功保存为 OTIO 文件，则返回 true；如果保存过程中发生任何错误
   * (如文件写入失败、序列化失败等)，则返回 false。
   */
  bool Run() override;

 private:
  /**
   * @brief 将 Olive 的 Sequence 对象序列化为 OTIO 的 Timeline 对象。
   *
   * 此方法负责转换单个序列及其包含的轨道和片段。
   * @param sequence 指向要序列化的 Olive Sequence 对象的指针。
   * @return OTIO::Timeline* 指向新创建的 OTIO Timeline 对象的指针。如果序列化失败，可能返回 nullptr。
   */
  OTIO::Timeline* SerializeTimeline(Sequence* sequence);

  /**
   * @brief 静态辅助函数，将 Olive 的 Track 对象序列化为 OTIO 的 Track 对象。
   * @param track 指向要序列化的 Olive Track 对象的指针。
   * @param sequence_rate 序列的帧率，用于时间转换。
   * @param max_track_length 轨道的最大长度（以有理数表示），可能用于确定 OTIO 轨道的范围。
   * @return OTIO::Track* 指向新创建的 OTIO Track 对象的指针。如果序列化失败，可能返回 nullptr。
   */
  static OTIO::Track* SerializeTrack(Track* track, double sequence_rate, rational max_track_length);

  /**
   * @brief 静态辅助函数，序列化 Olive 的 TrackList 并将其添加到 OTIO Timeline 中。
   * @param list 指向要序列化的 Olive TrackList 对象的指针。
   * @param otio_timeline 指向目标 OTIO Timeline 对象的指针，序列化后的轨道将添加到此时间线。
   * @param sequence_rate 序列的帧率，用于时间转换。
   * @return 如果所有轨道都成功序列化并添加到 OTIO 时间线，则返回 true；否则返回 false。
   */
  static bool SerializeTrackList(TrackList* list, OTIO::Timeline* otio_timeline, double sequence_rate);

  Project* project_;  ///< @brief 指向需要被保存为 OTIO 格式的 Project 对象的指针。
};

}  // namespace olive

#endif  // OTIODECODER_H (注意：此处的宏定义 PROJECTSAVEASOTIOTASK_H 与 #ifndef 不完全匹配，但按用户要求不修改代码)
