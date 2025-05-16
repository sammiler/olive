#ifndef RESIZABLETIMELINESCROLLBAR_H
#define RESIZABLETIMELINESCROLLBAR_H

#include <QPaintEvent>  // Qt 绘制事件类 (用于 paintEvent)
#include <QScrollBar>   // Qt 滚动条控件基类 (ResizableScrollBar 的基类)
#include <QWidget>      // Qt 控件基类 (ResizableScrollBar 的基类 QScrollBar 的基类 QAbstractSlider 的基类 QWidget)

#include "resizablescrollbar.h"                 // 可调整大小的滚动条基类
#include "timeline/timelinemarker.h"            // 时间轴标记列表类 (TimelineMarkerList)
#include "timeline/timelineworkarea.h"          // 时间轴工作区类
#include "widget/timebased/timescaledobject.h"  // 基于时间缩放的对象接口

// 前向声明项目内部类 (根据用户要求，不添加)
// class TimelineMarkerList; // 已包含 timeline/timelinemarker.h
// class TimelineWorkArea;   // 已包含 timeline/timelineworkarea.h
// class rational;           // TimeScaledObject 可能使用，或在包含的头文件中

namespace olive {

/**
 * @brief ResizableTimelineScrollBar 类是一个为时间轴设计的可调整大小的滚动条。
 *
 * 它继承自 ResizableScrollBar (允许用户调整滚动条的厚度) 和 TimeScaledObject (使其能够感知时间缩放)。
 * 此滚动条能够连接到时间轴标记 (TimelineMarkerList) 和工作区 (TimelineWorkArea)，
 * 并在其表面上绘制这些标记以及工作区的范围，为用户提供时间轴的概览和导航辅助。
 */
class ResizableTimelineScrollBar : public ResizableScrollBar, public TimeScaledObject {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit ResizableTimelineScrollBar(QWidget* parent = nullptr);
  /**
   * @brief 构造函数，允许指定滚动条的方向。
   * @param orientation 滚动条的方向 (Qt::Horizontal 或 Qt::Vertical)。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit ResizableTimelineScrollBar(Qt::Orientation orientation, QWidget* parent = nullptr);

  /**
   * @brief 连接时间轴标记列表到此滚动条。
   *
   * 连接后，滚动条会在其上绘制这些标记。
   * @param markers 指向 TimelineMarkerList 对象的指针。
   */
  void ConnectMarkers(TimelineMarkerList* markers);
  /**
   * @brief 连接时间轴工作区到此滚动条。
   *
   * 连接后，滚动条可能会在其上绘制工作区的范围。
   * @param workarea 指向 TimelineWorkArea 对象的指针。
   */
  void ConnectWorkArea(TimelineWorkArea* workarea);

  /**
   * @brief 设置滚动条的时间缩放比例。
   *
   * 此函数会影响标记和工作区范围在滚动条上的显示方式。
   * @param d 新的缩放比例值。
   */
  void SetScale(double d);

 protected:
  /**
   * @brief 重写 QWidget 的绘制事件处理函数。
   *
   * 用于自定义滚动条的绘制，例如在其上绘制时间轴标记和工作区范围。
   * @param event 绘制事件指针。
   */
  void paintEvent(QPaintEvent* event) override;

 private:
  TimelineMarkerList* markers_;  ///< 指向关联的时间轴标记列表对象的指针。

  TimelineWorkArea* workarea_;  ///< 指向关联的时间轴工作区对象的指针。

  double scale_;  ///< 当前的时间缩放比例。
};

}  // namespace olive

#endif  // RESIZABLETIMELINESCROLLBAR_H
