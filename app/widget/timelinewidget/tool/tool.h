#ifndef TIMELINETOOL_H  // 防止头文件被多次包含的宏定义
#define TIMELINETOOL_H

#include <QDragLeaveEvent>  // 引入 QDragLeaveEvent 类，用于处理拖动离开事件

#include "widget/timelinewidget/view/timelineviewghostitem.h"  // 引入 TimelineViewGhostItem 类，用于在时间轴视图中显示拖动预览的“幽灵”项
#include "widget/timelinewidget/view/timelineviewmouseevent.h"  // 引入 TimelineViewMouseEvent 类，封装了时间轴视图中的鼠标事件信息

// 根据代码上下文，以下类型应由已包含的头文件或其传递包含的头文件提供定义：
// - olive::TimelineWidget: 用于构造函数参数和成员变量
// - olive::Sequence: 用于 sequence() 方法的返回值类型
// - olive::Timeline::MovementMode: 用于 FlipTrimMode 方法参数和返回值类型 (Timeline 命名空间下的枚举)
// - olive::rational: 用于 SnapMovementToTimebase 和 GetGhostData 方法参数及返回值类型，以及 snap_points_ 成员变量类型
// - olive::TimelineCoordinate: 用于 drag_start_ 成员变量类型
// - olive::MultiUndoCommand: 用于 InsertGapsAtGhostDestination 方法参数
// - QVector: Qt 容器类，用于 ValidateTrackMovement 方法参数
// 此处严格按照用户提供的代码，不添加额外的 #include 或前向声明。

namespace olive {  // olive 命名空间开始

class TimelineWidget;    // 前向声明 TimelineWidget 类，表示时间轴主控件
class Sequence;          // 前向声明 Sequence 类，表示一个序列
class MultiUndoCommand;  // 前向声明 MultiUndoCommand 类，用于聚合多个撤销命令

/**
 * @brief TimelineTool 类是所有时间轴工具的抽象基类。
 *
 * 它定义了时间轴工具应具有的通用接口，例如处理各种鼠标事件
 * (按下、移动、释放、双击、悬停) 和拖放事件 (进入、移动、离开、放下)。
 * 派生类将实现特定工具的行为。
 */
class TimelineTool {
 public:
  /**
   * @brief 构造一个 TimelineTool 对象。
   * @param parent 指向其所属的 TimelineWidget 的指针。
   */
  explicit TimelineTool(TimelineWidget *parent);
  /**
   * @brief 虚析构函数，确保派生类的正确析构。
   */
  virtual ~TimelineTool();

  /**
   * @brief 处理鼠标按下事件的虚函数。派生类应重写此方法以实现具体行为。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。默认为空实现。
   */
  virtual void MousePress(TimelineViewMouseEvent *) {}
  /**
   * @brief 处理鼠标移动事件的虚函数。派生类应重写此方法以实现具体行为。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。默认为空实现。
   */
  virtual void MouseMove(TimelineViewMouseEvent *) {}
  /**
   * @brief 处理鼠标释放事件的虚函数。派生类应重写此方法以实现具体行为。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。默认为空实现。
   */
  virtual void MouseRelease(TimelineViewMouseEvent *) {}
  /**
   * @brief 处理鼠标双击事件的虚函数。派生类应重写此方法以实现具体行为。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。默认为空实现。
   */
  virtual void MouseDoubleClick(TimelineViewMouseEvent *) {}

  /**
   * @brief 处理鼠标悬停移动事件的虚函数。派生类应重写此方法以实现具体行为。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。默认为空实现。
   */
  virtual void HoverMove(TimelineViewMouseEvent *) {}

  /**
   * @brief 处理拖动进入事件的虚函数。派生类应重写此方法以实现具体行为。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含拖动事件的详细信息。默认为空实现。
   */
  virtual void DragEnter(TimelineViewMouseEvent *) {}
  /**
   * @brief 处理拖动移动事件的虚函数。派生类应重写此方法以实现具体行为。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含拖动事件的详细信息。默认为空实现。
   */
  virtual void DragMove(TimelineViewMouseEvent *) {}
  /**
   * @brief 处理拖动离开事件的虚函数。派生类应重写此方法以实现具体行为。
   * @param event 指向 QDragLeaveEvent 对象的指针，包含拖动离开事件的信息。默认为空实现。
   */
  virtual void DragLeave(QDragLeaveEvent *) {}
  /**
   * @brief 处理拖放事件的虚函数。派生类应重写此方法以实现具体行为。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含拖放事件的详细信息。默认为空实现。
   */
  virtual void DragDrop(TimelineViewMouseEvent *) {}

  /**
   * @brief 获取此工具所属的 TimelineWidget 父控件。
   * @return 指向 TimelineWidget 对象的指针。
   */
  TimelineWidget *parent();

  /**
   * @brief 获取当前活动的序列。
   * @return 指向 Sequence 对象的指针。
   */
  Sequence *sequence();

  /**
   * @brief 翻转给定的修剪模式。
   * 例如，将入点修剪模式 (kTrimIn) 翻转为出点修剪模式 (kTrimOut)，反之亦然。
   * @param trim_mode 要翻转的修剪模式 (Timeline::MovementMode)。
   * @return 翻转后的修剪模式 (Timeline::MovementMode)。
   */
  static Timeline::MovementMode FlipTrimMode(const Timeline::MovementMode &trim_mode);

  /**
   * @brief 将移动量吸附到时间基准的整数倍。
   * @param start 起始时间点 (rational)。
   * @param movement 原始的移动量 (rational)。
   * @param timebase 时间基准 (rational)，例如帧时长。
   * @return 吸附到时间基准后的移动量 (rational)。
   */
  static rational SnapMovementToTimebase(const rational &start, rational movement, const rational &timebase);

 protected:  // 受保护成员
  /**
   * @brief Validates Ghosts that are moving horizontally (time-based)
   * (原始英文注释：验证水平（基于时间）移动的幽灵项)
   *
   * Validation is the process of ensuring that whatever movements the user is making are "valid" and "legal". This
   * function's validation ensures that no Ghost's in point ends up in a negative timecode.
   * (原始英文注释：验证是确保用户所做的任何移动都是“有效”和“合法”的过程。此函数的验证确保没有幽灵项的入点最终为负时间码。)
   * @param movement 提议的时间移动量 (rational)。
   * @return 经过验证和调整后的实际允许时间移动量 (rational)。
   */
  rational ValidateTimeMovement(rational movement);

  /**
   * @brief Validates Ghosts that are moving vertically (track-based)
   * (原始英文注释：验证垂直（基于轨道）移动的幽灵项)
   *
   * This function's validation ensures that no Ghost's track ends up in a negative (non-existent) track.
   * (原始英文注释：此函数的验证确保没有幽灵项的轨道最终为负数（不存在的轨道）。)
   * @param movement 提议的轨道移动量 (int)，通常是向上或向下移动的轨道数。
   * @param ghosts 一个包含当前正在拖动的 TimelineViewGhostItem 指针的 QVector。
   * @return 经过验证和调整后的实际允许轨道移动量 (int)。
   */
  static int ValidateTrackMovement(int movement, const QVector<TimelineViewGhostItem *> &ghosts);

  /**
   * @brief 获取当前所有幽灵项中最早和最晚的时间点。
   * @param earliest_point 用于存储最早时间点的 rational 指针 (输出参数)。
   * @param latest_point 用于存储最晚时间点的 rational 指针 (输出参数)。
   */
  void GetGhostData(rational *earliest_point, rational *latest_point);

  /**
   * @brief 在幽灵项的目标位置插入间隙。
   *
   * 当执行插入式拖动操作时，可能需要此方法来推移现有剪辑。
   * @param command 指向 MultiUndoCommand 对象的指针，用于记录此操作以便撤销/重做。
   */
  void InsertGapsAtGhostDestination(MultiUndoCommand *command);

  std::vector<rational> snap_points_;  ///< 存储用于吸附操作的时间点列表。

  bool dragging_;  ///< 标记当前是否正在进行拖动操作。

  TimelineCoordinate drag_start_;  ///< 记录拖动操作开始时的时间轴坐标。

  static const int kDefaultDistanceFromOutput;  ///< 可能是一个默认距离常量，用于某些输出相关的计算。

 private:                   // 私有成员
  TimelineWidget *parent_;  ///< 指向此工具所属的 TimelineWidget 父控件的指针。
};

}  // namespace olive

#endif  // TIMELINETOOL_H
