#ifndef NODETABLEPANEL_H  // 防止头文件被重复包含的宏
#define NODETABLEPANEL_H  // 定义 NODETABLEPANEL_H 宏

#include "panel/timebased/timebased.h"             // 包含 TimeBasedPanel 基类的定义
#include "widget/nodetableview/nodetablewidget.h"  // 包含 NodeTableWidget 控件的定义

namespace olive {  // olive 项目的命名空间

/**
 * @brief NodeTablePanel 类代表一个以表格形式显示节点信息的面板。
 *
 * 它继承自 TimeBasedPanel，表明它可能与时间相关，或者至少其内容 (节点) 是时间相关的。
 * 这个面板内部通常会包含一个 NodeTableWidget 控件，该控件负责以表格的行列结构
 * 来展示节点的各种属性、参数或状态。
 *
 * 这种视图对于批量查看或比较多个节点的特定信息可能很有用，
 * 例如，查看所有选中节点的某个共同参数的值，或者它们的启用/禁用状态等。
 * 它提供了一种不同于图形化节点编辑器的节点信息展示方式。
 */
class NodeTablePanel : public TimeBasedPanel {  // NodeTablePanel 继承自 TimeBasedPanel
 Q_OBJECT                                       // 声明此类使用 Qt 的元对象系统

     public :
     // 构造函数
     NodeTablePanel();

 public slots:  // Qt 公有槽函数
  /**
   * @brief 在内部的 NodeTableWidget 中选择指定的节点列表。
   * @param nodes 要选择的节点指针的 QVector。
   */
  void SelectNodes(const QVector<Node*>& nodes) {
    // 获取内部的 TimeBasedWidget，并动态转换为 NodeTableWidget，然后调用其 SelectNodes 方法
    dynamic_cast<NodeTableWidget*>(GetTimeBasedWidget())->SelectNodes(nodes);
  }

  /**
   * @brief 在内部的 NodeTableWidget 中取消选择指定的节点列表。
   * @param nodes 要取消选择的节点指针的 QVector。
   */
  void DeselectNodes(const QVector<Node*>& nodes) {
    // 获取内部的 TimeBasedWidget，并动态转换为 NodeTableWidget，然后调用其 DeselectNodes 方法
    dynamic_cast<NodeTableWidget*>(GetTimeBasedWidget())->DeselectNodes(nodes);
  }

 private:
  /**
   * @brief 重写基类的 Retranslate 方法，用于在语言更改时更新此面板内UI元素的文本。
   * 例如，可能会更新面板标题或表格的列标题。
   */
  void Retranslate() override;

  // 注意：实际的 NodeTableWidget 成员变量通常在构造函数中创建，并通过 TimeBasedPanel
  // (或其父类 PanelWidget) 的机制 (如 setCentralWidget 或类似的) 设置为面板的中心内容。
  // 此处的槽函数直接通过 GetTimeBasedWidget() 和 dynamic_cast 来访问它。
};

}  // namespace olive

#endif  // NODETABLEPANEL_H