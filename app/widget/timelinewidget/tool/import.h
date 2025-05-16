#ifndef IMPORTTIMELINETOOL_H  // 防止头文件被多次包含的宏定义
#define IMPORTTIMELINETOOL_H

#include "tool.h"  // 引入 Tool 类的定义，ImportTool 是 TimelineTool 的派生类

// Qt类 (如 QVector, QPair, QDragLeaveEvent) 和项目特定类型 (如 ViewerOutput, rational, Track::Reference,
// MultiUndoCommand, TimelineViewGhostItem, TimeRange) 的定义应由 "tool.h"
// 或其传递包含的头文件提供，或者在使用这些类型的 .cpp 文件中包含。 此处严格按照用户提供的代码，不添加额外的 #include
// 或前向声明。

namespace olive {  // olive 命名空间开始

/**
 * @brief ImportTool 类是时间轴上的一个工具，专门用于处理素材的导入和拖放操作。
 *
 * 它继承自 TimelineTool，并重写了与拖放相关的事件处理函数，
 * 以实现在时间轴上通过拖拽方式放置素材的功能。
 */
class ImportTool : public TimelineTool {
 public:
  /**
   * @brief 构造一个 ImportTool 对象。
   * @param parent 指向其所属的 TimelineWidget 的指针。
   */
  explicit ImportTool(TimelineWidget *parent);

  /**
   * @brief 处理拖动进入事件。
   *
   * 当一个拖动操作进入此工具关联的视图时被调用。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含拖动事件的详细信息。
   */
  void DragEnter(TimelineViewMouseEvent *event) override;
  /**
   * @brief 处理拖动移动事件。
   *
   * 当一个拖动操作在工具关联的视图内移动时被调用。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void DragMove(TimelineViewMouseEvent *event) override;
  /**
   * @brief 处理拖动离开事件。
   *
   * 当一个拖动操作离开此工具关联的视图时被调用。
   * @param event 指向 QDragLeaveEvent 对象的指针，包含拖动离开事件的信息。
   */
  void DragLeave(QDragLeaveEvent *event) override;
  /**
   * @brief 处理拖放事件。
   *
   * 当一个拖动操作在工具关联的视图内释放（放下）时被调用。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含拖放事件的详细信息。
   */
  void DragDrop(TimelineViewMouseEvent *event) override;

  /**
   * @brief 定义用于存储拖拽素材数据及其目标轨道信息的类型别名。
   *
   * 它是一个 QVector，每个元素是一个 QPair。
   * QPair 的第一个元素是 ViewerOutput 指针 (代表一个素材源)。
   * QPair 的第二个元素是一个 QVector<Track::Reference> (代表该素材可以放置的目标轨道)。
   */
  using DraggedFootageData = QVector<QPair<ViewerOutput *, QVector<Track::Reference> > >;

  /**
   * @brief 将一组素材放置到时间轴的指定位置（重载版本1）。
   * @param footage 指向要放置的 ViewerOutput 对象指针的 QVector。
   * @param start 素材在时间轴上的起始放置时间 (rational)。
   * @param insert 如果为 true，则执行插入操作（会推移后续剪辑）；否则为覆盖操作。
   * @param command 指向 MultiUndoCommand 对象的指针，用于记录撤销/重做操作。
   * @param track_offset 放置时的轨道偏移量，默认为 0。
   * @param jump_to_end 操作完成后是否将播放头跳转到放置素材的末尾，默认为 false。
   */
  void PlaceAt(const QVector<ViewerOutput *> &footage, const rational &start, bool insert, MultiUndoCommand *command,
               int track_offset = 0, bool jump_to_end = false);
  /**
   * @brief 将一组已处理的拖拽素材数据放置到时间轴的指定位置（重载版本2）。
   * @param footage 包含素材及其目标轨道信息的 DraggedFootageData。
   * @param start 素材在时间轴上的起始放置时间 (rational)。
   * @param insert 如果为 true，则执行插入操作；否则为覆盖操作。
   * @param command 指向 MultiUndoCommand 对象的指针，用于记录撤销/重做操作。
   * @param track_offset 放置时的轨道偏移量，默认为 0。
   * @param jump_to_end 操作完成后是否将播放头跳转到放置素材的末尾，默认为 false。
   */
  void PlaceAt(const DraggedFootageData &footage, const rational &start, bool insert, MultiUndoCommand *command,
               int track_offset = 0, bool jump_to_end = false);

  /**
   * @brief DropWithoutSequenceBehavior 枚举定义了当没有序列存在时拖放操作的行为。
   */
  enum DropWithoutSequenceBehavior {
    kDWSAsk,     ///< 询问用户如何操作
    kDWSAuto,    ///< 自动创建序列并放置
    kDWSManual,  ///< 用户手动选择或创建序列
    kDWSDisable  ///< 禁止在没有序列的情况下拖放
  };

 private:  // 私有方法
  /**
   * @brief 根据拖拽的素材数据创建幽灵项（预览项）。
   * @param ghost_start 幽灵项在时间轴上的起始时间 (rational)。
   * @param sorted 已排序的拖拽素材数据 (DraggedFootageData)。
   * @param dest_tb 目标时间轴的时间基准 (rational)。
   * @param track_start 幽灵项开始放置的轨道索引。
   */
  void FootageToGhosts(rational ghost_start, const DraggedFootageData &sorted, const rational &dest_tb,
                       const int &track_start);

  /**
   * @brief 准备幽灵项，根据当前光标位置和轨道索引进行调整。
   * @param frame 当前光标对应的时间帧 (rational)。
   * @param track_index 当前光标所在的轨道索引。
   */
  void PrepGhosts(const rational &frame, const int &track_index);

  /**
   * @brief 将幽灵项（预览项）实际放置到时间轴上。
   * @param insert 如果为 true，则执行插入操作；否则为覆盖操作。
   * @param parent_command 指向父 MultiUndoCommand 对象的指针，用于将此操作作为子命令添加。
   */
  void DropGhosts(bool insert, MultiUndoCommand *parent_command);

  /**
   * @brief 创建一个幽灵项（预览项）。
   * @param range 幽灵项的时间范围 (TimeRange)。
   * @param media_in 幽灵项对应的媒体入点 (rational)。
   * @param track 幽灵项将要放置的轨道引用 (Track::Reference)。
   * @return 指向新创建的 TimelineViewGhostItem 对象的指针。
   */
  TimelineViewGhostItem *CreateGhost(const TimeRange &range, const rational &media_in, const Track::Reference &track);

  DraggedFootageData dragged_footage_;  ///< 存储当前正在拖拽的素材数据。

  int import_pre_buffer_;  ///< 导入素材时的预缓冲大小或相关设置。

  rational ghost_offset_;  ///< 幽灵项（预览项）的时间偏移量。
};

}  // namespace olive

#endif  // IMPORTTIMELINETOOL_H
