#ifndef TIMELINEWIDGET_H // 防止头文件被多次包含的宏定义
#define TIMELINEWIDGET_H

#include <QRubberBand>  // 引入 QRubberBand 类，用于橡皮筋选择框
#include <QScrollBar>   // 引入 QScrollBar 类，用于滚动条功能
#include <QWidget>      // 引入 QWidget 类，是所有用户界面对象的基类
#include <utility>      // 引入 std::move 等工具函数

#include "core.h" // 引入核心定义，可能包含 rational 等类型
#include "node/block/transition/transition.h" // 引入 TransitionBlock 类的定义
#include "node/output/viewer/viewer.h"        // 引入 ViewerOutput 类的定义，通常代表一个可预览的节点输出
#include "node/project/serializer/serializer.h" // 引入序列化相关的类 ProjectSerializer
#include "timeline/timelinecommon.h"            // 引入时间轴相关的通用定义，如 Timeline::MovementMode, TimeRange
#include "timelineandtrackview.h"               // 引入 TimelineAndTrackView 类的定义，组合了轨道头部和时间轴主体
#include "widget/slider/rationalslider.h"       // 引入 RationalSlider 类的定义，用于显示和编辑 rational 类型数值的滑块
#include "widget/timebased/timebasedwidget.h"   // 引入 TimeBasedWidget 类的定义，是 TimelineWidget 的基类
#include "widget/timelinewidget/timelinewidgetselections.h" // 引入 TimelineWidgetSelections 类的定义，管理时间轴选择状态
#include "widget/timelinewidget/tool/import.h"  // 引入 ImportTool 类的定义
#include "widget/timelinewidget/tool/tool.h"    // 引入 TimelineTool 基类的定义

// 根据代码上下文，以下 Qt 类型应由已包含的头文件或其传递包含的头文件提供定义：
// - QPoint, QByteArray, QVector, QHash, QList, QMap, QTimer, QAction, QSplitter, QResizeEvent, QMouseEvent, QDragLeaveEvent, QDragEnterEvent, QDragMoveEvent, QDropEvent: 标准Qt类型
// 此处严格按照用户提供的代码，不添加额外的 #include 或前向声明。

namespace olive {

/**
 * @brief TimelineWidget 类是一个用于处理 TimelineOutput 节点（通常是序列）的完整控件。
 *
 * 它封装了 TimelineView（时间轴视图）、TimeRuler（时间标尺）和滚动条等组件，
 * 提供了一个完整的界面来操作时间轴上的内容。
 */
class TimelineWidget : public TimeBasedWidget {
  Q_OBJECT // Q_OBJECT 宏，用于启用 Qt 的元对象特性，如信号和槽

 public:
  /**
   * @brief 构造一个 TimelineWidget 对象。
   * @param parent 父 QWidget 对象，默认为 nullptr。
   */
  explicit TimelineWidget(QWidget* parent = nullptr);

  /**
   * @brief 析构 TimelineWidget 对象。
   *
   * 清理所有子工具和可能的其他资源。
   */
  ~TimelineWidget() override;

  /**
   * @brief 清除时间轴上的所有内容和状态。
   */
  void Clear();

  /**
   * @brief 选择时间轴上的所有可选项目。
   */
  void SelectAll();

  /**
   * @brief 取消选择时间轴上的所有项目。
   */
  void DeselectAll();

  /**
   * @brief 对选中的剪辑执行涟漪编辑，将其结束点对齐到入点标记。
   */
  void RippleToIn();

  /**
   * @brief 对选中的剪辑执行涟漪编辑，将其结束点对齐到出点标记。
   */
  void RippleToOut();

  /**
   * @brief 对选中的剪辑执行普通编辑，将其结束点对齐到入点标记（不影响后续剪辑）。
   */
  void EditToIn();

  /**
   * @brief 对选中的剪辑执行普通编辑，将其结束点对齐到出点标记（不影响后续剪辑）。
   */
  void EditToOut();

  /**
   * @brief 在当前播放头位置分割选中的剪辑。
   */
  void SplitAtPlayhead();

  /**
   * @brief 删除选中的项目。
   * @param ripple 如果为 true，则执行涟漪删除（后续剪辑会向前移动以填充间隙）。默认为 false。
   */
  void DeleteSelected(bool ripple = false);

  /**
   * @brief 增加所有轨道的高度。
   */
  void IncreaseTrackHeight();

  /**
   * @brief 减少所有轨道的高度。
   */
  void DecreaseTrackHeight();

  /**
   * @brief 在播放头位置插入指定的素材（插入模式）。
   * @param footage 一个包含 ViewerOutput 指针的 QVector，代表要插入的素材。
   */
  void InsertFootageAtPlayhead(const QVector<ViewerOutput*>& footage);

  /**
   * @brief 在播放头位置覆盖指定的素材（覆盖模式）。
   * @param footage 一个包含 ViewerOutput 指针的 QVector，代表要覆盖的素材。
   */
  void OverwriteFootageAtPlayhead(const QVector<ViewerOutput*>& footage);

  /**
   * @brief 切换选中剪辑的链接状态（例如，链接视频和音频部分）。
   */
  void ToggleLinksOnSelected() const;

  /**
   * @brief 为选中的剪辑（或剪辑间的编辑点）添加默认转场。
   */
  void AddDefaultTransitionsToSelected();

  /**
   * @brief 复制当前选中的内容 (重写自 TimeBasedWidget)。
   * @param cut 如果为 true，则执行剪切操作（复制并删除）；否则执行复制操作。
   * @return 如果操作成功，则返回 true；否则返回 false。
   */
  bool CopySelected(bool cut) override;

  /**
   * @brief 粘贴内容 (重写自 TimeBasedWidget)。
   * @return 如果操作成功，则返回 true；否则返回 false。
   */
  bool Paste() override;

  /**
   * @brief 执行插入式粘贴操作。
   */
  void PasteInsert();

  /**
   * @brief 删除从入点到出点范围内的内容。
   * @param ripple 如果为 true，则执行涟漪删除。
   */
  void DeleteInToOut(bool ripple);

  /**
   * @brief 切换选中剪辑的启用/禁用状态。
   */
  void ToggleSelectedEnabled() const;

  /**
   * @brief 为选中的项目设置颜色标签。
   * @param index 颜色标签的索引。
   */
  void SetColorLabel(int index);

  /**
   * @brief 将选中的项目向左微移一个单位（通常是一帧）。
   */
  void NudgeLeft();

  /**
   * @brief 将选中的项目向右微移一个单位（通常是一帧）。
   */
  void NudgeRight();

  /**
   * @brief 将选中剪辑的入点移动到当前播放头位置。
   */
  void MoveInToPlayhead();

  /**
   * @brief 将选中剪辑的出点移动到当前播放头位置。
   */
  void MoveOutToPlayhead();

  /**
   * @brief 为选中的剪辑显示速度/时长调整对话框。
   */
  void ShowSpeedDurationDialogForSelectedClips();

  /**
   * @brief 录制回调函数，在录制操作完成时被调用。
   * @param filename 录制产生的文件名。
   * @param time 录制的时间范围。
   * @param track 录制的目标轨道引用。
   */
  void RecordingCallback(const QString& filename, const TimeRange& time, const Track::Reference& track);

  /**
   * @brief 在指定的时间轴坐标启用录制叠加层显示。
   * @param coord 录制叠加层应显示的时间轴坐标。
   */
  void EnableRecordingOverlay(const TimelineCoordinate& coord);

  /**
   * @brief 禁用录制叠加层的显示。
   */
  void DisableRecordingOverlay();

  /**
   * @brief 添加一个临时的字幕轨道（用于预览或编辑）。
   */
  void AddTentativeSubtitleTrack();

  /**
   * @brief 将选中的剪辑嵌套为一个新的序列（或复合剪辑）。
   */
  void NestSelectedClips();

  /**
   * @brief Timelines should always be connected to sequences
   * (原始英文注释：时间轴应该总是连接到序列)
   * @return 指向当前连接的 Sequence 对象的指针。如果连接的节点不是 Sequence 类型，则返回 nullptr。
   */
  [[nodiscard]] Sequence* sequence() const { return dynamic_cast<Sequence*>(GetConnectedNode()); }

  /**
   * @brief 获取当前选中的所有 Block 对象的列表。
   * @return 对选中 Block 指针 QVector 的常量引用。
   */
  [[nodiscard]] const QVector<Block*>& GetSelectedBlocks() const { return selected_blocks_; }

  /**
   * @brief 保存分割条 (splitter) 的状态。
   * @return 包含分割条状态的 QByteArray。
   */
  [[nodiscard]] QByteArray SaveSplitterState() const;

  /**
   * @brief 恢复分割条 (splitter) 的状态。
   * @param state 包含分割条状态的 QByteArray。
   */
  void RestoreSplitterState(const QByteArray& state);

  /**
   * @brief 用间隙替换指定的 Block 列表。
   * @param blocks 要被替换的 Block 指针的 QVector。
   * @param remove_from_graph 是否同时从节点图中移除这些 Block。
   * @param command 指向 MultiUndoCommand 对象的指针，用于记录此操作。
   * @param handle_transitions 是否处理与被移除 Block 相关的转场，默认为 true。
   */
  static void ReplaceBlocksWithGaps(const QVector<Block*>& blocks, bool remove_from_graph, MultiUndoCommand* command,
                                    bool handle_transitions = true);

  /**
   * @brief Retrieve the QGraphicsItem at a particular scene position
   * (原始英文注释：获取特定场景位置的 QGraphicsItem)
   *
   * Requires a float-based scene position. If you have a screen position, use GetScenePos() first to convert it to a
   * scene position
   * (原始英文注释：需要一个基于浮点数的场景位置。如果您有一个屏幕位置，请先使用 GetScenePos() 将其转换为场景位置)
   * @param coord 要查询的时间轴坐标 (TimelineCoordinate)。
   * @return 如果找到，则返回指向 Block 对象的指针；否则返回 nullptr。
   */
  Block* GetItemAtScenePos(const TimelineCoordinate& coord);

  /**
   * @brief 将指定的时间范围和轨道添加到当前选择中。
   * @param time 要选择的时间范围 (TimeRange)。
   * @param track 要选择的轨道引用 (Track::Reference)。
   */
  void AddSelection(const TimeRange& time, const Track::Reference& track);
  /**
   * @brief 将指定的 Block 项添加到当前选择中。
   * @param item 指向要选择的 Block 对象的指针。
   */
  void AddSelection(Block* item);

  /**
   * @brief 从当前选择中移除指定的时间范围和轨道。
   * @param time 要取消选择的时间范围 (TimeRange)。
   * @param track 要取消选择的轨道引用 (Track::Reference)。
   */
  void RemoveSelection(const TimeRange& time, const Track::Reference& track);
  /**
   * @brief 从当前选择中移除指定的 Block 项。
   * @param item 指向要取消选择的 Block 对象的指针。
   */
  void RemoveSelection(Block* item);

  /**
   * @brief 获取当前的选择状态。
   * @return 对 TimelineWidgetSelections 对象的常量引用。
   */
  [[nodiscard]] const TimelineWidgetSelections& GetSelections() const { return selections_; }

  /**
   * @brief 设置当前的选择状态。
   * @param s 新的选择状态 (TimelineWidgetSelections)。
   * @param process_block_changes 是否在设置选择后处理 Block 相关的变化（例如更新选中 Block 列表）。
   */
  void SetSelections(const TimelineWidgetSelections& s, bool process_block_changes);

  /**
   * @brief 根据轨道引用获取 Track 对象。
   * @param ref 轨道引用 (Track::Reference)。
   * @return 如果找到，则返回指向 Track 对象的指针；否则返回 nullptr。
   */
  [[nodiscard]] Track* GetTrackFromReference(const Track::Reference& ref) const;

  /**
   * @brief 在内部的 TimelineView 上设置光束光标的位置。
   * @param coord 光束光标的时间轴坐标。
   */
  void SetViewBeamCursor(const TimelineCoordinate& coord);
  /**
   * @brief 在内部的 TimelineView 上设置转场叠加层的显示。
   * @param out 指向转场出点侧的 ClipBlock 对象的指针。
   * @param in 指向转场入点侧的 ClipBlock 对象的指针。
   */
  void SetViewTransitionOverlay(ClipBlock* out, ClipBlock* in);

  /**
   * @brief 获取当前活动的幽灵项列表。
   * @return 对 TimelineViewGhostItem 指针 QVector 的常量引用。
   */
  [[nodiscard]] const QVector<TimelineViewGhostItem*>& GetGhostItems() const { return ghost_items_; }

  /**
   * @brief 在指定的最早点和长度处插入间隙。
   * @param earliest_point 间隙开始的最早时间点 (rational)。
   * @param length 间隙的长度 (rational)。
   * @param command 指向 MultiUndoCommand 对象的指针，用于记录此操作。
   */
  void InsertGapsAt(const rational& earliest_point, const rational& length, MultiUndoCommand* command) const;

  /**
   * @brief 开始橡皮筋选择操作。
   * @param global_cursor_start 橡皮筋选择开始时的全局光标位置 (QPoint)。
   */
  void StartRubberBandSelect(const QPoint& global_cursor_start);
  /**
   * @brief 在橡皮筋选择过程中移动鼠标。
   * @param enable_selecting 是否实际执行选择操作（可能仅更新橡皮筋框）。
   * @param select_links 是否同时选择链接的项。
   */
  void MoveRubberBandSelect(bool enable_selecting, bool select_links);
  /**
   * @brief 结束橡皮筋选择操作。
   */
  void EndRubberBandSelect();

  /**
   * @brief 获取指定轨道引用的顶部 Y 坐标。
   * @param ref 轨道引用。
   * @return 轨道的 Y 坐标。
   */
  int GetTrackY(const Track::Reference& ref);
  /**
   * @brief 获取指定轨道引用的高度。
   * @param ref 轨道引用。
   * @return 轨道的高度。
   */
  int GetTrackHeight(const Track::Reference& ref);

  /**
   * @brief 添加一个幽灵项到内部列表。
   * @param ghost 指向要添加的 TimelineViewGhostItem 对象的指针。
   */
  void AddGhost(TimelineViewGhostItem* ghost);

  /**
   * @brief 清除所有活动的幽灵项。
   */
  void ClearGhosts();

  /**
   * @brief 检查当前是否存在活动的幽灵项。
   * @return 如果存在幽灵项，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool HasGhosts() const { return !ghost_items_.isEmpty(); }

  /**
   * @brief 检查指定的 Block 是否被选中。
   * @param b 指向要检查的 Block 对象的指针。
   * @return 如果 Block 被选中，则返回 true；否则返回 false。
   */
  bool IsBlockSelected(Block* b) const { return selected_blocks_.contains(b); }

  /**
   * @brief 设置指定 ClipBlock 的链接项的选择状态。
   * @param block 指向目标 ClipBlock 对象的指针。
   * @param selected true 表示选中链接项，false 表示取消选中。
   */
  void SetBlockLinksSelected(ClipBlock* block, bool selected);

  /**
   * @brief 将滚动操作加入队列（可能用于延迟执行或平滑滚动）。
   * @param value 目标滚动值。
   */
  void QueueScroll(int value);

  /**
   * @brief 获取第一个（通常是主要的）TimelineView 实例。
   * @return 指向 TimelineView 对象的指针。
   */
  TimelineView* GetFirstTimelineView();

  /**
   * @brief 获取指定轨道类型的时间基准。
   * @param type 轨道类型 (Track::Type)。
   * @return 对应的时间基准 (rational)。
   */
  rational GetTimebaseForTrackType(Track::Type type);

  /**
   * @brief 获取当前橡皮筋选择框的几何形状。
   * @return 橡皮筋选择框的 QRect。
   */
  [[nodiscard]] const QRect& GetRubberBandGeometry() const;

  /**
   * @brief Track blocks that have newly been selected (this is preferred over emitting BlocksSelected directly)
   * (原始英文注释：跟踪新选择的块（这比直接发出 BlocksSelected 更可取）)
   *
   * TimelineWidget keeps track of which blocks are selected internally. Calling this function will
   * add to that list and emit a signal to other widgets that said blocks have been selected.
   * (原始英文注释：TimelineWidget 内部跟踪哪些块被选中。调用此函数会将块添加到该列表，并向其他小部件发出信号，表明所述块已被选中。)
   *
   * @param input The list of blocks to add to the internal selection list and signal.
   * (原始英文注释：要添加到内部选择列表并发出信号的块列表。)
   *
   * @param filter TRUE to automatically filter blocks that are already selected from the list. In most cases,
   * this is preferable and should only be set to FALSE if the list is guaranteed not to contain
   * already selected blocks (and therefore filtering can be skipped to save time).
   * (原始英文注释：TRUE 表示自动从列表中过滤掉已选择的块。在大多数情况下，这是可取的，并且只有在列表保证不包含已选择的块时才应设置为 FALSE（因此可以跳过过滤以节省时间）。)
   */
  void SignalSelectedBlocks(QVector<Block*> input, bool filter = true);

  /**
   * @brief Track blocks that have been newly deselected
   * (原始英文注释：跟踪新取消选择的块)
   * @param deselected_blocks 一个包含被取消选择的 Block 指针的 QVector。
   */
  void SignalDeselectedBlocks(const QVector<Block*>& deselected_blocks);

  /**
   * @brief Convenience function to deselect all blocks and signal them
   * (原始英文注释：用于取消选择所有块并发出信号的便捷函数)
   */
  void SignalDeselectedAllBlocks();

  /**
   * @brief 刷新视图（更新视口）。
   */
  void Refresh() { UpdateViewports(); }

  /**
   * @brief 获取并清除与字幕显示相关的撤销命令。
   * @return 指向 MultiUndoCommand 对象的指针，如果不存在则为 nullptr。
   */
  MultiUndoCommand* TakeSubtitleSectionCommand() {
    // 复制指针
    MultiUndoCommand* c = subtitle_show_command_;

    // 设置为 null
    subtitle_show_command_ = nullptr;
    subtitle_tentative_track_ = nullptr;

    // 返回命令
    return c;
  }

  /**
   * @brief SetSelectionsCommand 类是一个用于撤销/重做设置时间轴选择状态的命令。
   */
  class SetSelectionsCommand : public UndoCommand {
   public:
    /**
     * @brief 构造一个 SetSelectionsCommand 对象。
     * @param timeline 指向相关的 TimelineWidget。
     * @param now 新的选择状态。
     * @param old 旧的选择状态。
     * @param process_block_changes 是否在应用选择时处理块相关的变化，默认为 true。
     */
    SetSelectionsCommand(TimelineWidget* timeline, TimelineWidgetSelections now, TimelineWidgetSelections old,
                         bool process_block_changes = true)
        : timeline_(timeline),                // 初始化时间轴控件指针
          old_(std::move(old)),               // 初始化旧的选择状态 (使用移动语义)
          now_(std::move(now)),               // 初始化新的选择状态 (使用移动语义)
          process_block_changes_(process_block_changes) {} // 初始化是否处理块变化

    /**
     * @brief 获取与此命令相关的 Project 对象。对于此命令，可能不直接关联项目，返回 nullptr。
     * @return 指向 Project 对象的指针，此处为 nullptr。
     */
    [[nodiscard]] Project* GetRelevantProject() const override { return nullptr; }

   protected:
    /**
     * @brief 执行重做操作，将时间轴的选择设置为新的状态。
     */
    void redo() override { timeline_->SetSelections(now_, process_block_changes_); }

    /**
     * @brief 执行撤销操作，将时间轴的选择恢复为旧的状态。
     */
    void undo() override { timeline_->SetSelections(old_, process_block_changes_); }

   private:
    TimelineWidget* timeline_; ///< 指向关联的 TimelineWidget。
    TimelineWidgetSelections old_; ///< 旧的选择状态。
    TimelineWidgetSelections now_; ///< 新的选择状态。
    bool process_block_changes_;   ///< 标记在设置选择时是否处理块相关的变化。
  };

 public slots: // 公共槽函数
  /**
   * @brief 清除临时的字幕轨道。
   */
  void ClearTentativeSubtitleTrack();

  /**
   * @brief 重命名当前选中的 Block。
   */
  void RenameSelectedBlocks();

 signals: // 信号
  /**
   * @brief 当 Block 的选择状态发生变化时发出此信号。
   * @param selected_blocks 一个包含当前所有被选中 Block 指针的 QVector。
   */
  void BlockSelectionChanged(const QVector<Block*>& selected_blocks);

  /**
   * @brief 请求开始捕获（录制）指定时间范围和轨道的信号。
   * @param time 要捕获的时间范围。
   * @param track 要捕获的目标轨道引用。
   */
  void RequestCaptureStart(const TimeRange& time, const Track::Reference& track);

  /**
   * @brief 请求在素材查看器中显示指定的 ViewerOutput 及其时间范围。
   * @param r 指向要显示的 ViewerOutput 对象的指针。
   * @param range 要显示的时间范围。
   */
  void RevealViewerInFootageViewer(ViewerOutput* r, const TimeRange& range);
  /**
   * @brief 请求在项目面板中显示指定的 ViewerOutput。
   * @param r 指向要显示的 ViewerOutput 对象的指针。
   */
  void RevealViewerInProject(ViewerOutput* r);

 protected: // 受保护的重写方法
  /**
   * @brief 重写 QWidget::resizeEvent()，处理控件大小调整事件。
   * @param event QResizeEvent 指针，包含事件参数。
   */
  void resizeEvent(QResizeEvent* event) override;

  /**
   * @brief 当播放头时间发生变化时调用的事件处理函数 (重写自 TimeBasedWidget)。
   * @param time 新的播放头时间 (rational)。 // 参数名根据上下文推断，原始代码中省略
   */
  void TimeChangedEvent(const rational&) override;
  /**
   * @brief 当时间基准发生变化时调用的事件处理函数 (重写自 TimeBasedWidget)。
   * @param timebase 新的时间基准 (rational)。 // 参数名根据上下文推断，原始代码中省略
   */
  void TimebaseChangedEvent(const rational&) override;
  /**
   * @brief 当（水平）缩放比例发生变化时调用的事件处理函数 (重写自 TimeBasedWidget)。
   * @param scale 新的缩放比例 (double)。 // 参数名根据上下文推断，原始代码中省略
   */
  void ScaleChangedEvent(const double&) override;

  /**
   * @brief 当连接到一个新的 ViewerOutput 节点时调用的事件处理函数 (重写自 TimeBasedWidget)。
   * @param n 指向被连接的 ViewerOutput 节点的指针。
   */
  void ConnectNodeEvent(ViewerOutput* n) override;
  /**
   * @brief 当从 ViewerOutput 节点断开连接时调用的事件处理函数 (重写自 TimeBasedWidget)。
   * @param n 指向被断开连接的 ViewerOutput 节点的指针。
   */
  void DisconnectNodeEvent(ViewerOutput* n) override;

  /**
   * @brief 获取用于吸附的 Block 对象列表 (重写自 TimeBasedWidget)。
   * @return 指向已添加 Block 指针 QVector 的常量指针。
   */
  [[nodiscard]] const QVector<Block*>* GetSnapBlocks() const override { return &added_blocks_; }

 protected slots: // 受保护槽函数
  /**
   * @brief 发送“追赶”滚动事件的虚槽函数 (重写自 TimeBasedWidget)。
   */
  void SendCatchUpScrollEvent() override;

 private: // 私有方法
  /**
   * @brief 获取在涟漪/编辑到入点/出点操作中受影响的剪辑信息。
   * @param playhead_time 当前播放头时间。
   * @param mode 操作模式 (入点或出点)。
   * @return 包含受影响剪辑信息的 QVector<Timeline::EditToInfo>。
   */
  [[nodiscard]] QVector<Timeline::EditToInfo> GetEditToInfo(const rational& playhead_time,
                                                            Timeline::MovementMode mode) const;

  /**
   * @brief 执行涟漪到入点/出点操作。
   * @param mode 指定是涟漪到入点还是出点。
   */
  void RippleTo(Timeline::MovementMode mode);

  /**
   * @brief 执行编辑到入点/出点操作（非涟漪）。
   * @param mode 指定是编辑到入点还是出点。
   */
  void EditTo(Timeline::MovementMode mode);

  /**
   * @brief 更新所有视图的视口。
   * @param type 可选参数，指定仅更新特定类型的轨道视图，默认为 Track::kNone (更新所有)。
   */
  void UpdateViewports(const Track::Type& type = Track::kNone);

  /**
   * @brief 内部粘贴逻辑。
   * @param insert 如果为 true，则执行插入式粘贴；否则为覆盖粘贴。
   * @return 如果操作成功，则返回 true。
   */
  bool PasteInternal(bool insert);

  /**
   * @brief 添加一个新的 TimelineAndTrackView 实例到布局中。
   * @param alignment 新视图的垂直对齐方式。
   * @return 指向新创建的 TimelineAndTrackView 对象的指针。
   */
  [[nodiscard]] TimelineAndTrackView* AddTimelineAndTrackView(Qt::Alignment alignment) const;

  /**
   * @brief 为粘贴操作生成已存在的节点到新复制节点的映射。
   * @param r ProjectSerializer 的结果，包含复制的节点信息。
   * @return 一个 QHash，键是原始 Node 指针，值是新复制的 Node 指针。
   */
  QHash<Node*, Node*> GenerateExistingPasteMap(const ProjectSerializer::Result& r);

  QRubberBand rubberband_; ///< 用于橡皮筋选择的 QRubberBand 对象。
  QVector<QPointF> rubberband_scene_pos_; ///< 存储橡皮筋选择过程中鼠标在场景中的位置点。
  TimelineWidgetSelections rubberband_old_selections_; ///< 开始橡皮筋选择前的旧选择状态。
  QVector<Block*> rubberband_now_selected_; ///< 当前通过橡皮筋选择的 Block 列表。
  bool rubberband_enable_selecting_{}; ///< 标记橡皮筋选择是否实际更新选择集。
  bool rubberband_select_links_{};   ///< 标记橡皮筋选择是否同时选择链接的项。

  TimelineWidgetSelections selections_; ///< 当前时间轴的选择状态。

  /**
   * @brief 获取当前激活的时间轴工具。
   * @return 指向当前活动 TimelineTool 对象的指针。
   */
  TimelineTool* GetActiveTool();

  QVector<TimelineTool*> tools_; ///< 存储所有可用时间轴工具的列表。

  ImportTool* import_tool_; ///< 指向导入工具的特定指针。

  TimelineTool* active_tool_; ///< 指向当前激活的时间轴工具的指针。

  QVector<TimelineViewGhostItem*> ghost_items_; ///< 存储当前所有幽灵项（拖动预览）的列表。

  QVector<TimelineAndTrackView*> views_; ///< 存储此 TimelineWidget 管理的所有 TimelineAndTrackView 实例。

  RationalSlider* timecode_label_; ///< 指向 RationalSlider 对象的指针，可能用于显示当前时间码或进行精确跳转。

  QVector<Block*> selected_blocks_; ///< 存储当前所有被选中 Block 的列表。

  QVector<Block*> added_blocks_; ///< 存储已添加到时间轴上的 Block 列表（可能用于吸附等）。

  int deferred_scroll_value_{}; ///< 延迟滚动的目标值。

  bool use_audio_time_units_; ///< 标记当前是否使用音频时间单位（例如采样数）而非标准时间码。

  QSplitter* view_splitter_; ///< 指向 QSplitter 对象的指针，用于分隔不同的视图区域。

  MultiUndoCommand* subtitle_show_command_; ///< 指向与显示/隐藏字幕相关的聚合撤销命令。
  Track* subtitle_tentative_track_; ///< 指向临时的字幕轨道。

  QTimer* signal_block_change_timer_; ///< 用于延迟发送 Block选择变化信号的 QTimer。

  /**
   * @brief SetSplitterSizesCommand 类是一个用于撤销/重做设置分割条大小的命令。
   */
  class SetSplitterSizesCommand : public UndoCommand {
   public:
    /**
     * @brief 构造一个 SetSplitterSizesCommand 对象。
     * @param splitter 指向目标 QSplitter。
     * @param sizes 新的大小列表。
     */
    SetSplitterSizesCommand(QSplitter* splitter, const QList<int>& sizes) : splitter_(splitter), new_sizes_(sizes) {}

    /**
     * @brief 获取与此命令相关的 Project 对象。对于此命令，不直接关联项目，返回 nullptr。
     * @return 指向 Project 对象的指针，此处为 nullptr。
     */
    [[nodiscard]] Project* GetRelevantProject() const override { return nullptr; }

   protected:
    /**
     * @brief 执行重做操作，将分割条的大小设置为新值。
     */
    void redo() override;
    /**
     * @brief 执行撤销操作，将分割条的大小恢复为旧值。
     */
    void undo() override;

   private:
    QSplitter* splitter_;      ///< 指向被操作的 QSplitter。
    QList<int> new_sizes_;     ///< 新的分割条大小列表。
    QList<int> old_sizes_;     ///< 旧的分割条大小列表（在第一次 redo 时捕获）。
  };

  /**
   * @brief 将视图中心滚动到指定的场景位置。
   * @param scene_pos 目标场景 X 坐标。
   */
  void CenterOn(qreal scene_pos);

  /**
   * @brief 更新所有关联视图的时间基准。
   */
  void UpdateViewTimebases();

  /**
   * @brief 内部处理微移操作的逻辑。
   * @param amount 微移的时间量。
   */
  void NudgeInternal(rational amount);

  /**
   * @brief 内部处理将选中剪辑的入点或出点移动到播放头位置的逻辑。
   * @param out 如果为 true，则移动出点；否则移动入点。
   */
  void MoveToPlayheadInternal(bool out);

 private slots: // 私有槽函数
  /**
   * @brief 处理来自 TimelineView 的鼠标按下信号。
   * @param event 鼠标事件。
   */
  void ViewMousePressed(TimelineViewMouseEvent* event);
  /**
   * @brief 处理来自 TimelineView 的鼠标移动信号。
   * @param event 鼠标事件。
   */
  void ViewMouseMoved(TimelineViewMouseEvent* event);
  /**
   * @brief 处理来自 TimelineView 的鼠标释放信号。
   * @param event 鼠标事件。
   */
  void ViewMouseReleased(TimelineViewMouseEvent* event);
  /**
   * @brief 处理来自 TimelineView 的鼠标双击信号。
   * @param event 鼠标事件。
   */
  void ViewMouseDoubleClicked(TimelineViewMouseEvent* event);

  /**
   * @brief 处理来自 TimelineView 的拖动进入信号。
   * @param event 拖动事件。
   */
  void ViewDragEntered(TimelineViewMouseEvent* event);
  /**
   * @brief 处理来自 TimelineView 的拖动移动信号。
   * @param event 拖动事件。
   */
  void ViewDragMoved(TimelineViewMouseEvent* event);
  /**
   * @brief 处理来自 TimelineView 的拖动离开信号。
   * @param event 拖动离开事件。
   */
  void ViewDragLeft(QDragLeaveEvent* event);
  /**
   * @brief 处理来自 TimelineView 的拖放信号。
   * @param event 拖放事件。
   */
  void ViewDragDropped(TimelineViewMouseEvent* event);

  /**
   * @brief 当一个 Block 被添加到序列时调用的槽函数。
   * @param block 指向被添加的 Block 对象的指针。
   */
  void AddBlock(Block* block);
  /**
   * @brief 当一个 Block 从序列中移除时调用的槽函数。
   * @param block 指向被移除的 Block 对象的指针。 // 参数名应为 block，与 QVector<Block*> GetSelectedBlocks() 等一致
   */
  void RemoveBlock(Block* block); // 参数类型应为 Block*，不是 Block* blocks

  /**
   * @brief 当一个轨道被添加到序列时调用的槽函数。
   * @param track 指向被添加的 Track 对象的指针。
   */
  void AddTrack(Track* track);
  /**
   * @brief 当一个轨道从序列中移除时调用的槽函数。
   * @param track 指向被移除的 Track 对象的指针。
   */
  void RemoveTrack(Track* track);
  /**
   * @brief 当轨道信息更新时调用的槽函数。
   */
  void TrackUpdated();

  /**
   * @brief 当 Block 信息更新时调用的槽函数。
   */
  void BlockUpdated();

  /**
   * @brief 更新水平分割条的状态。
   */
  void UpdateHorizontalSplitters();

  /**
   * @brief 根据分割条的位置更新时间码标签的宽度。
   * @param s 指向相关的 QSplitter 对象的指针。
   */
  void UpdateTimecodeWidthFromSplitters(QSplitter* s);

  /**
   * @brief 显示上下文菜单。
   */
  void ShowContextMenu();

  /**
   * @brief 执行延迟的滚动操作。
   */
  void DeferredScrollAction();

  /**
   * @brief 显示序列设置对话框。
   */
  void ShowSequenceDialog();

  /**
   * @brief 设置是否使用音频时间单位。
   * @param use 如果为 true，则使用音频时间单位；否则使用标准时间单位。
   */
  void SetUseAudioTimeUnits(bool use);

  /**
   * @brief 当活动工具发生变化时调用的槽函数。
   */
  void ToolChanged();

  /**
   * @brief 当可添加对象（例如从媒体池拖动的素材）发生变化时调用的槽函数。
   */
  void AddableObjectChanged();

  /**
   * @brief 设置视图是否显示音频波形。
   * @param e 如果为 true，则显示波形。
   */
  void SetViewWaveformsEnabled(bool e);

  /**
   * @brief 设置视图是否显示视频缩略图。
   * @param action 指向触发此操作的 QAction 的指针。
   */
  void SetViewThumbnailsEnabled(QAction* action);

  /**
   * @brief 当连接的 ViewerOutput 节点的帧率发生变化时调用的槽函数。
   */
  void FrameRateChanged();

  /**
   * @brief 当连接的 ViewerOutput 节点的采样率发生变化时调用的槽函数。
   */
  void SampleRateChanged();

  /**
   * @brief 当轨道的索引发生变化时调用的槽函数。
   * @param old_index 旧的轨道索引。
   * @param new_index 新的轨道索引。
   */
  void TrackIndexChanged(int old_index, int new_index); // 参数名修正为 old_index, new_index

  /**
   * @brief 发送 Block 选择状态变化的信号。
   */
  void SignalBlockSelectionChange();

  /**
   * @brief 请求在素材查看器中显示当前选中的剪辑。
   */
  void RevealInFootageViewer();
  /**
   * @brief 请求在项目面板中显示当前选中的剪辑。
   */
  void RevealInProject();

  /**
   * @brief 当一个轨道即将被删除时调用的槽函数。
   * @param track 指向即将被删除的 Track 对象的指针。
   */
  void TrackAboutToBeDeleted(Track* track);

  /**
   * @brief 设置选中剪辑的自动缓存状态。
   * @param e 如果为 true，则启用自动缓存。
   */
  void SetSelectedClipsAutocaching(bool e);

  /**
   * @brief 缓存选中的剪辑。
   */
  void CacheClips();
  /**
   * @brief 缓存选中剪辑的入点到出点范围。
   */
  void CacheClipsInOut();
  /**
   * @brief 丢弃选中剪辑的缓存。
   */
  void CacheDiscard();

  /**
   * @brief 当多机位编辑模式启用/禁用状态改变时触发的槽函数。
   * @param e 如果为 true，则多机位编辑模式启用。
   */
  void MulticamEnabledTriggered(bool e);

  /**
   * @brief 强制更新橡皮筋选择的状态。
   */
  void ForceUpdateRubberBand();
};

}  // namespace olive

#endif  // TIMELINEWIDGET_H
