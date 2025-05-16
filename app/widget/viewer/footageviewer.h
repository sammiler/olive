#ifndef FOOTAGEVIEWERWIDGET_H // 防止头文件被多次包含的宏定义
#define FOOTAGEVIEWERWIDGET_H

#include "node/output/viewer/viewer.h" // 引入 ViewerOutput 类（可能也包含 TimeRange 定义）
#include "viewer.h"                    // 引入 ViewerWidget 类的定义，FootageViewerWidget 是其派生类


namespace olive { // olive 命名空间开始

class TimelineWorkArea; // 前向声明 TimelineWorkArea 类，用于定义工作区范围

/**
 * @brief FootageViewerWidget 类是一个专门用于显示和交互单个素材片段的查看器控件。
 *
 * 它继承自 ViewerWidget，提供了显示视频帧、播放控制以及可能的拖拽源功能，
 * 允许用户将素材（或其一部分）拖拽到时间轴或其他位置。
 * 此类还支持覆盖和重置工作区（入点/出点）。
 */
class FootageViewerWidget : public ViewerWidget {
  Q_OBJECT // Q_OBJECT 宏，用于启用 Qt 的元对象特性，如信号和槽

 public:
  /**
   * @brief 构造一个 FootageViewerWidget 对象。
   * @param parent 父 QWidget 对象，默认为 nullptr。
   */
  explicit FootageViewerWidget(QWidget *parent = nullptr);

  /**
   * @brief 覆盖当前查看器的工作区（入点和出点）。
   *
   * 这允许临时定义一个不同于素材原始长度的播放或拖拽范围。
   * @param r 新的工作区时间范围 (TimeRange)。
   */
  void OverrideWorkArea(const TimeRange &r);
  /**
   * @brief 重置（清除）被覆盖的工作区。
   *
   * 调用此方法后，查看器将恢复使用素材的完整范围或其默认工作区。
   */
  void ResetWorkArea();

 private: // 私有方法
  /**
   * @brief 内部辅助函数，用于开始素材的拖拽操作。
   * @param enable_video 如果为 true，则拖拽数据中包含视频部分。
   * @param enable_audio 如果为 true，则拖拽数据中包含音频部分。
   */
  void StartFootageDragInternal(bool enable_video, bool enable_audio);

  TimelineWorkArea *override_workarea_; ///< 指向一个可选的、覆盖默认工作区的 TimelineWorkArea 对象的指针。

 private slots: // 私有槽函数
  /**
   * @brief 开始拖拽素材（同时包含视频和音频，如果可用）。
   *
   * 通常由用户界面上的某个按钮或手势触发。
   */
  void StartFootageDrag();

  /**
   * @brief 开始仅拖拽素材的视频部分。
   *
   * 通常由用户界面上的特定视频拖拽按钮触发。
   */
  void StartVideoDrag();

  /**
   * @brief 开始仅拖拽素材的音频部分。
   *
   * 通常由用户界面上的特定音频拖拽按钮触发。
   */
  void StartAudioDrag();

  /**
   * @brief 当与视频相关的按钮（例如，仅拖拽视频按钮）被点击时调用的槽函数。
   */
  void VideoButtonClicked();

  /**
   * @brief 当与音频相关的按钮（例如，仅拖拽音频按钮）被点击时调用的槽函数。
   */
  void AudioButtonClicked();
};

}  // namespace olive

#endif  // FOOTAGEVIEWERWIDGET_H
