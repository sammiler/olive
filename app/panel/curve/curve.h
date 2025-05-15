#ifndef CURVEPANEL_H // 防止头文件被重复包含的宏
#define CURVEPANEL_H // 定义 CURVEPANEL_H 宏

#include "panel/timebased/timebased.h"     // 包含 TimeBasedPanel 基类的定义
#include "widget/curvewidget/curvewidget.h" // 包含 CurveWidget 控件的定义

namespace olive { // olive 项目的命名空间

/**
 * @brief CurvePanel 类代表一个用于显示和编辑曲线 (通常是参数关键帧动画曲线) 的面板。
 *
 * 它继承自 TimeBasedPanel，表明这是一个与时间相关的面板，可能会有时间轴或时间导航功能。
 * CurvePanel 内部通常会包含一个 CurveWidget 控件，该控件负责实际的曲线绘制和交互。
 * 用户可以通过这个面板来调整关键帧的值、时间、以及关键帧之间的插值曲线 (例如贝塞尔曲线控制柄)。
 */
class CurvePanel : public TimeBasedPanel { // CurvePanel 继承自 TimeBasedPanel
  Q_OBJECT // 声明此类使用 Qt 的元对象系统

 public:
  // 构造函数
  CurvePanel();

  /**
   * @brief 重写基类或接口方法，用于删除当前选中的曲线元素 (例如关键帧或控制柄)。
   */
  void DeleteSelected() override;

  /**
   * @brief 重写基类或接口方法，用于全选曲线编辑器中的所有可选元素。
   */
  void SelectAll() override;

  /**
   * @brief 重写基类或接口方法，用于取消选择曲线编辑器中的所有元素。
   */
  void DeselectAll() override;

public slots: // Qt 公有槽函数
 /**
  * @brief 设置曲线编辑器当前要显示的单个节点。
  * 内部会将单个节点指针转换为包含单个节点的向量，并调用 SetNodes。
  * @param node 要显示的节点指针。如果为 nullptr，则等同于清空当前显示的节点。
  */
 void SetNode(Node *node) {
  // 将单个指针转换为空向量或包含一个元素的向量
  QVector<Node *> nodes; // 创建一个空的节点向量

  if (node) { // 如果节点指针有效
    nodes.append(node); // 将节点添加到向量中
  }

  SetNodes(nodes); // 调用 SetNodes 处理节点向量
}

  /**
   * @brief 设置曲线编辑器当前要显示的一组节点。
   * 曲线编辑器将显示这些节点的可编辑参数曲线。
   * @param nodes 包含要显示的节点指针的向量。
   */
  void SetNodes(const QVector<Node *> &nodes);

  /**
   * @brief 重写基类方法，用于增加曲线编辑器中轨道（参数曲线）的显示高度。
   */
  void IncreaseTrackHeight() override;

  /**
   * @brief 重写基类方法，用于减少曲线编辑器中轨道的显示高度。
   */
  void DecreaseTrackHeight() override;

protected:
  /**
   * @brief 重写基类的 Retranslate 方法，用于在语言更改时更新此面板内UI元素的文本。
   */
  void Retranslate() override;

  // 注意：CurveWidget 成员变量通常会在 .cpp 文件中定义和初始化，
  // 或者如果 Panel 基类有统一的 contentWidget() 机制，则通过该机制访问。
  // 此处头文件仅声明接口。
};

}  // namespace olive

#endif  // CURVEPANEL_H