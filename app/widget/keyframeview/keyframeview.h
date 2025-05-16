#ifndef KEYFRAMEVIEWBASE_H // 文件名可能是 KEYFRAMEVIEWBASE_H，但类通常命名为 KeyframeView
#define KEYFRAMEVIEWBASE_H

#include <functional> // C++ 标准库，提供函数对象等功能

#include "keyframeviewinputconnection.h"             // 关键帧视图输入连接类
#include "node/keyframe.h"                           // 关键帧数据结构定义
#include "widget/menu/menu.h"                        // 自定义菜单基类
#include "widget/timebased/timebasedview.h"          // 基于时间的视图基类
#include "widget/timebased/timebasedviewselectionmanager.h" // 基于时间的视图选择管理器
#include "widget/timetarget/timetarget.h"            // 时间目标对象接口

// Qt 类的前向声明 (根据用户要求，不添加)
// class QWidget;
// class QMouseEvent;
// class QPainter;
// class QRectF;
// class QModelIndex; // 如果使用了 Qt Model/View
// class QString;
// class QColor;
// class QMap;
// class QVector;
// class QVariant;

namespace olive {

// 项目内部类的前向声明 (根据用户要求，不添加)
// class Node;
// class NodeInput;
// class NodeKeyframeTrackReference;
// class ViewerOutput;
// class MultiUndoCommand;

/**
 * @brief KeyframeView 类是显示和编辑关键帧的通用视图组件。
 *
 * 它继承自 TimeBasedView（处理时间轴相关的缩放、平移和网格绘制）
 * 和 TimeTargetObject（使其能够响应和同步到播放时间）。
 * KeyframeView 管理多个关键帧轨道，处理关键帧的选择、绘制、拖动、
 * 复制/粘贴以及上下文菜单等交互。它是曲线编辑器等更高级控件的基础。
 */
class KeyframeView : public TimeBasedView, public TimeTargetObject {
  Q_OBJECT // Qt 元对象系统宏，用于支持信号和槽机制

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit KeyframeView(QWidget *parent = nullptr);

  /**
   * @brief 删除当前选中的所有关键帧。
   */
  void DeleteSelected();

  // 类型定义，用于组织关键帧轨道的连接信息
  using ElementConnections = QVector<KeyframeViewInputConnection *>; ///< 表示单个元素（如向量的 x, y, z 分量之一）的所有关键帧轨道连接。
  using InputConnections = QVector<ElementConnections>;             ///< 表示单个输入参数（可能包含多个元素）的所有轨道连接。
  using NodeConnections = QMap<QString, InputConnections>;          ///< 表示单个节点的所有输入参数及其轨道连接。键为输入参数名。

  /**
   * @brief 添加指定节点的所有可动画参数的关键帧轨道到视图中。
   * @param n 要添加其关键帧轨道的 Node 指针。
   * @return 返回一个 NodeConnections 对象，包含了为该节点添加的所有轨道连接信息。
   */
  NodeConnections AddKeyframesOfNode(Node *n);

  /**
   * @brief 添加指定节点的特定输入参数的所有关键帧轨道到视图中。
   * @param n 节点指针。
   * @param input 输入参数的名称。
   * @return 返回一个 InputConnections 对象，包含为该输入参数添加的所有轨道连接信息。
   */
  InputConnections AddKeyframesOfInput(Node *n, const QString &input);

  /**
   * @brief 添加指定输入参数的单个元素（如果参数是多维的）的关键帧轨道。
   * @param input NodeInput 对象，描述了要连接的参数元素。
   * @return 返回一个 ElementConnections 对象，包含为该元素添加的轨道连接信息。
   */
  ElementConnections AddKeyframesOfElement(const NodeInput &input);

  /**
   * @brief 直接添加一个指定的关键帧轨道到视图中。
   * @param ref 要添加的关键帧轨道的引用 (NodeKeyframeTrackReference)。
   * @return 返回新创建的 KeyframeViewInputConnection 指针。
   */
  KeyframeViewInputConnection *AddKeyframesOfTrack(const NodeKeyframeTrackReference &ref);

  /**
   * @brief 从视图中移除一个关键帧轨道连接。
   * @param connection 要移除的 KeyframeViewInputConnection 指针。
   */
  void RemoveKeyframesOfTrack(KeyframeViewInputConnection *connection);

  /**
   * @brief 选中视图中的所有关键帧。
   */
  void SelectAll();

  /**
   * @brief 取消选中视图中的所有关键帧。
   */
  void DeselectAll();

  /**
   * @brief 清除视图中的所有关键帧轨道和连接。
   */
  void Clear();

  /**
   * @brief 获取当前所有选中的关键帧。
   * @return 返回一个包含 NodeKeyframe 指针的 std::vector 的 const 引用。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const std::vector<NodeKeyframe *> &GetSelectedKeyframes() const {
    return selection_manager_.GetSelectedObjects();
  }

  /**
   * @brief 获取当前视图中显示的所有关键帧轨道。
   * @return 返回一个包含 KeyframeViewInputConnection 指针的 QVector 的 const 引用。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const QVector<KeyframeViewInputConnection *> &GetKeyframeTracks() const { return tracks_; }

  /**
   * @brief 选择管理器选中对象事件的回调。
   * @param obj 被选中的对象指针 (通常是 NodeKeyframe*)。
   */
  void SelectionManagerSelectEvent(void *obj) override;
  /**
   * @brief 选择管理器取消选中对象事件的回调。
   * @param obj 被取消选中的对象指针 (通常是 NodeKeyframe*)。
   */
  void SelectionManagerDeselectEvent(void *obj) override;

  /**
   * @brief 设置视图的最大滚动范围。
   * @param i 最大滚动值。
   */
  void SetMaxScroll(int i) {
    max_scroll_ = i;
    UpdateSceneRect(); // 更新场景矩形以反映新的滚动范围
  }

  /**
   * @brief 复制当前选中的关键帧（支持剪切）。
   * @param cut 如果为 true，则执行剪切操作（复制后删除）。
   * @return 如果成功复制或剪切了关键帧，则返回 true。
   */
  bool CopySelected(bool cut);

  /**
   * @brief 粘贴之前复制或剪切的关键帧。
   * @param find_node_function 一个函数对象，用于根据节点 ID 查找节点指针，以便将关键帧粘贴到正确的节点上。
   * @return 如果成功粘贴了关键帧，则返回 true。
   */
  bool Paste(const std::function<Node *(const QString &)> &find_node_function);

  /**
   * @brief 重写基类的 CatchUpScrollEvent，用于在滚动后执行特定更新。
   */
  void CatchUpScrollEvent() override;

 signals:
  /**
   * @brief 当用户在视图中拖动（例如拖动关键帧或选择框）时发出此信号。
   * @param current_x 鼠标当前 x 坐标（视图坐标系）。
   * @param current_y 鼠标当前 y 坐标（视图坐标系）。
   */
  void Dragged(int current_x, int current_y);

  /**
   * @brief 当视图中的选择状态发生改变时发出此信号。
   */
  void SelectionChanged();

  /**
   * @brief 当用户在视图中释放鼠标（例如结束拖动操作）时发出此信号。
   */
  void Released();

 protected:
  /**
   * @brief 重写鼠标按下事件处理函数。
   * @param event 鼠标事件指针。
   */
  void mousePressEvent(QMouseEvent *event) override;
  /**
   * @brief 重写鼠标移动事件处理函数。
   * @param event 鼠标事件指针。
   */
  void mouseMoveEvent(QMouseEvent *event) override;
  /**
   * @brief 重写鼠标释放事件处理函数。
   * @param event 鼠标事件指针。
   */
  void mouseReleaseEvent(QMouseEvent *event) override;

  /**
   * @brief 重写绘制前景函数。
   *
   * 用于绘制关键帧、连接线、选择框等前景元素。
   * @param painter QPainter 指针。
   * @param rect 需要重绘的区域。
   */
  void drawForeground(QPainter *painter, const QRectF &rect) override;

  /**
   * @brief 虚函数，用于绘制单个关键帧。
   *
   * 派生类可以重写此函数以自定义关键帧的绘制方式。
   * @param painter QPainter 指针。
   * @param key 要绘制的 NodeKeyframe 指针。
   * @param track 关键帧所属的轨道连接对象指针。
   * @param key_rect 关键帧在视图中的预计算包围矩形。
   */
  virtual void DrawKeyframe(QPainter *painter, NodeKeyframe *key, KeyframeViewInputConnection *track,
                            const QRectF &key_rect);

  /**
   * @brief 当视图的水平缩放比例改变时调用的事件处理函数。
   * @param scale 新的水平缩放比例。
   */
  void ScaleChangedEvent(const double &scale) override;

  /**
   * @brief 当视图关联的时间目标（如播放头）改变时调用的事件处理函数。
   * @param v 新的时间目标 ViewerOutput 指针。
   */
  void TimeTargetChangedEvent(ViewerOutput *v) override;

  /**
   * @brief 当视图的时间基准（如帧率）改变时调用的事件处理函数。
   * @param timebase 新的时间基准。
   */
  void TimebaseChangedEvent(const rational &timebase) override;

  /**
   * @brief 虚函数，用于处理上下文菜单事件。
   *
   * 派生类可以重写此函数以添加自定义的上下文菜单项。
   * @param m 对 Menu 对象的引用，用于填充菜单。
   */
  virtual void ContextMenuEvent(Menu &m);

  /**
   * @brief 虚函数，提供第一轮处理鼠标按下事件的机会。
   * @param event 鼠标事件指针。
   * @return 如果事件已被处理，则返回 true，阻止后续处理；否则返回 false。
   */
  virtual bool FirstChanceMousePress(QMouseEvent *event) { return false; }
  /**
   * @brief 虚函数，提供第一轮处理鼠标移动事件的机会。
   * @param event 鼠标事件指针。
   */
  virtual void FirstChanceMouseMove(QMouseEvent *event) {}
  /**
   * @brief 虚函数，提供第一轮处理鼠标释放事件的机会。
   * @param event 鼠标事件指针。
   */
  virtual void FirstChanceMouseRelease(QMouseEvent *event) {}

  /**
   * @brief 虚函数，在关键帧拖动开始时调用。
   * @param event 鼠标事件指针。
   */
  virtual void KeyframeDragStart(QMouseEvent *event) {}
  /**
   * @brief 虚函数，在关键帧拖动过程中调用。
   * @param event 鼠标事件指针。
   * @param tip 用于在光标附近显示提示信息的 QString 引用。
   */
  virtual void KeyframeDragMove(QMouseEvent *event, QString &tip) {}
  /**
   * @brief 虚函数，在关键帧拖动结束时调用。
   * @param event 鼠标事件指针。
   * @param command 指向 MultiUndoCommand 的指针，用于记录拖动操作以便撤销。
   */
  virtual void KeyframeDragRelease(QMouseEvent *event, MultiUndoCommand *command) {}

  /**
   * @brief 选中指定的关键帧。
   * @param key 要选中的 NodeKeyframe 指针。
   */
  void SelectKeyframe(NodeKeyframe *key);

  /**
   * @brief 取消选中指定的关键帧。
   * @param key 要取消选中的 NodeKeyframe 指针。
   */
  void DeselectKeyframe(NodeKeyframe *key);

  /**
   * @brief 检查指定的关键帧是否被选中。
   * @param key 要检查的 NodeKeyframe 指针。
   * @return 如果关键帧被选中，则返回 true；否则返回 false。
   */
  bool IsKeyframeSelected(NodeKeyframe *key) const { return selection_manager_.IsSelected(key); }

  /**
   * @brief 获取关键帧未经时间调整（例如，未考虑父节点时间偏移）的原始时间。
   * @param key 关键帧指针。
   * @param time 关键帧的当前时间（可能已调整）。
   * @return 关键帧的原始 rational 时间。
   */
  rational GetUnadjustedKeyframeTime(NodeKeyframe *key, const rational &time);
  /**
   * @brief 获取关键帧未经时间调整的原始时间（重载版本）。
   * @param key 关键帧指针。
   * @return 关键帧的原始 rational 时间。
   */
  rational GetUnadjustedKeyframeTime(NodeKeyframe *key) { return GetUnadjustedKeyframeTime(key, key->time()); }

  /**
   * @brief 获取关键帧经过时间调整后的时间（例如，考虑了父节点时间偏移）。
   * @param key 关键帧指针。
   * @return 调整后的 rational 时间。
   */
  rational GetAdjustedKeyframeTime(NodeKeyframe *key);

  /**
   * @brief 获取关键帧在场景坐标系中的 X 轴位置。
   *
   * 此位置通常基于关键帧的时间和当前视图的时间缩放。
   * @param key 关键帧指针。
   * @return 关键帧在场景中的 X 坐标。
   */
  double GetKeyframeSceneX(NodeKeyframe *key);

  /**
   * @brief 虚函数，获取关键帧在场景坐标系中的 Y 轴位置。
   *
   * 派生类（如 CurveView）会重写此函数以根据关键帧的值来计算 Y 坐标。
   * @param track 关键帧所属的轨道连接对象指针。
   * @param key 关键帧指针。
   * @return 关键帧在场景中的 Y 坐标。
   */
  virtual qreal GetKeyframeSceneY(KeyframeViewInputConnection *track, NodeKeyframe *key);

  /**
   * @brief 设置是否自动选择兄弟关键帧（例如，在多维参数中，选中一个分量的关键帧时自动选中其他分量的对应关键帧）。
   * @param e 如果为 true，则启用自动选择兄弟关键帧；否则禁用。
   */
  void SetAutoSelectSiblings(bool e) { autoselect_siblings_ = e; }

  /**
   * @brief 当场景矩形需要更新时调用的事件处理函数。
   * @param rect 对 QRectF 的引用，用于返回计算得到的场景矩形。
   */
  void SceneRectUpdateEvent(QRectF &rect) override;

 protected slots:
  /**
   * @brief 请求重绘视图的槽函数。
   */
  void Redraw();

 private:
  /**
   * @brief 根据屏幕上的光标水平位移计算新的时间值。
   * @param old_time 原始时间。
   * @param cursor_diff 光标在屏幕 X 轴上的位移量。
   * @return 计算得到的新 rational 时间。
   */
  static rational CalculateNewTimeFromScreen(const rational &old_time, double cursor_diff);

  QVector<KeyframeViewInputConnection *> tracks_; ///< 存储当前视图中所有关键帧轨道连接的列表。

  TimeBasedViewSelectionManager<NodeKeyframe> selection_manager_; ///< 用于管理关键帧选择的模板类实例。

  bool autoselect_siblings_; ///< 标记是否启用自动选择兄弟关键帧的功能。

  int max_scroll_; ///< 视图的最大滚动范围（通常是垂直方向）。

  bool first_chance_mouse_event_; ///< 标记是否应由派生类优先处理鼠标事件。

 private slots:
  /**
   * @brief 显示上下文菜单的槽函数。
   */
  void ShowContextMenu();

  /**
   * @brief 显示关键帧属性对话框的槽函数。
   */
  void ShowKeyframePropertiesDialog();

  /**
   * @brief 当视图滚动时更新橡皮筋选择框（如果正在进行选择）的槽函数。
   */
  void UpdateRubberBandForScroll();
};

}  // namespace olive

#endif  // KEYFRAMEVIEWBASE_H
