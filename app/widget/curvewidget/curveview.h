#ifndef CURVEVIEW_H
#define CURVEVIEW_H

#include "node/keyframe.h"                     // 关键帧数据结构
#include "widget/keyframeview/keyframeview.h"  // 关键帧视图基类
#include "widget/slider/floatslider.h"         // 浮点数滑块控件 (可能用于显示/编辑关键帧值)

// 前向声明 Qt 类 (这些是根据代码中的使用推断的，如果头文件已包含则无需重复)
// class QPainter; // Qt 绘图类
// class QRectF;   // Qt 浮点数矩形类
// class QMouseEvent; // Qt 鼠标事件类
// class QPointF;  // Qt 浮点数二维点类
// class QColor;   // Qt 颜色类
// class QVariant; // Qt 通用数据类型类
// class QWidget; // Qt 控件基类 (已通过 KeyframeView 间接包含)

namespace olive {

// 前向声明项目内部类 (这些是根据代码中的使用推断的，如果头文件已包含则无需重复)
// class Menu; // 自定义菜单类
// class MultiUndoCommand; // 复合撤销命令类
// 结构体 NodeKeyframeTrackReference 和 KeyframeViewInputConnection 应该已在包含的头文件中定义

/**
 * @brief CurveView 类是一个用于显示和编辑动画曲线（关键帧及其插值）的控件。
 *
 * 它继承自 KeyframeView，专门用于可视化和操作参数随时间变化的曲线，
 * 包括关键帧的创建、删除、移动以及调整贝塞尔控制柄以改变插值曲线的形状。
 * 支持连接多个输入轨道 (NodeKeyframeTrackReference)，并为每个轨道显示其关键帧和曲线。
 */
class CurveView : public KeyframeView {
 Q_OBJECT  // Qt 元对象系统宏，用于支持信号和槽机制

     public :
     /**
      * @brief 构造函数。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit CurveView(QWidget *parent = nullptr);

  /**
   * @brief 连接一个节点关键帧轨道到视图中进行显示和编辑。
   * @param ref 要连接的节点关键帧轨道的引用。
   */
  void ConnectInput(const NodeKeyframeTrackReference &ref);

  /**
   * @brief 从视图中移除（断开连接）一个节点关键帧轨道。
   * @param ref 要断开连接的节点关键帧轨道的引用。
   */
  void DisconnectInput(const NodeKeyframeTrackReference &ref);

  /**
   * @brief 选中指定输入轨道上的所有关键帧。
   * @param ref 要选中其关键帧的节点关键帧轨道的引用。
   */
  void SelectKeyframesOfInput(const NodeKeyframeTrackReference &ref);

  /**
   * @brief 设置指定输入轨道的关键帧和曲线的显示颜色。
   * @param ref 要设置颜色的节点关键帧轨道的引用。
   * @param color 要设置的 QColor 对象。
   */
  void SetKeyframeTrackColor(const NodeKeyframeTrackReference &ref, const QColor &color);

  /**
   * @brief 获取当前所有连接到此视图的轨道及其对应的连接信息。
   * @return 返回一个 const 引用，指向存储轨道引用和 KeyframeViewInputConnection 指针的 QHash。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const QHash<NodeKeyframeTrackReference, KeyframeViewInputConnection *> &GetConnections() const {
    return track_connections_;
  }

 public slots:
  /**
   * @brief 缩放视图以适应所有可见的关键帧和曲线。
   */
  void ZoomToFit();

  /**
   * @brief 缩放视图以适应当前选中的关键帧和曲线。
   */
  void ZoomToFitSelected();

  /**
   * @brief 重置视图的缩放级别到默认状态。
   */
  void ResetZoom();

 protected:
  /**
   * @brief 重写的绘制背景函数。
   *
   * 在绘制前景内容（如关键帧、曲线）之前调用，用于绘制网格线等背景元素。
   * @param painter QPainter 指针，用于进行绘制操作。
   * @param rect 当前需要重绘的视口区域。
   */
  void drawBackground(QPainter *painter, const QRectF &rect) override;
  /**
   * @brief 重写的绘制前景函数。
   *
   * 在绘制背景之后调用，用于绘制关键帧、曲线、贝塞尔控制柄等前景元素。
   * @param painter QPainter 指针，用于进行绘制操作。
   * @param rect 当前需要重绘的视口区域。
   */
  void drawForeground(QPainter *painter, const QRectF &rect) override;

  /**
   * @brief 重写的上下文菜单事件处理函数。
   *
   * 当用户请求上下文菜单时（例如右键点击），此函数被调用以填充菜单项。
   * @param m 对 Menu 对象的引用，用于添加菜单项。
   */
  void ContextMenuEvent(Menu &m) override;

  /**
   * @brief 重写的场景矩形更新事件处理函数。
   *
   * 当视图的逻辑场景矩形需要更新或计算时调用。
   * @param r 对 QRectF 的引用，用于返回计算得到的场景矩形。
   */
  void SceneRectUpdateEvent(QRectF &r) override;

  /**
   * @brief 重写的获取关键帧在场景中的 Y 坐标的函数。
   *
   * 根据关键帧的值计算其在垂直方向上的显示位置。
   * @param track 关键帧所属的轨道连接对象指针。
   * @param key 指向 NodeKeyframe 对象的指针。
   * @return 返回关键帧在场景坐标系中的 Y 值。
   */
  qreal GetKeyframeSceneY(KeyframeViewInputConnection *track, NodeKeyframe *key) override;

  /**
   * @brief 重写的绘制单个关键帧的函数。
   * @param painter QPainter 指针。
   * @param key 要绘制的 NodeKeyframe 指针。
   * @param track 关键帧所属的轨道连接对象指针。
   * @param key_rect 关键帧在视图中的包围矩形。
   */
  void DrawKeyframe(QPainter *painter, NodeKeyframe *key, KeyframeViewInputConnection *track,
                    const QRectF &key_rect) override;

  /**
   * @brief 重写的第一轮鼠标按下事件处理。
   * @param event 鼠标事件指针。
   * @return 如果事件被处理则返回 true，否则返回 false 以便基类继续处理。
   */
  bool FirstChanceMousePress(QMouseEvent *event) override;
  /**
   * @brief 重写的第一轮鼠标移动事件处理。
   * @param event 鼠标事件指针。
   */
  void FirstChanceMouseMove(QMouseEvent *event) override;
  /**
   * @brief 重写的第一轮鼠标释放事件处理。
   * @param event 鼠标事件指针。
   */
  void FirstChanceMouseRelease(QMouseEvent *event) override;

  /**
   * @brief 重写的关键帧拖动开始处理。
   * @param event 鼠标事件指针。
   */
  void KeyframeDragStart(QMouseEvent *event) override;
  /**
   * @brief 重写的关键帧拖动过程处理。
   * @param event 鼠标事件指针。
   * @param tip 用于显示在鼠标光标附近的提示信息的 QString 引用。
   */
  void KeyframeDragMove(QMouseEvent *event, QString &tip) override;
  /**
   * @brief 重写的关键帧拖动结束处理。
   * @param event 鼠标事件指针。
   * @param command 指向 MultiUndoCommand 的指针，用于记录此次拖动操作以便撤销。
   */
  void KeyframeDragRelease(QMouseEvent *event, MultiUndoCommand *command) override;

 private:
  /**
   * @brief 内部实现的缩放以适应内容的逻辑。
   * @param selected_only 如果为 true，则只适应选中的关键帧；否则适应所有关键帧。
   */
  void ZoomToFitInternal(bool selected_only);

  /**
   * @brief 根据关键帧的值获取其在视图中的 Y 坐标（考虑当前的缩放和偏移）。
   * @param key 指向 NodeKeyframe 对象的指针。
   * @return 关键帧在视图 Y 轴上的位置。
   */
  qreal GetItemYFromKeyframeValue(NodeKeyframe *key);
  /**
   * @brief 根据关键帧的值获取其未缩放的 Y 坐标。
   * @param key 指向 NodeKeyframe 对象的指针。
   * @return 关键帧的原始 Y 值（未应用视图缩放和偏移）。
   */
  static qreal GetUnscaledItemYFromKeyframeValue(NodeKeyframe *key);

  /**
   * @brief 根据当前视图的变换（缩放、平移）来缩放一个点。
   * @param point 要缩放的 QPointF 点。
   * @return 缩放后的 QPointF 点。
   */
  QPointF ScalePoint(const QPointF &point);

  /**
   * @brief 根据关键帧的类型获取其浮点数值的显示方式。
   * @param key 指向 NodeKeyframe 对象的指针。
   * @return 返回 FloatSlider::DisplayType 枚举值。
   */
  static FloatSlider::DisplayType GetFloatDisplayTypeFromKeyframe(NodeKeyframe *key);

  /**
   * @brief 获取关键帧的偏移值（如果适用）。
   * @param key 指向 NodeKeyframe 对象的指针。
   * @return 关键帧的偏移量。
   */
  static double GetOffsetFromKeyframe(NodeKeyframe *key);

  /**
   * @brief 调整连接关键帧的曲线（线条）的绘制。
   */
  void AdjustLines();

  /**
   * @brief 获取指定关键帧在场景中的精确位置（x, y 坐标）。
   * @param key 指向 NodeKeyframe 对象的指针。
   * @return 关键帧的 QPointF 位置。
   */
  QPointF GetKeyframePosition(NodeKeyframe *key);

  /**
   * @brief 根据贝塞尔模式、起点和光标差值生成贝塞尔控制点的位置。
   * @param mode 贝塞尔控制柄的类型 (例如，入控制柄或出控制柄)。
   * @param start_point 关键帧点的位置。
   * @param scaled_cursor_diff 经过缩放的光标与关键帧点的差值向量。
   * @return 计算得到的贝塞尔控制点的位置。
   */
  static QPointF GenerateBezierControlPosition(NodeKeyframe::BezierType mode, const QPointF &start_point,
                                               const QPointF &scaled_cursor_diff);

  /**
   * @brief 获取经过当前视图缩放和平移变换后的光标位置。
   * @param cursor_pos 原始光标位置。
   * @return 变换后的光标位置。
   */
  QPointF GetScaledCursorPos(const QPointF &cursor_pos);

  QHash<NodeKeyframeTrackReference, QColor> keyframe_colors_;  ///< 存储每个轨道的显示颜色。
  QHash<NodeKeyframeTrackReference, KeyframeViewInputConnection *>
      track_connections_;  ///< 存储已连接轨道及其对应的连接对象。

  int text_padding_;  ///< 绘制文本时的内边距。

  int minimum_grid_space_;  ///< 网格线的最小间距。

  QVector<NodeKeyframeTrackReference> connected_inputs_;  ///< 存储所有已连接输入轨道的引用列表。

  /**
   * @brief BezierPoint 结构体用于表示和管理可交互的贝塞尔控制点。
   */
  struct BezierPoint {
    QRectF rect;                    ///< 控制点在视图中的包围矩形，用于拾取。
    NodeKeyframe *keyframe{};       ///< 此控制点所属的关键帧。
    NodeKeyframe::BezierType type;  ///< 控制点的类型（入控制柄或出控制柄）。
  };

  QVector<BezierPoint> bezier_pts_;        ///< 存储当前视图中所有可见的贝塞尔控制点。
  const BezierPoint *dragging_bezier_pt_;  ///< 指向当前正在被拖动的贝塞尔控制点的指针。

  QPointF dragging_bezier_point_start_;  ///< 拖动贝塞尔控制点开始时的原始位置。
  QPointF
      dragging_bezier_point_opposing_start_;  ///< 拖动贝塞尔控制点时，其对应（另一侧）控制点的起始位置（用于对称调整等）。
  QPointF drag_start_;                        ///< 拖动操作开始时的鼠标位置。

  QVector<QVariant> drag_keyframe_values_;  ///< 在拖动关键帧时，存储这些关键帧的原始值，用于计算相对变化。
};

}  // namespace olive

#endif  // CURVEVIEW_H
