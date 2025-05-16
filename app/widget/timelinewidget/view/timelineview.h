#ifndef TIMELINEVIEW_H  // 防止头文件被多次包含的宏定义
#define TIMELINEVIEW_H

#include <QDragEnterEvent>  // 引入 QDragEnterEvent 类，用于处理拖动进入事件
#include <QDragLeaveEvent>  // 引入 QDragLeaveEvent 类，用于处理拖动离开事件
#include <QDragMoveEvent>   // 引入 QDragMoveEvent 类，用于处理拖动移动事件
#include <QDropEvent>       // 引入 QDropEvent 类，用于处理拖放事件
#include <QGraphicsView>    // 引入 QGraphicsView 类，是显示 QGraphicsScene 内容的控件基类

#include "node/block/clip/clip.h"            // 引入 ClipBlock 类的定义
#include "timelineviewghostitem.h"           // 引入 TimelineViewGhostItem 类的定义，用于显示拖动时的预览项
#include "timelineviewmouseevent.h"          // 引入 TimelineViewMouseEvent 类的定义，封装时间轴视图的鼠标事件
#include "widget/timebased/timebasedview.h"  // 引入 TimeBasedView 类的定义，是 TimelineView 的基类

// 根据代码上下文，以下类型应由已包含的头文件或其传递包含的头文件提供定义：
// - Qt::Alignment, QWidget, QPoint, QPointF, QRectF, QPainter, QMouseEvent, Qt::MouseButton, Qt::KeyboardModifiers:
// 标准Qt类型
// - olive::TrackList, olive::TimelineCoordinate, olive::Block, olive::rational, olive::Track::Reference,
//   olive::TimeRangeList, olive::Tool::Item, olive::FrameHashCache, olive::TimelineMarker: 项目特定类型
// 此处严格按照用户提供的代码，不添加额外的 #include 或前向声明。

namespace olive {  // olive 命名空间开始

/**
 * @brief TimelineView 类是一个用于查看和与序列 (Sequences) 交互的控件。
 *
 * 这个控件主要让用户能够查看和修改 Block 节点，通常是通过一个 TimelineOutput 节点（尽管此处未直接引用，但上下文暗示）。
 * 它继承自 TimeBasedView，提供了时间轴相关的显示和交互基础。
 */
class TimelineView : public TimeBasedView {
 Q_OBJECT  // Q_OBJECT 宏，用于启用 Qt 的元对象特性，如信号和槽

     public :
     /**
      * @brief 构造一个 TimelineView 对象。
      * @param vertical_alignment 子项在垂直方向上的对齐方式，默认为 Qt::AlignTop (顶部对齐)。
      * @param parent 父 QWidget 对象，默认为 nullptr。
      */
     explicit TimelineView(Qt::Alignment vertical_alignment = Qt::AlignTop, QWidget *parent = nullptr);

  /**
   * @brief 获取指定轨道索引的 Y 轴场景坐标。
   * @param track_index 轨道的索引。
   * @return 轨道顶部的 Y 轴场景坐标 (int)。
   */
  [[nodiscard]] int GetTrackY(int track_index) const;
  /**
   * @brief 获取指定轨道索引的高度。
   * @param track_index 轨道的索引。
   * @return 轨道的高度 (int)。
   */
  [[nodiscard]] int GetTrackHeight(int track_index) const;

  /**
   * @brief 获取当前的滚动条坐标。
   * @return 表示水平和垂直滚动条位置的 QPoint。
   */
  [[nodiscard]] QPoint GetScrollCoordinates() const;
  /**
   * @brief 设置滚动条的坐标。
   * @param pt 包含新的水平和垂直滚动条位置的 QPoint。
   */
  void SetScrollCoordinates(const QPoint &pt);

  /**
   * @brief 连接到一个 TrackList 对象。
   *
   * TrackList 提供了轨道数据的来源。连接后，TimelineView 会监听 TrackList 的变化。
   * @param list 指向要连接的 TrackList 对象的指针。
   */
  void ConnectTrackList(TrackList *list);

  /**
   * @brief 设置光束光标（通常用于指示当前工具在时间轴上的精确位置）的坐标。
   * @param coord 光束光标的时间轴坐标 (TimelineCoordinate)。
   */
  void SetBeamCursor(const TimelineCoordinate &coord);
  /**
   * @brief 设置转场叠加层的显示。
   *
   * 用于在两个剪辑之间可视化地表示一个潜在的或已存在的转场。
   * @param out 指向转场出点侧的 ClipBlock 对象的指针。
   * @param in 指向转场入点侧的 ClipBlock 对象的指针。
   */
  void SetTransitionOverlay(ClipBlock *out, ClipBlock *in);
  /**
   * @brief 启用录制叠加层的显示。
   *
   * 可能用于指示当前正在录制或准备录制的区域。
   * @param coord 录制叠加层应显示的时间轴坐标 (TimelineCoordinate)。
   */
  void EnableRecordingOverlay(const TimelineCoordinate &coord);
  /**
   * @brief 禁用录制叠加层的显示。
   */
  void DisableRecordingOverlay();

  /**
   * @brief 设置当前的选择列表。
   *
   * 选择列表是一个哈希表，键是轨道引用，值是该轨道上被选中的时间范围列表。
   * @param s 指向选择列表 (QHash<Track::Reference, TimeRangeList>*) 的指针。
   */
  void SetSelectionList(QHash<Track::Reference, TimeRangeList> *s) { selections_ = s; }

  /**
   * @brief 设置当前的幽灵项列表。
   *
   * 幽灵项用于在拖动操作期间提供视觉预览。
   * @param ghosts 指向幽灵项列表 (QVector<TimelineViewGhostItem *>*) 的指针。
   */
  void SetGhostList(QVector<TimelineViewGhostItem *> *ghosts) { ghosts_ = ghosts; }

  /**
   * @brief 将场景中的 Y 坐标转换为轨道索引。
   * @param y 场景中的 Y 坐标 (double)。
   * @return 对应的轨道索引 (int)，如果坐标不在任何轨道内，可能返回无效值。
   */
  int SceneToTrack(double y);

  /**
   * @brief 获取在指定场景位置（时间和轨道索引）的 Block 项。
   * @param time 时间点 (rational)。
   * @param track_index 轨道的索引。
   * @return 如果找到，则返回指向 Block 对象的指针；否则返回 nullptr。
   */
  [[nodiscard]] Block *GetItemAtScenePos(const rational &time, int track_index) const;

  /**
   * @brief 获取与指定场景矩形区域相交的所有 Block 项。
   * @param rect 场景中的矩形区域 (QRectF)。
   * @return 包含所有相交 Block 指针的 QVector。
   */
  [[nodiscard]] QVector<Block *> GetItemsAtSceneRect(const QRectF &rect) const;

 signals:  // 信号
  /**
   * @brief 当在视图中按下鼠标按钮时发出此信号。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含事件信息。
   */
  void MousePressed(TimelineViewMouseEvent *event);
  /**
   * @brief 当在视图中移动鼠标（通常在按下按钮后）时发出此信号。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含事件信息。
   */
  void MouseMoved(TimelineViewMouseEvent *event);
  /**
   * @brief 当在视图中释放鼠标按钮时发出此信号。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含事件信息。
   */
  void MouseReleased(TimelineViewMouseEvent *event);
  /**
   * @brief 当在视图中双击鼠标按钮时发出此信号。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含事件信息。
   */
  void MouseDoubleClicked(TimelineViewMouseEvent *event);

  /**
   * @brief 当拖动操作进入视图时发出此信号。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含事件信息。
   */
  void DragEntered(TimelineViewMouseEvent *event);
  /**
   * @brief 当拖动操作在视图内移动时发出此信号。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含事件信息。
   */
  void DragMoved(TimelineViewMouseEvent *event);
  /**
   * @brief 当拖动操作离开视图时发出此信号。
   * @param event 指向 QDragLeaveEvent 对象的指针。
   */
  void DragLeft(QDragLeaveEvent *event);
  /**
   * @brief 当拖放操作在视图中完成时发出此信号。
   * @param event 指向 TimelineViewMouseEvent 对象的指针，包含事件信息。
   */
  void DragDropped(TimelineViewMouseEvent *event);

 protected:  // 受保护的重写方法
  /**
   * @brief 重写 QWidget::mousePressEvent()，处理原始鼠标按下事件。
   * @param event 指向 QMouseEvent 对象的指针。
   */
  void mousePressEvent(QMouseEvent *event) override;
  /**
   * @brief 重写 QWidget::mouseMoveEvent()，处理原始鼠标移动事件。
   * @param event 指向 QMouseEvent 对象的指针。
   */
  void mouseMoveEvent(QMouseEvent *event) override;
  /**
   * @brief 重写 QWidget::mouseReleaseEvent()，处理原始鼠标释放事件。
   * @param event 指向 QMouseEvent 对象的指针。
   */
  void mouseReleaseEvent(QMouseEvent *event) override;
  /**
   * @brief 重写 QWidget::mouseDoubleClickEvent()，处理原始鼠标双击事件。
   * @param event 指向 QMouseEvent 对象的指针。
   */
  void mouseDoubleClickEvent(QMouseEvent *event) override;

  /**
   * @brief 重写 QWidget::dragEnterEvent()，处理原始拖动进入事件。
   * @param event 指向 QDragEnterEvent 对象的指针。
   */
  void dragEnterEvent(QDragEnterEvent *event) override;
  /**
   * @brief 重写 QWidget::dragMoveEvent()，处理原始拖动移动事件。
   * @param event 指向 QDragMoveEvent 对象的指针。
   */
  void dragMoveEvent(QDragMoveEvent *event) override;
  /**
   * @brief 重写 QWidget::dragLeaveEvent()，处理原始拖动离开事件。
   * @param event 指向 QDragLeaveEvent 对象的指针。
   */
  void dragLeaveEvent(QDragLeaveEvent *event) override;
  /**
   * @brief 重写 QWidget::dropEvent()，处理原始拖放事件。
   * @param event 指向 QDropEvent 对象的指针。
   */
  void dropEvent(QDropEvent *event) override;

  /**
   * @brief 重写 QGraphicsView::drawBackground()，用于绘制视图的背景。
   * @param painter QPainter 指针，用于绘图。
   * @param rect 需要重绘的区域 (QRectF)。
   */
  void drawBackground(QPainter *painter, const QRectF &rect) override;
  /**
   * @brief 重写 QGraphicsView::drawForeground()，用于在前景中绘制额外内容（如播放头、选择框等）。
   * @param painter QPainter 指针，用于绘图。
   * @param rect 需要重绘的区域 (QRectF)。
   */
  void drawForeground(QPainter *painter, const QRectF &rect) override;

  /**
   * @brief 当活动工具发生变化时调用的事件处理函数 (可能继承自某个基类)。
   * @param tool 新的活动工具项 (Tool::Item)。
   */
  void ToolChangedEvent(Tool::Item tool) override;

  /**
   * @brief 当场景矩形需要更新时调用的事件处理函数 (继承自 TimeBasedView)。
   * @param rect 对场景矩形 (QRectF) 的引用，可以在函数内修改。
   */
  void SceneRectUpdateEvent(QRectF &rect) override;

 private:  // 私有方法
  /**
   * @brief 获取连接的轨道的类型。
   * @return 轨道的类型 (Track::Type)。
   */
  Track::Type ConnectedTrackType();

  /**
   * @brief 将屏幕坐标（相对于视图控件）转换为时间轴坐标。
   * @param pt 屏幕坐标 (QPoint)。
   * @return 对应的时间轴坐标 (TimelineCoordinate)。
   */
  TimelineCoordinate ScreenToCoordinate(const QPoint &pt);
  /**
   * @brief 将场景坐标转换为时间轴坐标。
   * @param pt 场景坐标 (QPointF)。
   * @return 对应的时间轴坐标 (TimelineCoordinate)。
   */
  TimelineCoordinate SceneToCoordinate(const QPointF &pt);

  /**
   * @brief 根据原始 QMouseEvent 创建一个 TimelineViewMouseEvent 对象。
   * @param event 指向原始 QMouseEvent 对象的指针。
   * @return 创建的 TimelineViewMouseEvent 对象。
   */
  TimelineViewMouseEvent CreateMouseEvent(QMouseEvent *event);
  /**
   * @brief 根据给定的位置和按键状态创建一个 TimelineViewMouseEvent 对象。
   * @param pos 鼠标位置 (QPoint)。
   * @param button 按下的鼠标按钮 (Qt::MouseButton)。
   * @param modifiers 键盘修饰键状态 (Qt::KeyboardModifiers)。
   * @return 创建的 TimelineViewMouseEvent 对象。
   */
  TimelineViewMouseEvent CreateMouseEvent(const QPoint &pos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);

  /**
   * @brief 绘制时间轴上的所有 Block 项。
   * @param painter QPainter 指针，用于绘图。
   * @param foreground 如果为 true，则绘制前景元素；否则绘制背景元素。
   */
  void DrawBlocks(QPainter *painter, bool foreground);

  /**
   * @brief 绘制单个 Block 项。
   * @param painter QPainter 指针。
   * @param foreground 如果为 true，则绘制前景；否则绘制背景。
   * @param block 指向要绘制的 Block 对象的指针。
   * @param top Block 在视图中的顶部 Y 坐标。
   * @param height Block 在视图中的高度。
   * @param in Block 的入点时间。
   * @param out Block 的出点时间。
   * @param media_in Block 对应媒体的入点时间。
   */
  void DrawBlock(QPainter *painter, bool foreground, Block *block, qreal top, qreal height, const rational &in,
                 const rational &out, const rational &media_in);
  /**
   * @brief 绘制单个 Block 项的重载版本 (主要用于 ClipBlock)。
   * @param painter QPainter 指针。
   * @param foreground 如果为 true，则绘制前景；否则绘制背景。
   * @param block 指向要绘制的 Block 对象的指针。
   * @param top Block 在视图中的顶部 Y 坐标。
   * @param height Block 在视图中的高度。
   */
  void DrawBlock(QPainter *painter, bool foreground, Block *block, qreal top, qreal height) {
    auto *cb = dynamic_cast<ClipBlock *>(block);  // 尝试将 Block 动态转换为 ClipBlock
    return DrawBlock(painter, foreground, block, top, height, block->in(), block->out(),
                     cb ? cb->media_in() : rational(0));  // 如果是 ClipBlock，使用其媒体入点，否则使用0
  }

  /**
   * @brief 绘制时间轴轨道的斑马条纹背景。
   * @param painter QPainter 指针。
   * @param r 需要绘制斑马条纹的矩形区域 (QRectF)。
   */
  void DrawZebraStripes(QPainter *painter, const QRectF &r);

  /**
   * @brief 获取所有轨道的总高度。
   * @return 所有轨道的总高度 (int)。
   */
  [[nodiscard]] int GetHeightOfAllTracks() const;

  /**
   * @brief 更新播放头在视图中表示的矩形区域。
   */
  void UpdatePlayheadRect();

  /**
   * @brief 获取时间轴内容区域的左边界场景坐标。
   * @return 左边界的 X 坐标 (qreal)。
   */
  [[nodiscard]] qreal GetTimelineLeftBound() const;

  /**
   * @brief 获取时间轴内容区域的右边界场景坐标。
   * @return 右边界的 X 坐标 (qreal)。
   */
  [[nodiscard]] qreal GetTimelineRightBound() const;

  /**
   * @brief 绘制指定时间的缩略图。
   * @param painter QPainter 指针。
   * @param thumbs 指向 FrameHashCache 对象的指针，用于获取缩略图数据。
   * @param time 要绘制缩略图的时间点 (rational)。
   * @param x 缩略图在视图中的 X 绘制位置。
   * @param preview_rect 缩略图的预览区域 (QRect)。
   * @param thumb_rect 指向 QRect 的指针，用于存储实际绘制的缩略图矩形 (输出参数)。
   */
  static void DrawThumbnail(QPainter *painter, const FrameHashCache *thumbs, const rational &time, int x,
                            const QRect &preview_rect, QRect *thumb_rect);

  QHash<Track::Reference, TimeRangeList> *selections_;  ///< 指向当前选择信息哈希表的指针。

  QVector<TimelineViewGhostItem *> *ghosts_;  ///< 指向当前幽灵项（拖动预览）列表的指针。

  bool show_beam_cursor_;  ///< 标记是否显示光束光标。

  TimelineCoordinate cursor_coord_;  ///< 当前光束光标的时间轴坐标。

  TrackList *connected_track_list_;  ///< 指向已连接的 TrackList 对象的指针。

  ClipBlock *transition_overlay_out_;  ///< 指向用于转场叠加层显示的“出点侧”剪辑块的指针。
  ClipBlock *transition_overlay_in_;   ///< 指向用于转场叠加层显示的“入点侧”剪辑块的指针。

  QMap<TimelineMarker *, QRectF> clip_marker_rects_;  ///< 存储剪辑标记点及其在场景中矩形区域的映射。

  bool recording_overlay_{};            ///< 标记录制叠加层是否启用，默认为 false。
  TimelineCoordinate recording_coord_;  ///< 录制叠加层的时间轴坐标。

 private slots:  // 私有槽函数
  /**
   * @brief 当连接的 TrackList 发生变化（例如添加/删除轨道）时调用的槽函数。
   *
   * 用于更新视图以反映轨道的更改。
   */
  void TrackListChanged();
};

}  // namespace olive

#endif  // TIMELINEVIEW_H
