#ifndef TIMELINEANDTRACKVIEW_H  // 防止头文件被多次包含的宏定义
#define TIMELINEANDTRACKVIEW_H

#include <QSplitter>  // 引入 QSplitter 类，用于创建可动态调整大小的分割条
#include <QWidget>    // 引入 QWidget 类，是所有用户界面对象的基类

#include "trackview/trackview.h"  // 引入 TrackView 类的定义，用于显示轨道头部信息
#include "view/timelineview.h"    // 引入 TimelineView 类的定义，用于显示时间轴主体内容

namespace olive {  // olive 命名空间开始

/**
 * @brief TimelineAndTrackView 类是一个组合控件，用于同时显示时间轴视图 (TimelineView) 和轨道头部视图 (TrackView)。
 *
 * 这个控件通常是视频编辑软件中时间轴区域的核心组成部分。
 * 它使用一个 QSplitter 来允许用户调整 TimelineView 和 TrackView 之间的相对宽度。
 * 并且它负责同步这两个视图的滚动行为。
 */
class TimelineAndTrackView : public QWidget {
 Q_OBJECT  // Q_OBJECT 宏，用于启用 Qt 的元对象特性，如信号和槽

     public :
     /**
      * @brief 构造一个 TimelineAndTrackView 对象。
      * @param vertical_alignment 内部 TrackView 中轨道项的垂直对齐方式，默认为 Qt::AlignTop。
      * @param parent 父 QWidget 对象，默认为 nullptr。
      */
     explicit TimelineAndTrackView(Qt::Alignment vertical_alignment = Qt::AlignTop, QWidget* parent = nullptr);

  /**
   * @brief 获取内部的 QSplitter 对象。
   * @return 指向 QSplitter 对象的常量指针。
   */
  [[nodiscard]] QSplitter* splitter() const;

  /**
   * @brief 获取内部的 TimelineView 对象（时间轴主体视图）。
   * @return 指向 TimelineView 对象的常量指针。
   */
  [[nodiscard]] TimelineView* view() const;

  /**
   * @brief 获取内部的 TrackView 对象（轨道头部视图）。
   * @return 指向 TrackView 对象的常量指针。
   */
  [[nodiscard]] TrackView* track_view() const;

 private:                // 私有成员变量
  QSplitter* splitter_;  ///< 指向 QSplitter 对象的指针，用于管理 TimelineView 和 TrackView 的布局。

  TimelineView* view_;  ///< 指向 TimelineView 对象的指针，显示时间轴上的剪辑内容。

  TrackView* track_view_;  ///< 指向 TrackView 对象的指针，显示轨道头部信息和控制。

 private slots:  // 私有槽函数
  /**
   * @brief 当 TimelineView 的滚动条值发生变化时调用的槽函数。
   *
   * 用于同步 TrackView 的滚动条。
   * @param v TimelineView 滚动条的新值。
   */
  void ViewValueChanged(int v);

  /**
   * @brief 当 TrackView 的滚动条值发生变化时调用的槽函数。
   *
   * 用于同步 TimelineView 的滚动条。
   * @param v TrackView 滚动条的新值。
   */
  void TracksValueChanged(int v);
};

}  // namespace olive

#endif  // TIMELINEANDTRACKVIEW_H
