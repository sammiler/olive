#ifndef PRECACHETASK_H // 防止头文件被重复包含的预处理器指令
#define PRECACHETASK_H // 定义 PRECACHETASK_H 宏

#include "node/project/footage/footage.h" // 包含了项目素材相关的定义
#include "node/project/sequence/sequence.h" // 包含了序列相关的定义
#include "task/render/render.h"             // 包含了渲染任务基类的定义

namespace olive { // olive 项目的命名空间

/**
 * @brief PreCacheTask 类定义，继承自 RenderTask 类。
 *
 * 此类用于执行预缓存任务。预缓存任务的目的是提前渲染并存储素材（Footage）的
 * 视频帧和音频样本，以便在后续播放或编辑操作中能够快速访问，从而提高性能和响应速度。
 * 它通常针对特定的素材片段或序列中的一部分进行操作。
 */
class PreCacheTask : public RenderTask {
  Q_OBJECT // Qt 对象的宏，用于启用信号和槽机制等 Qt 特性
 public:
  /**
   * @brief PreCacheTask 的构造函数。
   * @param footage 指向要进行预缓存的 Footage 对象的指针。
   * @param index 一个索引值，可能用于标识素材中的特定流或部分（例如，视频流索引或音频轨道索引）。
   * @param sequence 指向当前操作的 Sequence 对象的指针，预缓存通常在特定序列的上下文中进行。
   */
  PreCacheTask(Footage* footage, int index, Sequence* sequence);

  /**
   * @brief PreCacheTask 的析构函数。
   *
   * 重写基类的虚析构函数，确保正确的资源清理。
   */
  ~PreCacheTask() override;

 protected:
  /**
   * @brief 执行预缓存任务的核心逻辑。
   *
   * 此方法重写自 RenderTask 的 Run 方法。当任务被调度执行时，此函数会被调用。
   * 它负责请求渲染指定素材的帧和音频数据，并将这些数据存储到缓存中。
   * @return 如果任务成功完成（或按预期方式处理完毕），则返回 true；否则返回 false。
   */
  bool Run() override;

  /**
   * @brief 当一帧视频数据下载完成（即渲染完成）时被调用。
   *
   * 此方法重写自 RenderTask。它接收渲染好的视频帧，并负责将其存入预缓存。
   * @param frame 指向下载完成的视频帧的智能指针。
   * @param times 该帧对应的时间点（以有理数表示）。
   * @return 如果帧被成功处理并存入缓存，则返回 true；否则返回 false。
   */
  bool FrameDownloaded(FramePtr frame, const rational& times) override;

  /**
   * @brief 当一段音频数据下载完成（即渲染完成）时被调用。
   *
   * 此方法重写自 RenderTask。它接收渲染好的音频样本，并负责将其存入预缓存。
   * @param range 该段音频数据的时间范围。
   * @param samples 包含下载完成的音频样本的 SampleBuffer。
   * @return 如果音频数据被成功处理并存入缓存，则返回 true；否则返回 false。
   */
  bool AudioDownloaded(const TimeRange& range, const SampleBuffer& samples) override;

 private:
  Project* project_; ///< @brief 指向当前项目对象的指针。预缓存操作通常与整个项目相关联。
                     ///< 注意：此成员未在构造函数中初始化，可能由基类或后续设置。

  Footage* footage_; ///< @brief 指向当前正在进行预缓存操作的 Footage (素材) 对象的指针。
};

}  // namespace olive // 结束 olive 命名空间

#endif  // PRECACHETASK_H // 结束预处理器指令 #ifndef PRECACHETASK_H
