#ifndef CURVEWIDGET_H
#define CURVEWIDGET_H

#include <QCheckBox>     // Qt 复选框控件
#include <QHBoxLayout>   // Qt 水平布局类
#include <QPushButton>   // Qt 按钮控件
#include <QWidget>       // Qt 控件基类

#include "curveview.h"                                 // 自定义的曲线视图控件
#include "widget/nodeparamview/nodeparamviewkeyframecontrol.h" // 节点参数视图中的关键帧控制相关
#include "widget/nodeparamview/nodeparamviewwidgetbridge.h"  // 节点参数视图与控件之间的桥接
#include "widget/nodetreeview/nodetreeview.h"          // 节点树视图控件
#include "widget/timebased/timebasedwidget.h"        // 基于时间的控件基类

// 前向声明 Qt 类
class QColor;
class QPointF; // 已在 curveview.h 中间接包含或使用
class QRectF;  // 已在 curveview.h 中间接包含或使用
class QVariant; // 已在 curveview.h 中间接包含或使用

namespace olive {

// 前向声明项目内部类
class Node;             // 节点基类
class ViewerOutput;     // 查看器输出类
class MultiUndoCommand; // 复合撤销命令类
// 结构体 NodeKeyframeTrackReference 和 KeyframeViewInputConnection 已在 curveview.h 中定义

/**
 * @brief CurveWidget 类是一个综合性的曲线编辑器控件。
 *
 * 它结合了一个 CurveView（用于显示和编辑动画曲线）、一个 NodeTreeView（用于显示和选择可动画化的节点和参数），
 * 以及一组控制按钮（用于设置关键帧插值类型）。
 * CurveWidget 继承自 TimeBasedWidget 和 TimeTargetObject，表明它与时间轴和播放目标相关联。
 */
class CurveWidget : public TimeBasedWidget, public TimeTargetObject {
  Q_OBJECT // Qt 元对象系统宏，用于支持信号和槽机制

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit CurveWidget(QWidget *parent = nullptr);

  /**
   * @brief 获取垂直方向的缩放比例。
   * @return 返回当前的垂直缩放比例 (double 类型)。
   */
  const double &GetVerticalScale();
  /**
   * @brief 设置垂直方向的缩放比例。
   * @param vscale 要设置的垂直缩放比例。
   */
  void SetVerticalScale(const double &vscale);

  /**
   * @brief 删除当前选中的关键帧。
   */
  void DeleteSelected();

  /**
   * @brief 选中视图中的所有关键帧。
   */
  void SelectAll() { view_->SelectAll(); }

  /**
   * @brief 取消选中视图中的所有关键帧。
   */
  void DeselectAll() { view_->DeselectAll(); }

  /**
   * @brief 根据 ID 获取选中的节点。
   * @param id 节点的唯一标识符。
   * @return 如果找到具有该 ID 的选中节点，则返回节点指针；否则返回 nullptr。
   */
  Node *GetSelectedNodeWithID(const QString &id);

  /**
   * @brief 复制选中的关键帧（支持剪切操作）。
   * @param cut 如果为 true，则执行剪切操作（复制后删除）；否则执行复制操作。
   * @return 如果操作成功（有选中的关键帧可供复制/剪切），则返回 true；否则返回 false。
   */
  bool CopySelected(bool cut) override;

  /**
   * @brief 粘贴之前复制或剪切的关键帧。
   * @return 如果粘贴操作成功，则返回 true；否则返回 false。
   */
  bool Paste() override;

 public slots:
  /**
   * @brief 设置要在曲线编辑器中显示的节点列表。
   *
   * 此函数会更新 NodeTreeView 和 CurveView 以反映新的节点集。
   * @param nodes 包含 Node 指针的 QVector。
   */
  void SetNodes(const QVector<Node *> &nodes);

 protected:
  /**
   * @brief 当时间基准（例如帧率）改变时调用的事件处理函数。
   * @param new_timebase 新的时间基准。
   */
  void TimebaseChangedEvent(const rational &) override;
  /**
   * @brief 当时间轴的缩放比例改变时调用的事件处理函数。
   * @param new_scale 新的缩放比例。
   */
  void ScaleChangedEvent(const double &) override;

  /**
   * @brief 当时间目标（例如当前播放头所在的查看器）改变时调用的事件处理函数。
   * @param target 新的时间目标 ViewerOutput 指针。
   */
  void TimeTargetChangedEvent(ViewerOutput *target) override;

  /**
   * @brief 当连接的节点（通常是时间目标所关联的节点）发生改变时的事件处理函数。
   * @param n 新的关联节点 ViewerOutput 指针。
   */
  void ConnectedNodeChangeEvent(ViewerOutput *n) override;

  /**
   * @brief 获取用于对齐操作的关键帧轨道列表。
   * @return 返回一个指向包含 KeyframeViewInputConnection 指针的 QVector 的 const 指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const QVector<KeyframeViewInputConnection *> *GetSnapKeyframes() const override {
    return &view_->GetKeyframeTracks();
  }

  /**
   * @brief 获取关键帧的时间目标对象。
   * @return 返回一个指向 TimeTargetObject 的 const 指针，这里通常是内部的 CurveView 实例。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const TimeTargetObject *GetKeyframeTimeTarget() const override { return view_; }

  /**
   * @brief 获取在对齐操作中应忽略的关键帧列表（通常是当前选中的关键帧）。
   * @return 返回一个指向包含 NodeKeyframe 指针的 std::vector 的 const 指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const std::vector<NodeKeyframe *> *GetSnapIgnoreKeyframes() const override {
    return &view_->GetSelectedKeyframes();
  }

 private:
  /**
   * @brief 设置关键帧类型按钮（线性、贝塞尔、保持）的启用状态。
   * @param enable 如果为 true，则启用按钮；否则禁用。
   */
  void SetKeyframeButtonEnabled(bool enable);

  /**
   * @brief 设置关键帧类型按钮的选中状态。
   * @param checked 如果为 true，则将按钮设置为选中状态；否则取消选中。
   *
   * 注意：这通常会根据当前选中的关键帧类型来更新一组按钮中某一个的选中状态。
   */
  void SetKeyframeButtonChecked(bool checked);

  /**
   * @brief 根据给定的关键帧插值类型设置相应类型按钮的选中状态。
   * @param type 关键帧的插值类型 (NodeKeyframe::Type)。
   */
  void SetKeyframeButtonCheckedFromType(NodeKeyframe::Type type);

  /**
   * @brief 连接指定节点的特定输入参数（或其元素，如向量的 x,y,z 分量）到 CurveView。
   * @param node 要连接的节点指针。
   * @param input 要连接的输入参数的名称。
   * @param element 如果参数是向量或多维的，则指定要连接的元素索引 (例如 0 代表 x, 1 代表 y)。
   */
  void ConnectInput(Node *node, const QString &input, int element);

  /**
   * @brief 内部实现的连接输入逻辑。
   * @param node 节点指针。
   * @param input 输入参数名称。
   * @param element 元素索引。
   */
  void ConnectInputInternal(Node *node, const QString &input, int element);

  QHash<NodeKeyframeTrackReference, QColor> keyframe_colors_; ///< 存储每个已连接轨道的显示颜色。

  NodeTreeView *tree_view_; ///< 用于显示可动画化节点和参数的树形视图控件。

  QPushButton *linear_button_; ///< 用于将选中关键帧设置为线性插值的按钮。
  QPushButton *bezier_button_; ///< 用于将选中关键帧设置为贝塞尔插值的按钮。
  QPushButton *hold_button_;   ///< 用于将选中关键帧设置为保持（阶梯）插值的按钮。

  CurveView *view_; ///< 内部的 CurveView 实例，实际显示和编辑曲线。

  NodeParamViewKeyframeControl *key_control_; ///< 可能用于控制关键帧添加/删除等操作的辅助控件。

  QVector<Node *> nodes_; ///< 当前在此曲线编辑器中加载的节点列表。

  QVector<NodeKeyframeTrackReference> selected_tracks_; ///< 当前在 NodeTreeView 中选中的轨道列表。

 private slots:
  /**
   * @brief 当 NodeTreeView 中的选择项发生改变时调用的槽函数。
   *
   * 此函数会更新 CurveView 中显示的轨道。
   */
  void SelectionChanged();

  /**
   * @brief 当关键帧插值类型按钮（线性、贝塞尔、保持）被点击时调用的槽函数。
   * @param checked 按钮的勾选状态（尽管对于非checkable按钮，此参数可能不直接使用）。
   */
  void KeyframeTypeButtonTriggered(bool checked);

  /**
   * @brief 当 CurveView 中的输入轨道选择发生改变时调用的槽函数。
   * @param ref 被选中或取消选中的轨道的引用。
   */
  void InputSelectionChanged(const NodeKeyframeTrackReference &ref);

  /**
   * @brief 当在 CurveView 中拖动关键帧时调用的槽函数。
   * @param x 鼠标的 x 坐标（可能未使用）。
   * @param y 鼠标的 y 坐标（可能未使用）。
   */
  void KeyframeViewDragged(int x, int y);
  /**
   * @brief 当在 CurveView 中释放鼠标（结束拖动关键帧）时调用的槽函数。
   */
  void KeyframeViewReleased();
};

}  // namespace olive

#endif  // CURVEWIDGET_H
