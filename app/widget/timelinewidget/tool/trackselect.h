#ifndef TRACKSELECTTOOL_H // 防止头文件被多次包含的宏定义
#define TRACKSELECTTOOL_H

#include "pointer.h" // 引入 PointerTool 类的定义，TrackSelectTool 是 PointerTool 的派生类

namespace olive { // olive 命名空间开始

/**
 * @brief TrackSelectTool 类是时间轴上的轨道选择工具。
 *
 * 它继承自 PointerTool，专门用于通过单击轨道上的某个点来选择该点之后（或之前，取决于点击位置和修饰键）
 * 同一轨道上的所有剪辑（Blocks）。
 */
class TrackSelectTool : public PointerTool {
public:
  /**
   * @brief 构造一个 TrackSelectTool 对象。
   * @param parent 指向其所属的 TimelineWidget 的指针。
   */
  explicit TrackSelectTool(TimelineWidget *parent);

  /**
   * @brief 处理鼠标按下事件。
   *
   * 重写自基类 (PointerTool)。当用户使用轨道选择工具在时间轴上点击鼠标时调用。
   * 此方法将确定被点击的轨道和位置，并调用 SelectBlocksOnTrack 来选择相应的剪辑。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void MousePress(TimelineViewMouseEvent *event) override;

private: // 私有方法
  /**
   * @brief 在指定轨道上选择从某个点开始向前或向后的所有剪辑。
   * @param track 指向目标 Track 对象的指针。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，用于获取点击位置等信息。
   * @param blocks 指向 QVector<Block *> 的指针，用于存储被选中的剪辑。
   * @param forward 如果为 true，则选择点击点之后的所有剪辑；如果为 false，则选择点击点之前的所有剪辑。
   */
  void SelectBlocksOnTrack(Track *track, TimelineViewMouseEvent *event, QVector<Block *> *blocks, bool forward);
};

}  // namespace olive

#endif  // TRACKSELECTTOOL_H
