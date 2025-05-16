#ifndef TRACKVIEW_H  // 防止头文件被多次包含的宏定义
#define TRACKVIEW_H

#include <QScrollArea>  // 引入 QScrollArea 类，用于提供带滚动条的视图区域
#include <QSplitter>    // 引入 QSplitter 类，用于创建可动态调整大小的分割条

#include "node/output/track/tracklist.h"  // 引入 TrackList 类的定义，表示轨道列表
#include "trackviewitem.h"                // 引入 TrackViewItem 类的定义，表示单个轨道视图项
#include "trackviewsplitter.h"            // 引入 TrackViewSplitter 类的定义，自定义的分割条

// 根据代码上下文，以下类型应由已包含的头文件或其传递包含的头文件提供定义：
// - Qt::Alignment: Qt 枚举，用于构造函数参数
// - QWidget: Qt 基类，用于构造函数参数
// - QResizeEvent: Qt 事件类，用于 resizeEvent 方法参数
// - olive::Track: 用于信号和槽函数的参数
// 此处严格按照用户提供的代码，不添加额外的 #include 或前向声明。

namespace olive {  // olive 命名空间开始

/**
 * @brief TrackView 类是一个用于显示时间轴中各个轨道头部信息（如轨道名称、控制按钮等）的控件。
 *
 * 它通常与时间轴的主体部分（显示剪辑内容的部分）并列显示，并允许用户与轨道进行交互。
 * TrackView 继承自 QScrollArea，因此当轨道数量过多超出可视范围时，可以提供滚动功能。
 * 它内部使用一个 QSplitter 来管理各个轨道视图项 (TrackViewItem) 的布局和大小调整。
 */
class TrackView : public QScrollArea {
 Q_OBJECT  // Q_OBJECT 宏，用于启用 Qt 的元对象特性，如信号和槽

     public :
     /**
      * @brief 构造一个 TrackView 对象。
      * @param vertical_alignment 轨道视图项在垂直方向上的对齐方式，默认为 Qt::AlignTop (顶部对齐)。
      * @param parent 父 QWidget 对象，默认为 nullptr。
      */
     explicit TrackView(Qt::Alignment vertical_alignment = Qt::AlignTop, QWidget* parent = nullptr);

  /**
   * @brief 连接到一个 TrackList 对象。
   *
   * TrackList 提供了轨道数据的来源。连接后，TrackView 会监听 TrackList 的变化
   * (例如添加或移除轨道) 并相应地更新其显示。
   * @param list 指向要连接的 TrackList 对象的指针。
   */
  void ConnectTrackList(TrackList* list);
  /**
   * @brief 断开与当前 TrackList 对象的连接。
   *
   * 断开连接后，TrackView 将不再响应 TrackList 的变化。
   */
  void DisconnectTrackList();

 signals:  // 信号
  /**
   * @brief 当一个轨道即将被删除时发出此信号。
   *
   * 这允许其他关联的组件在轨道实际从数据模型中移除前做出响应。
   * @param track 指向即将被删除的 Track 对象的指针。
   */
  void AboutToDeleteTrack(Track* track);

 protected:  // 受保护成员
  /**
   * @brief 重写 QWidget::resizeEvent()，处理控件大小调整事件。
   *
   * 当 TrackView 的大小发生变化时调用此方法，可能用于调整内部布局或滚动条。
   * @param e 指向 QResizeEvent 对象的指针，包含事件参数。
   */
  void resizeEvent(QResizeEvent* e) override;

 private:            // 私有成员
  TrackList* list_;  ///< 指向当前连接的 TrackList 对象的指针，提供轨道数据。

  TrackViewSplitter* splitter_;  ///< 指向 TrackViewSplitter 对象的指针，用于管理轨道项的布局和分隔。

  Qt::Alignment alignment_;  ///< 存储轨道视图项的垂直对齐方式。

  int last_scrollbar_max_;  ///< 记录上一次滚动条的最大值，可能用于检测变化。

 private slots:  // 私有槽函数
  /**
   * @brief 当滚动条的范围 (最小值和最大值) 发生变化时调用的槽函数。
   * @param min 滚动条新的最小值。
   * @param max 滚动条新的最大值。
   */
  void ScrollbarRangeChanged(int min, int max);

  /**
   * @brief 当某个轨道的视图项高度发生变化时调用的槽函数。
   * @param index 发生高度变化的轨道的索引。
   * @param height 轨道视图项的新高度。
   */
  void TrackHeightChanged(int index, int height);

  /**
   * @brief 当 TrackList 中插入一个新轨道时调用的槽函数。
   *
   * 此槽函数负责在 TrackView 中创建一个对应的 TrackViewItem 并显示它。
   * @param track 指向新插入的 Track 对象的指针。
   */
  void InsertTrack(Track* track);

  /**
   * @brief 当 TrackList 中移除一个轨道时调用的槽函数。
   *
   * 此槽函数负责从 TrackView 中移除并删除与该轨道对应的 TrackViewItem。
   * @param track 指向被移除的 Track 对象的指针。
   */
  void RemoveTrack(Track* track);
};

}  // namespace olive

#endif  // TRACKVIEW_H
