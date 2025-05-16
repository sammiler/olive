#ifndef NODETABLEWIDGET_H
#define NODETABLEWIDGET_H

#include "nodetableview.h"                    // 节点表格/树形视图控件
#include "widget/timebased/timebasedwidget.h" // 基于时间的控件基类

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QWidget;
// class QVector;
// class Node; // 已在 nodetableview.h 中包含

// 前向声明项目内部类 (根据用户要求，不添加)
// class rational; // 已在 timebasedwidget.h 中间接包含

namespace olive {

/**
 * @brief NodeTableWidget 类是一个包含 NodeTableView 的复合控件。
 *
 * 它继承自 TimeBasedWidget，表明它与时间轴同步。
 * 此控件主要作为 NodeTableView 的一个包装器或管理器，
 * 将选择节点和时间变化等操作委托给内部的 NodeTableView 实例。
 */
class NodeTableWidget : public TimeBasedWidget {
  // Q_OBJECT // 通常 QWidget 子类如果定义了自定义信号或槽才需要，此处遵循不修改原则。
public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit NodeTableWidget(QWidget* parent = nullptr);

  /**
   * @brief 在内部的 NodeTableView 中选中指定的节点列表。
   * @param nodes 包含要选中的 Node 指针的 QVector。
   */
  void SelectNodes(const QVector<Node*>& nodes) { view_->SelectNodes(nodes); }

  /**
   * @brief 在内部的 NodeTableView 中取消选中指定的节点列表。
   * @param nodes 包含要取消选中的 Node 指针的 QVector。
   */
  void DeselectNodes(const QVector<Node*>& nodes) { view_->DeselectNodes(nodes); }

protected:
  /**
   * @brief 当关联的时间目标的时间发生改变时调用的事件处理函数。
   *
   * 此函数会将新的时间值传递给内部的 NodeTableView 实例。
   * @param time 新的当前时间 (rational 类型)。
   */
  void TimeChangedEvent(const rational& time) override { view_->SetTime(time); }

private:
  NodeTableView* view_; ///< 指向内部 NodeTableView 实例的指针，用于实际显示和管理节点列表。
};

}  // namespace olive

#endif  // NODETABLEWIDGET_H
