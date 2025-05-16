#ifndef POINTERTIMELINETOOL_H  // 防止头文件被多次包含的宏定义
#define POINTERTIMELINETOOL_H

#include "tool.h"  // 引入 Tool 类的定义，PointerTool 是 TimelineTool 的派生类

namespace olive {  // olive 命名空间开始

/**
 * @brief PointerTool 类是时间轴上的指针/选择工具。
 *
 * 它继承自 TimelineTool (或其派生类如 BeamTool，具体需查看 "tool.h")，
 * 负责处理时间轴上对象的选择、移动、修剪（trimming）等核心交互操作。
 * 它管理拖动状态、幽灵项（用于预览拖动效果）以及各种允许的操作模式。
 */
class PointerTool : public TimelineTool {
 public:
  /**
   * @brief 构造一个 PointerTool 对象。
   * @param parent 指向其所属的 TimelineWidget 的指针。
   */
  explicit PointerTool(TimelineWidget* parent);

  /**
   * @brief 处理鼠标按下事件。
   *
   * 重写自基类。通常用于开始选择、拖动或修剪操作。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void MousePress(TimelineViewMouseEvent* event) override;
  /**
   * @brief 处理鼠标移动事件。
   *
   * 重写自基类。在鼠标按下后移动时，用于更新拖动/修剪操作的预览。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void MouseMove(TimelineViewMouseEvent* event) override;
  /**
   * @brief 处理鼠标释放事件。
   *
   * 重写自基类。通常用于完成选择、拖动或修剪操作，并应用更改。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void MouseRelease(TimelineViewMouseEvent* event) override;

  /**
   * @brief 处理鼠标悬停移动事件。
   *
   * 重写自基类。当鼠标在时间轴视图中移动（未按下按钮）且此工具处于激活状态时调用，
   * 可能用于显示修剪手柄或更改光标样式。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void HoverMove(TimelineViewMouseEvent* event) override;

 protected:  // 受保护成员
  /**
   * @brief 完成拖动操作的虚函数。
   *
   * 在鼠标释放后调用，用于清理拖动状态并应用最终的更改。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  virtual void FinishDrag(TimelineViewMouseEvent* event);

  /**
   * @brief 初始化拖动操作的虚函数。
   * @param clicked_item 指向被点击的 Block 对象的指针，该对象将成为拖动操作的主体。
   * @param trim_mode 拖动开始时的修剪模式 (Timeline::MovementMode)。
   * @param modifiers 当前的键盘修饰键状态 (Qt::KeyboardModifiers)。
   */
  virtual void InitiateDrag(Block* clicked_item, Timeline::MovementMode trim_mode, Qt::KeyboardModifiers modifiers);

  /**
   * @brief 从给定的 Block 获取已存在的幽灵项 (TimelineViewGhostItem)。
   * @param block 指向 Block 对象的指针。
   * @return 如果存在与该 Block 关联的幽灵项，则返回其指针；否则返回 nullptr。
   */
  TimelineViewGhostItem* GetExistingGhostFromBlock(Block* block);

  /**
   * @brief 根据给定的 Block 创建并添加一个新的幽灵项。
   * @param block 指向 Block 对象的指针，幽灵项将基于此 Block 创建。
   * @param mode 幽灵项的移动/修剪模式 (Timeline::MovementMode)。
   * @param check_if_exists 是否在添加前检查是否已存在针对此 block 的幽灵项，默认为 false。
   * @return 指向新创建或已存在的 TimelineViewGhostItem 对象的指针。
   */
  TimelineViewGhostItem* AddGhostFromBlock(Block* block, Timeline::MovementMode mode, bool check_if_exists = false);

  /**
   * @brief 从空状态（非基于现有Block）创建一个幽灵项。
   * @param in 幽灵项的入点时间 (rational)。
   * @param out 幽灵项的出点时间 (rational)。
   * @param track 幽灵项所在的轨道引用 (Track::Reference)。
   * @param mode 幽灵项的移动/修剪模式 (Timeline::MovementMode)。
   * @return 指向新创建的 TimelineViewGhostItem 对象的指针。
   */
  TimelineViewGhostItem* AddGhostFromNull(const rational& in, const rational& out, const Track::Reference& track,
                                          Timeline::MovementMode mode);

  /**
   * @brief Validates Ghosts that are getting their in points trimmed
   * (原始英文注释：验证正在修剪其入点的幽灵项)
   *
   * Assumes ghost->data() is a Block. Ensures no Ghost's in point becomes a negative timecode. Also ensures no
   * Ghost's length becomes 0 or negative.
   * (原始英文注释：假设 ghost->data() 是一个
   * Block。确保没有幽灵项的入点变为负时间码。同时确保没有幽灵项的长度变为0或负数。)
   * @param movement 提议的入点移动量 (rational)。
   * @return 经过验证和调整后的实际允许移动量 (rational)。
   */
  rational ValidateInTrimming(rational movement);

  /**
   * @brief Validates Ghosts that are getting their out points trimmed
   * (原始英文注释：验证正在修剪其出点的幽灵项)
   *
   * Assumes ghost->data() is a Block. Ensures no Ghost's in point becomes a negative timecode. Also ensures no
   * Ghost's length becomes 0 or negative.
   * (原始英文注释：假设 ghost->data() 是一个
   * Block。确保没有幽灵项的入点变为负时间码。同时确保没有幽灵项的长度变为0或负数。)
   * @param movement 提议的出点移动量 (rational)。
   * @return 经过验证和调整后的实际允许移动量 (rational)。
   */
  rational ValidateOutTrimming(rational movement);

  /**
   * @brief 处理拖动过程中的逻辑的虚函数。
   *
   * 根据当前的鼠标位置更新拖动状态和幽灵项。
   * @param mouse_pos 当前鼠标在时间轴坐标系中的位置 (TimelineCoordinate)。
   */
  virtual void ProcessDrag(const TimelineCoordinate& mouse_pos);

  /**
   * @brief 内部初始化拖动操作的函数。
   * @param clicked_item 指向被点击的 Block 对象的指针。
   * @param trim_mode 拖动开始时的修剪模式。
   * @param modifiers 当前的键盘修饰键状态。
   * @param dont_roll_trims 是否禁止滚动修剪（rolling trims）。
   * @param allow_nongap_rolling 是否允许非间隙滚动（non-gap rolling）。
   * @param slide_instead_of_moving 是否执行滑动（slide）操作而非普通移动。
   */
  void InitiateDragInternal(Block* clicked_item, Timeline::MovementMode trim_mode, Qt::KeyboardModifiers modifiers,
                            bool dont_roll_trims, bool allow_nongap_rolling, bool slide_instead_of_moving);

  /**
   * @brief 获取当前的拖动移动模式。
   * @return 对当前拖动移动模式 (Timeline::MovementMode) 的常量引用。
   */
  [[nodiscard]] const Timeline::MovementMode& drag_movement_mode() const { return drag_movement_mode_; }
  /**
   * @brief 设置当前的拖动移动模式。
   * @param d 新的拖动移动模式 (Timeline::MovementMode)。
   */
  void set_drag_movement_mode(const Timeline::MovementMode& d) { drag_movement_mode_ = d; }

  /**
   * @brief 检查一个转场 (TransitionBlock) 是否可以与给定的剪辑集合一起移动。
   * @param transit 指向要检查的 TransitionBlock 的指针。
   * @param clips 一个包含相关 Block 指针的 QVector。
   * @return 如果转场可以移动，则返回 true；否则返回 false。
   */
  static bool CanTransitionMove(TransitionBlock* transit, const QVector<Block*>& clips);

  /**
   * @brief 设置是否允许对象移动。
   * @param e 如果为 true，则允许移动；否则禁止。
   */
  void SetMovementAllowed(bool e) { movement_allowed_ = e; }

  /**
   * @brief 设置是否允许修剪操作。
   * @param e 如果为 true，则允许修剪；否则禁止。
   */
  void SetTrimmingAllowed(bool e) { trimming_allowed_ = e; }

  /**
   * @brief 设置是否允许跨轨道移动。
   * @param e 如果为 true，则允许跨轨道移动；否则禁止。
   */
  void SetTrackMovementAllowed(bool e) { track_movement_allowed_ = e; }

  /**
   * @brief 设置是否允许修剪间隙 (gap)。
   * @param e 如果为 true，则允许修剪间隙；否则禁止。
   */
  void SetGapTrimmingAllowed(bool e) { gap_trimming_allowed_ = e; }

  /**
   * @brief 设置当前点击的条目。
   * @param b 指向被点击的 Block 对象的指针。
   */
  void SetClickedItem(Block* b) { clicked_item_ = b; }

 private:  // 私有方法
  /**
   * @brief 判断光标是否在给定 Block 的修剪手柄区域内。
   * @param block 指向要检查的 Block 对象的指针。
   * @param cursor_x 光标在场景中的 X 坐标 (qreal)。
   * @return 如果光标在修剪手柄内，则返回对应的修剪模式 (Timeline::MovementMode)；否则返回 Timeline::kMove。
   */
  Timeline::MovementMode IsCursorInTrimHandle(Block* block, qreal cursor_x);

  /**
   * @brief 内部方法，用于添加一个幽灵项并设置其模式。
   * @param ghost 指向要添加的 TimelineViewGhostItem 对象的指针。
   * @param mode 幽灵项的移动/修剪模式 (Timeline::MovementMode)。
   */
  void AddGhostInternal(TimelineViewGhostItem* ghost, Timeline::MovementMode mode);

  /**
   * @brief 检查一个剪辑 (Block) 是否可以根据给定的模式和上下文进行修剪。
   * @param clip 指向要检查的 Block 对象的指针。
   * @param items 一个包含相关 Block 指针的 QVector，代表上下文中的其他条目。
   * @param mode 当前的修剪模式 (Timeline::MovementMode)。
   * @return 如果剪辑可修剪，则返回 true；否则返回 false。
   */
  static bool IsClipTrimmable(Block* clip, const QVector<Block*>& items, const Timeline::MovementMode& mode);

  /**
   * @brief 处理用于滑动 (sliding) 操作的幽灵项。
   */
  void ProcessGhostsForSliding();

  /**
   * @brief 处理用于滚动修剪 (rolling) 操作的幽灵项。
   */
  void ProcessGhostsForRolling();

  bool movement_allowed_;        ///< 标记是否允许对象移动。
  bool trimming_allowed_;        ///< 标记是否允许修剪操作。
  bool track_movement_allowed_;  ///< 标记是否允许跨轨道移动。
  bool gap_trimming_allowed_;    ///< 标记是否允许修剪间隙。
  bool can_rubberband_select_;   ///< 标记是否可以开始橡皮筋选择。
  bool rubberband_selecting_;    ///< 标记当前是否正在进行橡皮筋选择。

  Track::Type drag_track_type_;                ///< 拖动操作涉及的轨道类型。
  Timeline::MovementMode drag_movement_mode_;  ///< 当前拖动操作的移动/修剪模式。

  Block* clicked_item_{};  ///< 指向当前鼠标按下的 Block 对象的指针，默认为 nullptr。

  QPoint drag_global_start_;  ///< 拖动开始时的全局鼠标坐标。
};

}  // namespace olive

#endif  // POINTERTIMELINETOOL_H
