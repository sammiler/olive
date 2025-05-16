#ifndef NODEVALUETREE_H
#define NODEVALUETREE_H

#include <QRadioButton> // Qt 单选按钮控件 (可能用于树中的项)
#include <QTreeWidget>  // Qt 树形控件基类

#include "node/node.h"   // 节点基类定义 (NodeInput 依赖于此)
#include "node/param.h"  // 节点参数相关定义 (包含 NodeInput)
#include "olive/core/core.h" // 包含 rational 等核心类型

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QWidget;
// class QEvent;
// class QTreeWidgetItem; // QTreeWidget 会处理

// 前向声明项目内部类 (根据用户要求，不添加)
// class NodeInput; // 已在 node/param.h 中定义
// class rational;  // 已在 olive/core/core.h 中定义

namespace olive {

/**
 * @brief NodeValueTree 类是一个用于以树形结构显示节点输入参数在特定时间点的值的控件。
 *
 * 它继承自 QTreeWidget。当给定一个 NodeInput 和一个时间点时，
 * 此控件会填充树形结构来展示该输入参数在那个时间点的具体值。
 * 如果参数是复合类型（例如向量、颜色、结构体），则会分层显示其各个分量。
 * 可能包含交互元素，如单选按钮，用于选择或切换某些值的表示方式。
 */
class NodeValueTree : public QTreeWidget {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit NodeValueTree(QWidget *parent = nullptr);

  /**
   * @brief 设置要在树中显示的节点输入参数及其在特定时间的值。
   *
   * 此函数会清除树的现有内容，并根据指定的输入参数和时间点重新填充树。
   * @param input 要显示的节点输入参数 (NodeInput)。
   * @param time 要获取参数值的时间点 (rational 类型)。
   */
  void SetNode(const NodeInput &input, const rational &time);

protected:
  /**
   * @brief 处理 Qt 的 changeEvent 事件。
   *
   * 主要用于响应 QEvent::LanguageChange 事件，以便在应用程序语言设置更改时
   * 调用 Retranslate() 方法来更新树中各项的文本（如果适用）。
   * @param event 指向 QEvent 对象的指针。
   */
  void changeEvent(QEvent *event) override;

private:
  /**
   * @brief 重新翻译树视图中所有项的文本。
   *
   * 此方法会在构造时以及语言更改时被调用，以确保任何可翻译的文本都得到更新。
   */
  void Retranslate();

private slots:
 /**
  * @brief 当树中的某个单选按钮 (QRadioButton) 的选中状态改变时调用的槽函数。
  * @param e 单选按钮新的选中状态 (true 表示被选中)。
  */
 void RadioButtonChecked(bool e);
};

}  // namespace olive

#endif  // NODEVALUETREE_H
