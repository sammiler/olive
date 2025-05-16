#ifndef RAZORTIMELINETOOL_H  // 防止头文件被多次包含的宏定义
#define RAZORTIMELINETOOL_H

#include "beam.h"  // 引入 BeamTool 类的定义，RazorTool 是 BeamTool 的派生类

namespace olive {  // olive 命名空间开始

/**
 * @brief RazorTool 类是时间轴上的剃刀工具，用于在时间轴上切割剪辑。
 *
 * 它继承自 BeamTool，这可能表明它利用了 BeamTool 的一些与光束或位置指示相关的特性。
 * RazorTool 主要通过处理鼠标事件来实现点击位置的剪辑分割功能。
 */
class RazorTool : public BeamTool {
 public:
  /**
   * @brief 构造一个 RazorTool 对象。
   * @param parent 指向其所属的 TimelineWidget 的指针。
   */
  explicit RazorTool(TimelineWidget *parent);

  /**
   * @brief 处理鼠标按下事件。
   *
   * 重写自基类 (BeamTool)。当用户使用剃刀工具在时间轴上点击时调用，
   * 通常用于确定切割点。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void MousePress(TimelineViewMouseEvent *event) override;
  /**
   * @brief 处理鼠标移动事件。
   *
   * 重写自基类 (BeamTool)。当用户在使用剃刀工具时移动鼠标，
   * 此方法可能会更新切割点的预览。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void MouseMove(TimelineViewMouseEvent *event) override;
  /**
   * @brief 处理鼠标释放事件。
   *
   * 重写自基类 (BeamTool)。当用户释放鼠标按钮时调用，
   * 此时可能会执行实际的切割操作。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含鼠标事件的详细信息。
   */
  void MouseRelease(TimelineViewMouseEvent *event) override;

 private:                                   // 私有成员
  QVector<Track::Reference> split_tracks_;  ///< 存储在切割操作中涉及的轨道引用。
};

}  // namespace olive

#endif  // RAZORTIMELINETOOL_H
