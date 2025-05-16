#ifndef NODEPARAMVIEWITEM_H
#define NODEPARAMVIEWITEM_H

#include <QCheckBox>    // Qt 复选框控件
#include <QGridLayout>  // Qt 网格布局类
#include <QLabel>       // Qt 标签控件
#include <QPushButton>  // Qt 按钮控件
#include <QVBoxLayout>  // Qt 垂直布局类
#include <QWidget>      // Qt 控件基类

#include "node/node.h"                             // 节点基类定义
#include "nodeparamviewarraywidget.h"              // 节点参数视图中用于数组操作的控件
#include "nodeparamviewconnectedlabel.h"           // 节点参数视图中显示连接信息的标签
#include "nodeparamviewitembase.h"                 // 节点参数视图项的基类
#include "nodeparamviewkeyframecontrol.h"          // 节点参数视图中关键帧控制按钮
#include "nodeparamviewwidgetbridge.h"             // 节点参数视图与具体参数编辑控件之间的桥梁
#include "widget/clickablelabel/clickablelabel.h"  // 可点击标签控件
#include "widget/collapsebutton/collapsebutton.h"  // 可折叠/展开按钮控件
#include "widget/keyframeview/keyframeview.h"      // 关键帧视图控件

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QEvent;
// class QMouseEvent;
// class QHash;
// class QString;
// class rational;

// 前向声明项目内部类 (根据用户要求，不添加)
// class ViewerOutput;
// class NodeInput;
// class NodeInputPair; // 假设在 node/param.h 或 node/node.h 中定义
// class ProjectSerializer; // 假设在 node/project/serializer/serializer.h 中定义

namespace olive {

/**
 * @brief 定义节点参数视图中复选框的行为。
 */
enum NodeParamViewCheckBoxBehavior {
  kNoCheckBoxes,             ///< 不显示任何复选框。
  kCheckBoxesOn,             ///< 始终显示复选框（例如，用于启用/禁用参数）。
  kCheckBoxesOnNonConnected  ///< 仅当参数未连接到其他节点输出时显示复选框。
};

/**
 * @brief NodeParamViewItemBody 类是 NodeParamViewItem 的内部主体部分。
 *
 * 它负责为单个节点动态创建和布局所有参数的UI元素，
 * 包括参数标签、值编辑控件、关键帧按钮、连接指示器以及数组参数的控制按钮。
 */
class NodeParamViewItemBody : public QWidget {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param node 要显示其参数的 Node 指针。
      * @param create_checkboxes 复选框的显示行为策略。
      * @param parent 父控件指针，默认为 nullptr。
      */
     explicit NodeParamViewItemBody(Node *node, NodeParamViewCheckBoxBehavior create_checkboxes,
                                    QWidget *parent = nullptr);

  /**
   * @brief 设置此参数体关联的时间目标（通常是查看器输出）。
   * @param target 指向 ViewerOutput 对象的指针。
   */
  void SetTimeTarget(ViewerOutput *target);

  /**
   * @brief 重新翻译此参数体中所有UI元素的文本。
   */
  void Retranslate();

  /**
   * @brief 获取指定输入参数元素在垂直布局中的Y坐标（或相对位置）。
   * @param c 要查询的 NodeInput。
   * @return 返回Y坐标或一个指示相对位置的值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] int GetElementY(NodeInput c) const;

  /**
   * @brief 设置此参数体中包含的任何基于时间的控件的时间基准。
   * @param timebase 新的时间基准 (rational 类型)。
   */
  void SetTimebase(const rational &timebase);

  /**
   * @brief 设置指定输入参数的复选框（如果存在）的勾选状态。
   * @param input 目标 NodeInput 的引用。
   * @param e 布尔值，true 表示勾选，false 表示取消勾选。
   */
  void SetInputChecked(const NodeInput &input, bool e);

 signals:
  /**
   * @brief 当用户请求选择某个节点时发出此信号（例如，通过点击连接标签）。
   * @param node 指向要选择的 Node 对象的指针。
   */
  void RequestSelectNode(Node *node);

  /**
   * @brief 当数组参数的展开/折叠状态改变时发出此信号。
   * @param e 如果为 true，表示数组已展开；否则为折叠。
   */
  void ArrayExpandedChanged(bool e);

  /**
   * @brief 当某个输入参数旁边的可选复选框状态改变时发出此信号。
   * @param input 发生改变的 NodeInput 的引用。
   * @param e 复选框的新状态 (true 表示勾选)。
   */
  void InputCheckedChanged(const NodeInput &input, bool e);

  /**
   * @brief 请求在查看器中开始编辑文本（例如，当文本参数被激活时）。
   */
  void RequestEditTextInViewer();

 private:
  /**
   * @brief 为指定的输入参数（或其元素）创建并布局UI控件。
   * @param layout 用于放置控件的 QGridLayout 指针。
   * @param node 当前节点。
   * @param input 输入参数的名称。
   * @param element 如果参数是多维的，则为元素索引；否则通常为-1。
   * @param row_index 在网格布局中要放置控件的起始行索引。
   */
  void CreateWidgets(QGridLayout *layout, Node *node, const QString &input, int element, int row_index);

  /**
   * @brief 当输入参数的边连接状态改变时，更新UI（例如，显示或隐藏连接标签）。
   * @param input 发生连接状态改变的 NodeInput。
   */
  void UpdateUIForEdgeConnection(const NodeInput &input);

  /**
   * @brief 根据 NodeParamViewWidgetBridge 提供的控件，将它们放置到布局中。
   * @param layout 目标 QGridLayout。
   * @param bridge 包含要放置控件的 NodeParamViewWidgetBridge。
   * @param row 要放置控件的行号。
   */
  static void PlaceWidgetsFromBridge(QGridLayout *layout, NodeParamViewWidgetBridge *bridge, int row);

  /**
   * @brief 内部处理数组大小改变的逻辑，更新UI。
   * @param node 相关的节点。
   * @param input 数组参数的名称。
   * @param size 数组的新大小。
   */
  void InputArraySizeChangedInternal(Node *node, const QString &input, int size);

  /**
   * @brief InputUI 结构体用于存储与单个输入参数UI元素相关的指针和信息。
   */
  struct InputUI {
    InputUI();  ///< 默认构造函数。

    QLabel *main_label;                            ///< 参数名称标签。
    NodeParamViewWidgetBridge *widget_bridge;      ///< 连接参数实际编辑控件的桥梁。
    NodeParamViewConnectedLabel *connected_label;  ///< 显示输入连接信息的标签。
    NodeParamViewKeyframeControl *key_control;     ///< 关键帧控制按钮（例如，添加/删除关键帧）。
    QGridLayout *layout{};                         ///< 此参数项使用的局部布局。
    int row{};                                     ///< 此参数项在父布局中的行号。
    QPushButton *extra_btn;                        ///< 额外的按钮（例如，用于颜色参数的颜色拾取器）。
    QCheckBox *optional_checkbox;                  ///< 可选的复选框（例如，启用/禁用）。

    NodeParamViewArrayButton *array_insert_btn;  ///< 用于在数组中插入元素的按钮。
    NodeParamViewArrayButton *array_remove_btn;  ///< 用于从数组中移除元素的按钮。
  };

  QHash<NodeInput, InputUI> input_ui_map_;  ///< 存储每个 NodeInput 与其对应 InputUI 结构的映射。

  /**
   * @brief ArrayUI 结构体用于存储与数组参数整体UI相关的指针和信息。
   */
  struct ArrayUI {
    QWidget *widget;                       ///< 包含数组控件的容器 QWidget。
    int count;                             ///< 数组当前的元素数量。
    NodeParamViewArrayButton *append_btn;  ///< 用于在数组末尾追加元素的按钮。
  };

  /**
   * @brief 为指定的 InputUI 设置时间目标。
   * @param ui 要设置时间目标的 InputUI 结构体。
   */
  void SetTimeTargetOnInputUI(const InputUI &ui);
  /**
   * @brief 为指定的 InputUI 设置时间基准。
   * @param ui 要设置时间基准的 InputUI 结构体。
   */
  void SetTimebaseOnInputUI(const InputUI &ui);

  Node *node_;  ///< 指向此参数体所关联的 Node 对象的指针。

  QHash<NodeInputPair, ArrayUI> array_ui_;  ///< 存储每个数组类型输入参数 (NodeInputPair) 与其对应 ArrayUI 结构的映射。

  QHash<NodeInputPair, CollapseButton *> array_collapse_buttons_;  ///< 存储每个数组参数的折叠/展开按钮。

  rational timebase_;  ///< 当前的时间基准。

  ViewerOutput *time_target_;  ///< 当前的时间目标（通常是查看器）。

  NodeParamViewCheckBoxBehavior create_checkboxes_;  ///< 创建复选框的行为策略。

  QHash<NodeInputPair, NodeInputPair> input_group_lookup_;  ///< 可能用于查找输入参数所属的组。

  /**
   * @brief 关键帧控制按钮所在的列索引。
   *
   * 作为有效的“最大列”索引，因为关键帧按钮总是右对齐。
   */
  static const int kKeyControlColumn;  ///< 关键帧控制按钮的列号。

  static const int kArrayInsertColumn;  ///< 数组插入按钮的列号。
  static const int kArrayRemoveColumn;  ///< 数组移除按钮的列号。
  static const int kExtraButtonColumn;  ///< 额外按钮（如颜色拾取器）的列号。

  static const int kOptionalCheckBox;        ///< 可选复选框的列号。
  static const int kArrayCollapseBtnColumn;  ///< 数组折叠按钮的列号。
  static const int kLabelColumn;             ///< 参数标签的列号。
  static const int kWidgetStartColumn;       ///< 参数编辑控件开始的列号。
  static const int kMaxWidgetColumn;         ///< 参数编辑控件结束的最大列号。

 private slots:
  /**
   * @brief 当输入参数的连接状态（边）发生改变时调用的槽函数。
   * @param output 提供输出的节点（如果连接）。
   * @param input 发生连接状态改变的 NodeInput。
   */
  void EdgeChanged(Node *output, const NodeInput &input);

  /**
   * @brief 当数组参数的折叠/展开按钮被按下时调用的槽函数。
   * @param checked 按钮的勾选状态 (true 表示展开)。
   */
  void ArrayCollapseBtnPressed(bool checked);

  /**
   * @brief 当数组参数的大小发生改变时调用的槽函数。
   * @param input 发生大小改变的数组参数的名称。
   * @param old_sz 数组的旧大小。
   * @param size 数组的新大小。
   */
  void InputArraySizeChanged(const QString &input, int old_sz, int size);

  /**
   * @brief 当数组参数的“追加”按钮被点击时调用的槽函数。
   */
  void ArrayAppendClicked();

  /**
   * @brief 当数组参数的“插入”按钮被点击时调用的槽函数。
   */
  void ArrayInsertClicked();

  /**
   * @brief 当数组参数的“移除”按钮被点击时调用的槽函数。
   */
  void ArrayRemoveClicked();

  /**
   * @brief 切换数组参数的展开/折叠状态（通常由用户点击数组本身触发）。
   */
  void ToggleArrayExpanded();

  /**
   * @brief 当需要替换某个输入参数的编辑控件时（例如，参数类型改变）调用的槽函数。
   * @param input 需要替换控件的 NodeInput。
   */
  void ReplaceWidgets(const NodeInput &input);

  /**
   * @brief 为当前节点显示“速度/持续时间”对话框。
   */
  void ShowSpeedDurationDialogForNode();

  /**
   * @brief 当可选复选框的状态改变时调用的槽函数。
   * @param e 复选框的新状态。
   */
  void OptionalCheckBoxClicked(bool e);
};

/**
 * @brief NodeParamViewItem 类代表节点参数视图中的一个可显示项，通常对应一个节点的所有参数。
 *
 * 它继承自 NodeParamViewItemBase，并包含一个 NodeParamViewItemBody 实例来实际展示参数的UI。
 * 此类负责将节点数据与UI表示连接起来，并处理与时间轴和关键帧视图的同步。
 */
class NodeParamViewItem : public NodeParamViewItemBase {
 Q_OBJECT  // Qt 元对象系统宏

     public :
     /**
      * @brief 构造函数。
      * @param node 要显示其参数的 Node 指针。
      * @param create_checkboxes 复选框的显示行为策略。
      * @param parent 父控件指针，默认为 nullptr。
      */
     NodeParamViewItem(Node *node, NodeParamViewCheckBoxBehavior create_checkboxes, QWidget *parent = nullptr);

  /**
   * @brief 设置此参数项关联的时间目标。
   * @param target 指向 ViewerOutput 对象的指针。
   */
  void SetTimeTarget(ViewerOutput *target) {
    time_target_ = target;  // 存储时间目标

    body_->SetTimeTarget(target);  // 将时间目标传递给内部的参数体控件
  }

  /**
   * @brief 设置此参数项关联的时间基准。
   * @param timebase 新的时间基准 (rational 类型)。
   */
  void SetTimebase(const rational &timebase) {
    timebase_ = timebase;  // 存储时间基准

    body_->SetTimebase(timebase);  // 将时间基准传递给内部的参数体控件
  }

  /**
   * @brief 获取此参数项所属的上下文节点。
   * @return 返回上下文 Node 指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Node *GetContext() const { return ctx_; }

  /**
   * @brief 设置此参数项的上下文节点。
   * @param ctx 指向上下文 Node 的指针。
   */
  void SetContext(Node *ctx) { ctx_ = ctx; }

  /**
   * @brief 获取此参数项直接关联的节点。
   * @return 返回 Node 指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Node *GetNode() const { return node_; }

  /**
   * @brief 获取指定输入参数元素在垂直布局中的Y坐标。
   * @param c 要查询的 NodeInput。
   * @return 返回Y坐标。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] int GetElementY(const NodeInput &c) const;

  /**
   * @brief 设置指定输入参数的复选框的勾选状态。
   * @param input 目标 NodeInput。
   * @param e 布尔值，true 表示勾选。
   */
  void SetInputChecked(const NodeInput &input, bool e);

  /**
   * @brief 获取与此参数项关联的关键帧连接信息。
   * @return 返回对 KeyframeView::NodeConnections 对象的引用。
   */
  KeyframeView::NodeConnections &GetKeyframeConnections() { return keyframe_connections_; }

  /**
   * @brief 设置此参数项的关键帧连接信息。
   * @param c 要设置的 KeyframeView::NodeConnections 对象。
   */
  void SetKeyframeConnections(const KeyframeView::NodeConnections &c) { keyframe_connections_ = c; }

 signals:
  /**
   * @brief 当用户请求选择某个节点时发出此信号。
   * @param node 指向要选择的 Node 对象的指针。
   */
  void RequestSelectNode(Node *node);

  /**
   * @brief 当数组参数的展开/折叠状态改变时发出此信号。
   * @param e 如果为 true，表示数组已展开；否则为折叠。
   */
  void ArrayExpandedChanged(bool e);

  /**
   * @brief 当某个输入参数旁边的可选复选框状态改变时发出此信号。
   * @param input 发生改变的 NodeInput 的引用。
   * @param e 复选框的新状态 (true 表示勾选)。
   */
  void InputCheckedChanged(const NodeInput &input, bool e);

  /**
   * @brief 请求在查看器中开始编辑文本。
   */
  void RequestEditTextInViewer();

  /**
   * @brief 当数组输入参数的大小发生改变时发出此信号。
   * @param input 发生大小改变的数组参数的名称。
   * @param old_size 数组的旧大小。
   * @param new_size 数组的新大小。
   */
  void InputArraySizeChanged(const QString &input, int old_size, int new_size);

 protected slots:
  /**
   * @brief 重写基类的 Retranslate 方法，用于更新此参数项相关的 UI 文本。
   */
  void Retranslate() override;

 private:
  NodeParamViewItemBody *body_;  ///< 指向内部的参数体控件，实际显示参数UI。

  Node *node_;  ///< 此参数项关联的节点。

  NodeParamViewCheckBoxBehavior create_checkboxes_;  ///< 创建复选框的行为策略。

  Node *ctx_;  ///< 此参数项所属的上下文节点。

  ViewerOutput *time_target_;  ///< 当前的时间目标。

  rational timebase_;  ///< 当前的时间基准。

  KeyframeView::NodeConnections keyframe_connections_;  ///< 存储此节点参数与关键帧视图中轨道的连接信息。

 private slots:
  /**
   * @brief 当需要时（例如，节点类型改变或上下文改变）重新创建内部的参数体控件 (body_)。
   */
  void RecreateBody();
};

}  // namespace olive

#endif  // NODEPARAMVIEWITEM_H
