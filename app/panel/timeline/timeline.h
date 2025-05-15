#ifndef TIMELINE_PANEL_H // 防止头文件被重复包含的宏
#define TIMELINE_PANEL_H // 定义 TIMELINE_PANEL_H 宏

#include "panel/timebased/timebased.h"     // 包含 TimeBasedPanel 基类的定义
#include "widget/timelinewidget/timelinewidget.h" // 包含 TimelineWidget 控件的定义

namespace olive { // olive 项目的命名空间

class Sequence; // 向前声明 Sequence 类

/**
 * @brief TimelinePanel 类是一个用于容纳和管理时间轴控件 (TimelineWidget) 的面板。
 *
 * 它继承自 TimeBasedPanel，因此具备时间导航、播放控制等与时间相关的基本功能。
 * 这个面板是视频编辑的核心界面，用户在这里排列视频片段、音频片段、添加效果、
 * 进行剪辑操作 (如分割、波纹删除、设置过渡等)。
 *
 * TimelinePanel 负责将用户的操作意图传递给内部的 TimelineWidget，并可能
 * 发出信号以通知其他模块 (如序列查看器、参数面板) 进行相应的更新。
 */
class TimelinePanel : public TimeBasedPanel { // TimelinePanel 继承自 TimeBasedPanel
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  /**
   * @brief 构造函数。
   * @param name 面板的名称 (通常是 Qt 对象名称)。
   */
  explicit TimelinePanel(const QString &name);

  /**
   * @brief 获取内部封装的 TimelineWidget 控件的指针。
   * 通过动态类型转换从基类 (TimeBasedPanel) 提供的 GetTimeBasedWidget() 方法获取。
   * @return 返回 TimelineWidget 指针，如果转换失败或内部控件不是 TimelineWidget 类型，则返回 nullptr。
   */
  [[nodiscard]] inline TimelineWidget *timeline_widget() const {
    // 将基类提供的 TimeBasedWidget 动态转换为 TimelineWidget
    return dynamic_cast<TimelineWidget *>(GetTimeBasedWidget());
  }

  /**
   * @brief 在播放头当前位置分割选中的片段。
   */
  void SplitAtPlayhead() const;

  // --- 重写 PanelWidget 或特定接口的方法，用于保存/加载面板状态 ---
  /**
   * @brief (可能重写自PanelWidget) 加载面板的特定数据/状态。
   * @param info 包含要加载数据的 Info 结构体或对象。
   */
  void LoadData(const Info &info) override;
  /**
   * @brief (可能重写自PanelWidget) 保存面板的特定数据/状态。
   * @return 返回包含面板数据的 Info 结构体或对象。
   */
  [[nodiscard]] Info SaveData() const override;

  // --- 重写 TimeBasedPanel 或通用编辑接口的方法 ---
  void SelectAll() override;    // 全选时间轴上的所有可选内容 (例如片段)
  void DeselectAll() override;  // 取消选择时间轴上的所有内容

  void RippleToIn() override;   // 波纹编辑到入点 (可能会移动后续片段)
  void RippleToOut() override;  // 波纹编辑到出点
  void EditToIn() override;     // (标准) 编辑到入点 (不影响后续片段位置)
  void EditToOut() override;    // (标准) 编辑到出点

  void DeleteSelected() override; // 删除选中的片段
  void RippleDelete() override;   // 波纹删除选中的片段 (后续片段会前移以填充空隙)

  void IncreaseTrackHeight() override; // 增加时间轴轨道的显示高度
  void DecreaseTrackHeight() override; // 减少时间轴轨道的显示高度

  void ToggleLinks() override; // 切换选中片段的链接状态 (例如，音视频链接)

  void PasteInsert() override; // 粘贴并插入 (可能会推后后续片段)

  void DeleteInToOut() override; // 删除入点和出点之间的内容 (标准删除)
  void RippleDeleteInToOut() override; // 波纹删除入点和出点之间的内容

  void ToggleSelectedEnabled() override; // 切换选中片段的启用/禁用状态

  void SetColorLabel(int index) override; // 为选中的片段设置颜色标签

  void NudgeLeft() override;  // 将选中的片段向左微移一小段距离 (通常是一帧)
  void NudgeRight() override; // 将选中的片段向右微移

  void MoveInToPlayhead() override;  // 将选中片段的入点移动到播放头位置
  void MoveOutToPlayhead() override; // 将选中片段的出点移动到播放头位置

  void RenameSelected() override; // 重命名选中的片段 (通常是修改其标签)

  /**
   * @brief 为选中的片段添加默认的过渡效果。
   */
  void AddDefaultTransitionsToSelected() const { timeline_widget()->AddDefaultTransitionsToSelected(); }

  /**
   * @brief 为选中的片段显示速度/时长调整对话框。
   */
  void ShowSpeedDurationDialogForSelectedClips() const { timeline_widget()->ShowSpeedDurationDialogForSelectedClips(); }

  /**
   * @brief 将选中的片段嵌套成一个新的复合片段 (或序列)。
   */
  void NestSelectedClips() const { timeline_widget()->NestSelectedClips(); }

  /**
   * @brief 在播放头当前位置插入指定的素材片段 (作为新的 Block)。
   * @param footage 包含要插入的素材 (作为 ViewerOutput 指针) 的 QVector。
   */
  void InsertFootageAtPlayhead(const QVector<ViewerOutput *> &footage) const;

  /**
   * @brief 在播放头当前位置覆盖指定的素材片段。
   * @param footage 包含要覆盖的素材 (作为 ViewerOutput 指针) 的 QVector。
   */
  void OverwriteFootageAtPlayhead(const QVector<ViewerOutput *> &footage) const;

  /**
   * @brief 获取在时间轴上当前选中的所有 Block (片段) 对象。
   * @return 返回一个包含选中 Block 指针的 QVector 的常量引用。
   */
  [[nodiscard]] const QVector<Block *> &GetSelectedBlocks() const { return timeline_widget()->GetSelectedBlocks(); }

  /**
   * @brief 获取当前时间轴面板正在编辑的序列 (Sequence) 对象。
   * 通过将连接的 ViewerOutput 动态转换为 Sequence 来获取。
   * @return 返回 Sequence 指针，如果当前未连接到序列或转换失败，则返回 nullptr。
   */
  [[nodiscard]] Sequence *GetSequence() const { return dynamic_cast<Sequence *>(GetConnectedViewer()); }

 protected:
  /**
   * @brief 重写基类的 Retranslate 方法，用于在语言更改时更新此面板内UI元素的文本。
   */
  void Retranslate() override;

 signals: // Qt 信号声明
  /**
   * @brief 当时间轴上的 Block (片段) 选择状态发生改变时发出的信号。
   * @param selected_blocks 当前所有被选中的 Block 指针列表。
   */
  void BlockSelectionChanged(const QVector<Block *> &selected_blocks);

  /**
   * @brief 请求开始某种捕获或交互操作的信号 (具体含义参考 SequenceViewerPanel 中的同名槽)。
   * @param time 相关的时间范围。
   * @param track 相关的轨道引用。
   */
  void RequestCaptureStart(const TimeRange &time, const Track::Reference &track);

  /**
   * @brief 请求在项目浏览器 (ProjectPanel) 中显示指定的 ViewerOutput (通常是素材或序列)。
   * @param r 要显示的 ViewerOutput 指针。
   */
  void RevealViewerInProject(ViewerOutput *r);
  /**
   * @brief 请求在素材查看器 (FootageViewerPanel) 中显示指定的 ViewerOutput，并定位到特定时间范围。
   * @param r 要显示的 ViewerOutput 指针。
   * @param range 要在查看器中关注的时间范围。
   */
  void RevealViewerInFootageViewer(ViewerOutput *r, const TimeRange &range);
};

}  // namespace olive

#endif  // TIMELINE_PANEL_H