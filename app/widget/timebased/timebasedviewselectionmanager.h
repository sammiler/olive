#ifndef TIMEBASEDVIEWSELECTIONMANAGER_H // 防止头文件被多次包含的宏定义
#define TIMEBASEDVIEWSELECTIONMANAGER_H

#include <QGraphicsView> // 引入 QGraphicsView 类，用于显示 QGraphicsScene 中的内容
#include <QMouseEvent>   // 引入 QMouseEvent 类，用于处理鼠标事件
#include <QRubberBand>   // 引入 QRubberBand 类，用于创建橡皮筋选择框
#include <QToolTip>      // 引入 QToolTip 类，用于显示工具提示

#include "common/qtutils.h"    // 引入 Qt 工具类，可能包含一些辅助函数
#include "timebasedview.h"     // 引入 TimeBasedView 类，是此选择管理器所服务的视图
#include "timebasedwidget.h"   // 引入 TimeBasedWidget 类，可能用于吸附等功能
#include "widget/timetarget/timetarget.h" // 引入 TimeTargetObject 类，用于时间目标转换

// 前向声明 (如果需要)
namespace olive {
class UndoCommand; // 从 SetTimeCommand 的继承关系推断
class MultiUndoCommand; // 从 DragStop 参数推断
class Node; // 从 time_targets_ 和 GetParentOfType 推断
class Project; // 从 SetTimeCommand::GetRelevantProject 推断
class TimelineMarker; // 从模板特化推断
}


namespace olive {

/**
 * @brief TimeBasedViewSelectionManager 类是一个模板类，用于管理 TimeBasedView 中对象的选择、拖动和橡皮筋选择。
 *
 * @tparam T 被管理对象的类型。这个类型需要有一些特定的接口，例如 time() 和 set_time() 方法，
 * 以及 has_sibling_at_time()。对于 TimelineMarker 类型有特殊的拖动和吸附处理。
 */
template <typename T>
class TimeBasedViewSelectionManager {
 public:
  /**
   * @brief 构造一个 TimeBasedViewSelectionManager 对象。
   * @param view 指向关联的 TimeBasedView 对象的指针。
   */
  explicit TimeBasedViewSelectionManager(TimeBasedView *view)
      : view_(view), rubberband_(nullptr), snap_mask_(TimeBasedWidget::kSnapAll) {}

  /**
   * @brief 设置吸附掩码。
   * @param e 吸附掩码枚举值 (TimeBasedWidget::SnapMask)。
   */
  void SetSnapMask(TimeBasedWidget::SnapMask e) { snap_mask_ = e; }

  /**
   * @brief 清除所有已绘制对象的记录。
   *
   * 当视图中的对象需要重新声明其绘制区域时调用。
   */
  void ClearDrawnObjects() { drawn_objects_.clear(); }

  /**
   * @brief 声明一个已绘制的对象及其在场景中的矩形区域。
   *
   * 矩形区域会从视图坐标转换为非缩放的场景坐标进行存储。
   * @param object 指向已绘制对象的指针。
   * @param rect 对象在视图坐标系中的矩形区域 (QRectF)。
   */
  void DeclareDrawnObject(T *object, const QRectF &rect) {
    // 将视图坐标的矩形转换为非缩放的场景坐标
    QRectF r(view_->UnscalePoint(rect.topLeft()), view_->UnscalePoint(rect.bottomRight()));
    drawn_objects_.push_back({object, r}); // 存储对象及其非缩放的场景矩形
  }

  /**
   * @brief 选择一个对象。
   * @param key 指向要选择的对象的指针。
   * @return 如果对象成功被选择（之前未被选择），则返回 true；否则返回 false。
   */
  bool Select(T *key) {
    Q_ASSERT(key); // 断言确保 key 不为 nullptr

    if (!IsSelected(key)) { // 如果对象尚未被选择
      selected_.push_back(key); // 将对象添加到已选择列表
      return true;
    }

    return false; // 对象已被选择
  }

  /**
   * @brief 取消选择一个对象。
   * @param key 指向要取消选择的对象的指针。
   * @return 如果对象成功被取消选择（之前已被选择），则返回 true；否则返回 false。
   */
  bool Deselect(T *key) {
    Q_ASSERT(key); // 断言确保 key 不为 nullptr

    auto it = std::find(selected_.cbegin(), selected_.cend(), key); // 查找对象
    if (it == selected_.cend()) { // 如果未找到
      return false;
    } else {
      selected_.erase(it); // 从已选择列表中移除
      return true;
    }
  }

  /**
   * @brief 清除所有已选择的对象。
   */
  void ClearSelection() { selected_.clear(); }

  /**
   * @brief 检查一个对象是否已被选择。
   * @param key 指向要检查的对象的指针。
   * @return 如果对象已被选择，则返回 true；否则返回 false。
   */
  bool IsSelected(T *key) const { return std::find(selected_.cbegin(), selected_.cend(), key) != selected_.cend(); }

  /**
   * @brief 获取当前所有已选择对象的列表。
   * @return 对已选择对象指针向量的常量引用。
   */
  [[nodiscard]] const std::vector<T *> &GetSelectedObjects() const { return selected_; }

  /**
   * @brief 设置时间基准（例如帧率）。
   * @param tb 新的时间基准 (rational)。
   */
  void SetTimebase(const rational &tb) { timebase_ = tb; }

  /**
   * @brief 获取指定场景坐标点处的对象。
   *
   * 迭代顺序是反向的，因为后绘制的对象会显示在用户视图的顶层。
   * @param scene_pt 场景中的坐标点 (QPointF)。
   * @return 如果找到对象，则返回指向该对象的指针；否则返回 nullptr。
   */
  T *GetObjectAtPoint(const QPointF &scene_pt) {
    // 将场景坐标转换为非缩放坐标以进行命中测试
    QPointF unscaled = view_->UnscalePoint(scene_pt);
    // 反向迭代，因为后绘制的对象在顶层
    for (auto it = drawn_objects_.crbegin(); it != drawn_objects_.crend(); it++) {
      const DrawnObject &kp = *it; // kp 是 QPair<T*, QRectF>
      if (kp.second.contains(unscaled)) { // 检查点是否在对象的非缩放矩形内
        return kp.first; // 返回找到的对象
      }
    }
    return nullptr; // 未找到对象
  }

  /**
   * @brief 获取指定视图坐标点处的对象。
   *
   * 此方法会将视图坐标转换为场景坐标，然后调用另一个 GetObjectAtPoint 版本。
   * @param pt 视图中的坐标点 (QPoint)。
   * @return 如果找到对象，则返回指向该对象的指针；否则返回 nullptr。
   */
  T *GetObjectAtPoint(const QPoint &pt) { return GetObjectAtPoint(view_->mapToScene(pt)); }

  /**
   * @brief 处理鼠标按下事件。
   *
   * 根据鼠标按键和修饰键（如 Shift）来处理对象的选择和取消选择。
   * @param event QMouseEvent 指针，包含鼠标事件参数。
   * @return 如果光标下有对象并且该对象被选中（或保持选中），则返回指向该对象的指针；否则返回 nullptr。
   */
  T *MousePress(QMouseEvent *event) {
    T *key_under_cursor = nullptr; // 光标下的对象指针

    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) { // 只处理左键或右键按下
      key_under_cursor = GetObjectAtPoint(event->pos()); // 获取光标下的对象

      bool holding_shift = event->modifiers() & Qt::ShiftModifier; // 检查是否按下了 Shift 键

      if (!key_under_cursor || !IsSelected(key_under_cursor)) { // 如果光标下没有对象，或者对象未被选中
        if (!holding_shift) { // 如果没有按住 Shift
          // 清除当前选择
          ClearSelection();
        }
        // 将光标下的对象（如果存在）添加到选择中
        if (key_under_cursor) {
          Select(key_under_cursor);
          view_->SelectionManagerSelectEvent(key_under_cursor); // 通知视图对象被选中
        }
      } else if (holding_shift) { // 如果对象已被选中且按住了 Shift
        // 取消选择此对象，但不做其他操作
        Deselect(key_under_cursor);
        view_->SelectionManagerDeselectEvent(key_under_cursor); // 通知视图对象被取消选择
        key_under_cursor = nullptr; // 返回 nullptr，因为对象已被取消选择
      }
    }
    return key_under_cursor; // 返回最终在光标下且被选中的对象（或 nullptr）
  }

  /**
   * @brief 检查当前是否正在拖动对象。
   * @return 如果正在拖动，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool IsDragging() const { return !dragging_.empty(); }

  /**
   * @brief 开始拖动操作。
   *
   * 记录拖动开始时的状态，包括选中的对象、初始时间、鼠标位置等。
   * @param initial_item 指向拖动开始时鼠标下的对象的指针。
   * @param event QMouseEvent 指针，包含鼠标事件参数。
   * @param target 可选的时间目标对象 (TimeTargetObject*)，用于时间转换。
   */
  void DragStart(T *initial_item, QMouseEvent *event, TimeTargetObject *target = nullptr) {
    if (event->button() != Qt::LeftButton) { // 只响应左键拖动
      return;
    }

    time_target_ = target; // 存储时间目标对象

    initial_drag_item_ = initial_item; // 存储初始拖动项

    dragging_.resize(selected_.size()); // 调整拖动时间点向量的大小

    // 根据对象类型（特别是 TimelineMarker）调整吸附点向量的大小
    if constexpr (std::is_same_v<T, TimelineMarker>) { // TimelineMarker 有入点和出点
      snap_points_.resize(selected_.size() * 2);
    } else {
      snap_points_.resize(selected_.size());
    }

    if (target) { // 如果提供了时间目标对象
      time_targets_.resize(snap_points_.size()); // 调整时间目标节点向量的大小
      // 初始化时间目标节点为 nullptr
      memset(time_targets_.data(), 0, time_targets_.size() * sizeof(Node *));
    } else {
      time_targets_.clear(); // 清空时间目标节点向量
    }

    // 记录每个选中对象的初始拖动时间和吸附点
    for (size_t i = 0; i < selected_.size(); i++) {
      T *obj = selected_.at(i);

      if constexpr (std::is_same_v<T, TimelineMarker>) { // 特殊处理 TimelineMarker
        dragging_[i] = obj->time().in(); // 记录入点时间
        snap_points_[i] = obj->time().in(); // 入点作为吸附点
        snap_points_[i + selected_.size()] = obj->time().out(); // 出点作为吸附点

        if (target) { // 如果有时间目标，获取父节点
          time_targets_[i] = time_targets_[i + selected_.size()] = QtUtils::GetParentOfType<Node>(obj);
        }
      } else { // 其他类型对象
        dragging_[i] = obj->time(); // 记录对象时间
        snap_points_[i] = obj->time(); // 对象时间作为吸附点

        if (target) { // 如果有时间目标，获取父节点
          time_targets_[i] = QtUtils::GetParentOfType<Node>(obj);
        }
      }
    }
    // 记录拖动开始时鼠标在非缩放场景坐标系中的位置
    drag_mouse_start_ = view_->UnscalePoint(view_->mapToScene(event->pos()));
  }

  /**
   * @brief 对吸附点进行吸附处理。
   * @param movement 指向表示时间移动量的 rational 对象的指针，此值可能会被吸附逻辑修改。
   */
  void SnapPoints(rational *movement) {
    std::vector<rational> copy = snap_points_; // 复制吸附点用于计算

    if (time_target_) { // 如果存在时间目标对象，进行时间转换
      for (size_t i = 0; i < copy.size(); i++) {
        if (Node *parent = time_targets_[i]) { // 如果该吸附点有关联的父节点
          // 将吸附点从其父节点的时间上下文转换到目标时间上下文
          copy[i] = time_target_->GetAdjustedTime(parent, time_target_->GetTimeTarget(), copy[i],
                                                  Node::kTransformTowardsOutput);
        }
      }
    }

    // 如果启用了全局吸附且视图有吸附服务
    if (Core::instance()->snapping() && view_->GetSnapService()) {
      // 调用吸附服务进行吸附计算，可能会修改 movement
      view_->GetSnapService()->SnapPoint(copy, movement, snap_mask_);
    }
  }

  /**
   * @brief 取消吸附状态，隐藏吸附提示。
   */
  void Unsnap() {
    if (view_->GetSnapService()) { // 如果视图有吸附服务
      view_->GetSnapService()->HideSnaps(); // 隐藏吸附提示线
    }
  }

  /**
   * @brief 处理拖动过程中的鼠标移动事件。
   *
   * 计算时间差，处理吸附，验证移动的有效性，并更新选中对象的时间。
   * 同时显示工具提示。
   * @param local_pos 鼠标在视图中的当前本地坐标 (QPoint)。
   * @param tip_format 可选的工具提示格式字符串，默认为空。
   */
  void DragMove(const QPoint &local_pos, const QString &tip_format = QString()) {
    // 计算鼠标从拖动开始点到当前点的水平位移，并转换为时间差
    rational time_diff =
        view_->SceneToTimeNoGrid(view_->mapToScene(local_pos).x() - view_->ScalePoint(drag_mouse_start_).x());

    rational presnap_time_diff = time_diff; // 保存吸附前的时间差
    SnapPoints(&time_diff); // 对时间差进行吸附处理

    // 验证吸附结果的有效性
    if (Core::instance()->snapping() && view_->GetSnapService()) {
      for (size_t i = 0; i < selected_.size(); i++) {
        rational proposed_time = dragging_.at(i) + time_diff; // 计算提议的新时间
        T *sel = selected_.at(i); // 获取当前选中的对象

        if (sel->has_sibling_at_time(proposed_time)) { // 如果提议的时间点已有同级对象
          // 取消吸附，恢复到吸附前的时间差
          time_diff = presnap_time_diff;
          if (view_->GetSnapService()) {
            view_->GetSnapService()->HideSnaps(); // 隐藏吸附提示
          }
          break; // 停止进一步验证
        }
      }
    }

    // 验证移动的有效性（例如，防止重叠或移出边界）
    for (size_t i = 0; i < selected_.size(); i++) {
      rational proposed_time = dragging_.at(i) + time_diff; // 计算提议的新时间
      T *sel = selected_.at(i); // 获取当前选中的对象

      // 使用一个微小的时间间隔 (1ms) 来避免精确的碰撞判断问题
      rational adj(1, 1000);
      if (dragging_.at(i) < proposed_time) { // 如果是从左向右拖动
        adj = -adj; // 调整值为负，用于向左微调
      }

      bool loop; // 循环标记，用于处理多次调整的情况
      do {
        loop = false;
        // 检查提议的时间点是否有同级对象，如果有，则微调提议的时间
        while (sel->has_sibling_at_time(proposed_time)) {
          proposed_time += adj; // 微调
          Unsnap(); // 取消吸附状态，因为微调可能导致不再吸附
        }

        if (proposed_time < rational(0)) { // 如果提议的时间小于0
          proposed_time = rational(0); // 将提议的时间设置为0
          Unsnap(); // 取消吸附

          // 将时间设置为0后，可能再次与0位置的同级对象冲突，
          // 因此需要重新检查。为避免负的 adj 导致再次小于0，强制 adj 为正。
          adj = rational::qAbs(adj);
          loop = true; // 需要再次循环检查
        }
      } while (loop);

      time_diff = proposed_time - dragging_.at(i); // 更新最终的时间差
    }

    // 应用计算出的时间差来移动所有选中的对象
    for (size_t i = 0; i < selected_.size(); i++) {
      selected_.at(i)->set_time(dragging_.at(i) + time_diff);
    }

    // 显示关于当前拖动项的工具提示信息
    rational display_time; // 用于显示的时间

    if constexpr (std::is_same_v<T, TimelineMarker>) { // 对 TimelineMarker 特殊处理
      display_time = initial_drag_item_->time().in(); // 获取入点时间
    } else {
      display_time = initial_drag_item_->time(); // 获取对象时间
    }

    // 将时间转换为时间码字符串
    QString tip = QString::fromStdString(
        Timecode::time_to_timecode(display_time, timebase_, Core::instance()->GetTimecodeDisplay(), false));

    last_used_tip_format_ = tip_format; // 保存上一次使用的提示格式
    if (!tip_format.isEmpty()) { // 如果提供了格式字符串
      tip = tip_format.arg(tip); // 应用格式
    }

    QToolTip::hideText(); // 隐藏旧的工具提示
    QToolTip::showText(QCursor::pos(), tip); // 在当前光标位置显示新的工具提示
  }

  /**
   * @brief 停止拖动操作。
   *
   * 清理拖动状态，并将拖动操作记录到撤销命令中。
   * @param command 指向 MultiUndoCommand 对象的指针，用于聚合多个撤销命令。
   */
  void DragStop(MultiUndoCommand *command) {
    QToolTip::hideText(); // 隐藏工具提示

    // 为每个被拖动的选中对象创建一个 SetTimeCommand，并添加到聚合命令中
    for (size_t i = 0; i < selected_.size(); i++) {
      rational current_time; // 当前时间
      if constexpr (std::is_same_v<T, TimelineMarker>) { // TimelineMarker 的特殊处理
        current_time = selected_.at(i)->time().in();
      } else {
        current_time = selected_.at(i)->time();
      }
      // 创建设置时间的撤销命令，记录新旧时间
      command->add_child(new SetTimeCommand(selected_.at(i), current_time, dragging_.at(i)));
    }

    dragging_.clear(); // 清空拖动状态数据
    Unsnap(); // 取消吸附状态
  }

  /**
   * @brief 开始橡皮筋选择操作。
   * @param event QMouseEvent 指针，包含鼠标事件参数。
   */
  void RubberBandStart(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) { // 只响应左键或右键
      // 记录橡皮筋开始时的非缩放场景坐标
      rubberband_scene_start_ = view_->UnscalePoint(view_->mapToScene(event->pos()));

      // 创建 QRubberBand 对象
      rubberband_ = new QRubberBand(QRubberBand::Rectangle, view_);
      // 设置橡皮筋的初始几何形状（一个点）
      rubberband_->setGeometry(QRect(event->pos().x(), event->pos().y(), 0, 0));
      rubberband_->show(); // 显示橡皮筋

      rubberband_preselected_ = selected_; // 保存开始橡皮筋选择前的已选对象
    }
  }

  /**
   * @brief 处理橡皮筋选择过程中的鼠标移动事件。
   *
   * 更新橡皮筋的几何形状，并根据橡皮筋覆盖的区域更新选择的对象。
   * @param pos 鼠标在视图中的当前本地坐标 (QPoint)。
   */
  void RubberBandMove(const QPoint &pos) {
    if (IsRubberBanding()) { // 如果正在进行橡皮筋选择
      // 计算橡皮筋在视图坐标系中的矩形，并规范化（确保左上角坐标小于右下角）
      QRectF band_rect = QRectF(view_->mapFromScene(view_->ScalePoint(rubberband_scene_start_)), pos).normalized();
      rubberband_->setGeometry(band_rect.toRect()); // 更新橡皮筋的几何形状

      // 计算橡皮筋在非缩放场景坐标系中的矩形
      QPointF current_unscaled_scene_pos = view_->UnscalePoint(view_->mapToScene(pos));
      QRectF scene_rect = QRectF(rubberband_scene_start_, current_unscaled_scene_pos).normalized();

      selected_ = rubberband_preselected_; // 恢复到橡皮筋选择开始前的选择状态
      // 遍历所有已绘制的对象
      foreach (const DrawnObject &kp, drawn_objects_) {
        if (scene_rect.intersects(kp.second)) { // 如果对象的非缩放场景矩形与橡皮筋矩形相交
          Select(kp.first); // 选择该对象
        }
      }
    }
  }

  /**
   * @brief 停止橡皮筋选择操作。
   *
   * 删除橡皮筋对象。
   */
  void RubberBandStop() {
    if (IsRubberBanding()) { // 如果正在进行橡皮筋选择
      delete rubberband_;    // 删除 QRubberBand 对象
      rubberband_ = nullptr; // 将指针置空
    }
  }

  /**
   * @brief 检查当前是否正在进行橡皮筋选择。
   * @return 如果正在进行橡皮筋选择，则返回 true；否则返回 false。
   */
  [[nodiscard]] bool IsRubberBanding() const { return rubberband_; }

  /**
   * @brief 强制更新拖动或橡皮筋选择的状态。
   *
   * 当视图外部发生可能影响拖动或选择状态的事件时（例如，滚动），
   * 可以调用此方法来确保选择和显示保持最新。
   */
  void ForceDragUpdate() {
    if (IsRubberBanding() || IsDragging()) { // 如果正在进行橡皮筋选择或拖动
      // 获取当前全局光标位置，并转换为视图的本地坐标
      QPoint local_pos = view_->viewport()->mapFromGlobal(QCursor::pos());
      if (IsRubberBanding()) {
        RubberBandMove(local_pos); // 更新橡皮筋选择
      } else {
        DragMove(local_pos, last_used_tip_format_); // 更新拖动
      }
    }
  }

 private:
  /**
   * @brief SetTimeCommand 类是一个用于撤销/重做设置对象时间操作的命令。
   *
   * 它继承自 UndoCommand。
   */
  class SetTimeCommand : public UndoCommand {
   public:
    /**
     * @brief 构造一个 SetTimeCommand 对象。
     * @param key 指向要修改时间的对象。
     * @param time 新的时间值。
     */
    SetTimeCommand(T *key, const rational &time) {
      key_ = key;
      new_time_ = time;
      old_time_ = key_->time(); // 保存旧的时间值以供撤销
    }

    /**
     * @brief 构造一个 SetTimeCommand 对象，并直接指定新旧时间。
     * @param key 指向要修改时间的对象。
     * @param new_time 新的时间值。
     * @param old_time 旧的时间值。
     */
    SetTimeCommand(T *key, const rational &new_time, const rational &old_time) {
      key_ = key;
      new_time_ = new_time;
      old_time_ = old_time;
    }

    /**
     * @brief 获取与此命令相关的 Project 对象。
     * @return 指向 Project 对象的指针。
     */
    [[nodiscard]] Project *GetRelevantProject() const override { return Project::GetProjectFromObject(key_); }

   protected:
    /**
     * @brief 执行重做操作，将对象的时间设置为新时间。
     */
    void redo() override { key_->set_time(new_time_); }

    /**
     * @brief 执行撤销操作，将对象的时间恢复为旧时间。
     */
    void undo() override { key_->set_time(old_time_); }

   private:
    /**
     * @brief 指向被操作对象的指针。
     */
    T *key_;

    /**
     * @brief 操作前的旧时间值。
     */
    rational old_time_;

    /**
     * @brief 操作后的新时间值。
     */
    rational new_time_;
  };

  /**
   * @brief 指向关联的 TimeBasedView 对象的指针。
   */
  TimeBasedView *view_;

  /**
   * @brief 定义 DrawnObject 类型，为一个 QPair，包含对象指针和其在场景中的矩形。
   */
  using DrawnObject = QPair<T *, QRectF>;

  /**
   * @brief 存储所有已声明的绘制对象及其矩形区域的向量。
   */
  std::vector<DrawnObject> drawn_objects_;

  /**
   * @brief 存储当前所有已选择对象的指针的向量。
   */
  std::vector<T *> selected_;

  /**
   * @brief 存储拖动过程中每个选中对象的初始时间点的向量。
   */
  std::vector<rational> dragging_;

  /**
   * @brief 存储用于吸附计算的点的向量。
   * 对于 TimelineMarker，可能包含入点和出点。
   */
  std::vector<rational> snap_points_;

  /**
   * @brief 存储与吸附点关联的父节点 (Node) 的向量，用于时间目标转换。
   */
  std::vector<Node *> time_targets_;

  /**
   * @brief 指向拖动操作开始时鼠标下的初始对象的指针。
   */
  T *initial_drag_item_;

  /**
   * @brief 拖动开始时鼠标在非缩放场景坐标系中的位置。
   */
  QPointF drag_mouse_start_;

  /**
   * @brief 当前的时间基准（例如帧率）。
   */
  rational timebase_;

  /**
   * @brief 指向 QRubberBand 对象的指针，用于橡皮筋选择。如果不在橡皮筋选择模式，则为 nullptr。
   */
  QRubberBand *rubberband_;

  /**
   * @brief 橡皮筋选择开始时鼠标在非缩放场景坐标系中的位置。
   */
  QPointF rubberband_scene_start_;

  /**
   * @brief 在开始橡皮筋选择之前已选择的对象的列表。
   */
  std::vector<T *> rubberband_preselected_;

  /**
   * @brief 当前的吸附掩码，用于控制吸附行为。
   */
  TimeBasedWidget::SnapMask snap_mask_;

  /**
   * @brief 指向当前时间目标对象 (TimeTargetObject) 的指针，用于在拖动时进行时间转换。
   */
  TimeTargetObject *time_target_{};

  /**
   * @brief 上一次用于显示工具提示的格式字符串。
   */
  QString last_used_tip_format_;
};

}  // namespace olive

#endif  // TIMEBASEDVIEWSELECTIONMANAGER_H
