#ifndef NODEVIEWITEM_H
#define NODEVIEWITEM_H

#include <QFontMetrics>        // Qt 字体度量类，用于计算文本尺寸
#include <QGraphicsRectItem>   // Qt 图形矩形项基类
#include <QLinearGradient>     // Qt 线性渐变类 (可能用于绘制背景或高亮)
#include <QWidget>             // Qt 控件基类 (用于 paint 方法的 widget 参数)


#include "node/node.h"                 // 节点基类定义
#include "nodeviewcommon.h"            // 节点视图通用定义 (例如 FlowDirection)
#include "nodeviewitemconnector.h"     // 节点视图项的连接器（输入/输出点）

namespace olive {

class NodeViewItem; // 前向声明自身，用于某些内部结构或递归引用
class NodeViewEdge; // 前向声明节点视图边类，因为 NodeViewItem 会管理与之相连的边

/**
 * @brief NodeViewItem 类是 NodeView 中 Node 对象的视觉控件表示。
 *
 * 此控件可以折叠或展开以显示/隐藏节点的各种参数。
 *
 * 要检索特定 Node 的 NodeViewItem，请使用 NodeView::NodeToUIObject() (如果该方法存在于 NodeView 中)。
 * 它继承自 QObject (用于信号槽) 和 QGraphicsRectItem (用于在场景中绘制为矩形)。
 */
class NodeViewItem : public QObject, public QGraphicsRectItem {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数，用于表示节点的一个特定输入/元素。
   * @param node 此图形项关联的实际 Node 对象。
   * @param input 如果此项代表一个输入参数，则为输入参数的名称；否则为空 QString。
   * @param element 如果输入参数是多维的（如向量），则为元素索引；否则为 -1。
   * @param context 此节点项所属的父上下文 Node（例如，如果此节点在组内，则 context 是该组节点）。
   * @param parent 父 QGraphicsItem 指针，默认为 nullptr。
   */
  NodeViewItem(Node *node, QString input, int element, Node *context, QGraphicsItem *parent = nullptr);
  /**
   * @brief 构造函数重载，用于表示整个节点（而非特定输入）。
   * @param node 此图形项关联的实际 Node 对象。
   * @param context 此节点项所属的父上下文 Node。
   * @param parent 父 QGraphicsItem 指针，默认为 nullptr。
   */
  NodeViewItem(Node *node, Node *context, QGraphicsItem *parent = nullptr)
      : NodeViewItem(node, QString(), -1, context, parent) {} // 委托给更通用的构造函数

  /**
   * @brief 析构函数。
   */
  ~NodeViewItem() override;

  /**
   * @brief 获取节点在节点图中的位置数据结构。
   * @return 返回 Node::Position 结构体。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Node::Position GetNodePositionData() const;
  /**
   * @brief 获取节点在场景中的位置。
   * @return 返回 QPointF 表示的节点位置。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QPointF GetNodePosition() const;
  /**
   * @brief 设置节点在场景中的位置。
   * @param pos 新的 QPointF 位置。
   */
  void SetNodePosition(const QPointF &pos);
  /**
   * @brief 设置节点的位置（使用 Node::Position 结构体）。
   * @param pos 新的 Node::Position 位置数据。
   */
  void SetNodePosition(const Node::Position &pos);

  /**
   * @brief 递归地获取所有与此节点项（及其子项，如果它是组）相连的边。
   * @return 返回一个包含所有相关 NodeViewEdge 指针的 QVector。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QVector<NodeViewEdge *> GetAllEdgesRecursively() const;

  /**
   * @brief 获取当前附加的 Node 对象。
   * @return 返回 Node 指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Node *GetNode() const { return node_; }

  /**
   * @brief 获取此项代表的节点输入参数。
   * @return 返回一个 NodeInput 对象。如果此项代表整个节点而非特定输入，则 NodeInput 可能无效。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] NodeInput GetInput() const { return NodeInput(node_, input_, element_); }

  /**
   * @brief 获取此节点项所属的上下文节点。
   * @return 返回上下文 Node 指针。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] Node *GetContext() const { return context_; }

  /**
   * @brief 获取节点的展开状态。
   * @return 如果节点项已展开（显示其参数或子项），则返回 true；否则返回 false。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool IsExpanded() const { return expanded_; }

  /**
   * @brief 获取与此节点项直接相连的所有边的列表。
   * @return 返回一个 const 引用，指向包含 NodeViewEdge 指针的 QVector。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] const QVector<NodeViewEdge *> &edges() const { return edges_; }

  /**
   * @brief 设置节点的展开状态。
   * @param e 如果为 true，则展开节点项；否则折叠。
   * @param hide_titlebar 如果为 true，则在折叠时可能隐藏标题栏（特定情况）。默认为 false。
   */
  void SetExpanded(bool e, bool hide_titlebar = false);
  /**
   * @brief 切换节点的展开/折叠状态。
   */
  void ToggleExpanded();

  /**
   * @brief 获取输入连接点在项坐标系中的位置。
   * @return 返回 QPointF 表示的输入点位置。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QPointF GetInputPoint() const;
  /**
   * @brief 获取输出连接点在项坐标系中的位置。
   * @return 返回 QPointF 表示的输出点位置。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] QPointF GetOutputPoint() const;

  /**
   * @brief 设置节点图中节点的流向（布局方向）。
   * @param dir FlowDirection 枚举值。
   */
  void SetFlowDirection(NodeViewCommon::FlowDirection dir);

  /**
   * @brief 获取当前节点图的流向。
   * @return 返回 FlowDirection 枚举值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] NodeViewCommon::FlowDirection GetFlowDirection() const { return flow_dir_; }

  /**
   * @brief 获取默认的文本内边距。
   * @return 返回 int 型的像素值。
   */
  static int DefaultTextPadding();

  /**
   * @brief 获取节点项的默认高度（单位可能是逻辑单位或基于字体）。
   * @return 返回 int 型的高度值。
   */
  static int DefaultItemHeight();

  /**
   * @brief 获取节点项的默认宽度。
   * @return 返回 int 型的宽度值。
   */
  static int DefaultItemWidth();

  /**
   * @brief 获取节点项的默认边框宽度。
   * @return 返回 int 型的边框宽度值。
   */
  static int DefaultItemBorder();

  /**
   * @brief 将节点内部逻辑坐标点转换为屏幕（视图）坐标点。
   * @param p 节点逻辑坐标点。
   * @param direction 当前的流向。
   * @return 返回转换后的屏幕坐标点。
   */
  static QPointF NodeToScreenPoint(QPointF p, NodeViewCommon::FlowDirection direction);
  /**
   * @brief 将屏幕（视图）坐标点转换为节点内部逻辑坐标点。
   * @param p 屏幕坐标点。
   * @param direction 当前的流向。
   * @return 返回转换后的节点逻辑坐标点。
   */
  static QPointF ScreenToNodePoint(QPointF p, NodeViewCommon::FlowDirection direction);

  /**
   * @brief 获取给定流向下的默认水平内边距。
   * @param dir 流向。
   * @return 返回 qreal 型的水平内边距。
   */
  static qreal DefaultItemHorizontalPadding(NodeViewCommon::FlowDirection dir);
  /**
   * @brief 获取给定流向下的默认垂直内边距。
   * @param dir 流向。
   * @return 返回 qreal 型的垂直内边距。
   */
  static qreal DefaultItemVerticalPadding(NodeViewCommon::FlowDirection dir);
  /**
   * @brief 获取当前流向下的默认水平内边距。
   * @return 返回 qreal 型的水平内边距。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] qreal DefaultItemHorizontalPadding() const;
  /**
   * @brief 获取当前流向下的默认垂直内边距。
   * @return 返回 qreal 型的垂直内边距。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] qreal DefaultItemVerticalPadding() const;

  /**
   * @brief 向此节点项添加一条连接边。
   * @param edge 指向要添加的 NodeViewEdge 的指针。
   */
  void AddEdge(NodeViewEdge *edge);
  /**
   * @brief 从此节点项移除一条连接边。
   * @param edge 指向要移除的 NodeViewEdge 的指针。
   */
  void RemoveEdge(NodeViewEdge *edge);

  /**
   * @brief 检查此项是否被标记为上下文的输出。
   *
   * 这可能用于节点组的输出伪节点。
   * @return 如果被标记为输出，则返回 true。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool IsLabelledAsOutputOfContext() const { return label_as_output_; }

  /**
   * @brief 设置此项是否应被标记为上下文的输出。
   * @param e 如果为 true，则标记为输出。
   */
  void SetLabelAsOutput(bool e);

  /**
   * @brief 设置此节点项的高亮状态。
   * @param e 如果为 true，则高亮显示；否则取消高亮。
   */
  void SetHighlighted(bool e);

  /**
   * @brief 获取代表指定输入参数的子 NodeViewItem。
   * @param input 要查找其图形项的 NodeInput。
   * @return 如果找到，则返回 NodeViewItem 指针；否则返回 nullptr。
   */
  NodeViewItem *GetItemForInput(NodeInput input);

  /**
   * @brief 检查此项是否代表一个节点的输出（即它不是一个输入参数项）。
   * @return 如果是输出项（或代表整个节点），则返回 true。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool IsOutputItem() const { return input_.isEmpty(); }

  /**
   * @brief 重新调整所有与此节点项连接的边的位置和路径。
   */
  void ReadjustAllEdges();

  /**
   * @brief 更新子输入项的流向（通常在父项流向改变时调用）。
   * @param child 指向子 NodeViewItem 的指针。
   */
  void UpdateFlowDirectionOfInputItem(NodeViewItem *child);

  /**
   * @brief 检查此节点项是否可以被展开（例如，如果它是一个组节点或有多个参数）。
   * @return 如果可以展开，则返回 true。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool CanBeExpanded() const;

 protected:
  /**
   * @brief 重写 QGraphicsItem 的 paint 方法，用于自定义节点项的绘制。
   * @param painter QPainter 指针。
   * @param option 绘图选项。
   * @param widget 目标 QWidget 指针，默认为 nullptr。
   */
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

  /**
   * @brief 重写 QGraphicsItem 的 mousePressEvent 方法。
   * @param event 鼠标场景事件指针。
   */
  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
  /**
   * @brief 重写 QGraphicsItem 的 mouseMoveEvent 方法。
   * @param event 鼠标场景事件指针。
   */
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
  /**
   * @brief 重写 QGraphicsItem 的 mouseReleaseEvent 方法。
   * @param event 鼠标场景事件指针。
   */
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

  /**
   * @brief 重写 QGraphicsItem 的 itemChange 方法，用于响应项的特定状态变化（如选中、位置改变等）。
   * @param change 正在发生的改变类型。
   * @param value 与改变相关的值。
   * @return 返回处理后的值。
   */
  QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

 private:
  /**
   * @brief 更新其父上下文（NodeViewContext）的矩形区域，以适应此项及其子项的变化。
   */
  void UpdateContextRect();

  /**
   * @brief 绘制节点标题文本。
   * @param painter QPainter 指针。
   * @param text 要绘制的文本。
   * @param rect 文本绘制的目标矩形区域。
   * @param vertical_align 垂直对齐方式。
   * @param icon_full_size 图标占用的完整尺寸（用于文本偏移）。
   */
  void DrawNodeTitle(QPainter *painter, QString text, const QRectF &rect, Qt::Alignment vertical_align,
                     int icon_full_size);

  /**
   * @brief 绘制展开/折叠状态的箭头图标。
   * @param painter QPainter 指针。
   * @return 返回箭头图标区域的高度（或宽度，取决于方向）。
   */
  int DrawExpandArrow(QPainter *painter);

  /**
   * @brief 当节点的逻辑位置数据发生改变时，内部更新函数。
   */
  void UpdateNodePosition();

  /**
   * @brief 更新输入连接器的位置。
   */
  void UpdateInputConnectorPosition();
  /**
   * @brief 更新输出连接器的位置。
   */
  void UpdateOutputConnectorPosition();

  /**
   * @brief 检查指定的输入参数名称是否有效（即是否存在于当前节点中）。
   * @param input 输入参数的名称。
   * @return 如果输入有效，则返回 true。
   */
  bool IsInputValid(const QString &input);

  /**
   * @brief 根据高度单位设置矩形项的尺寸。
   * @param height_units 高度单位数量，默认为1（代表一个标准参数行的高度）。
   */
  void SetRectSize(int height_units = 1);

  /**
   * @brief 更新所有子项（代表输入参数）的位置。
   */
  void UpdateChildrenPositions();

  /**
   * @brief 获取此节点项及其所有子项（如果已展开）在逻辑上占用的总高度。
   * @return 返回逻辑高度值。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] int GetLogicalHeightWithChildren() const;

  /**
   * @brief 引用附加的 Node 对象。
   */
  Node *node_;      ///< 指向此图形项所代表的实际 Node 对象。
  QString input_;   ///< 如果此项代表一个输入参数，则为其名称；否则为空。
  int element_;     ///< 如果输入参数是多维的，则为元素索引；否则为 -1。

  Node *context_;   ///< 此节点项所属的父上下文 Node（例如，所在的组节点）。

  /**
   * @brief 缓存的节点输入子项列表。
   */
  QVector<NodeViewItem *> children_; ///< 如果此项是可展开的（如代表整个节点或组），则存储其子参数项。

  /// 绘制时使用的尺寸变量
  int node_border_width_; ///< 节点项边框的宽度。

  /**
   * @brief 展开状态。
   */
  bool expanded_; ///< 标记此项当前是否已展开以显示其内容/子项。

  bool highlighted_; ///< 标记此项当前是否处于高亮状态。

  NodeViewCommon::FlowDirection flow_dir_; ///< 此节点项内部子项（如果存在）的布局流向。

  QVector<NodeViewEdge *> edges_; ///< 与此节点项（的输入或输出连接器）相连的所有边的列表。

  QPointF cached_node_pos_; ///< 缓存的节点位置，用于优化或检测变化。

  QRect last_arrow_rect_; ///< 上次绘制的展开/折叠箭头图标的矩形区域，用于点击检测。
  bool arrow_click_;      ///< 标记鼠标按下是否在箭头上，用于处理展开/折叠点击。

  NodeViewItemConnector *input_connector_;  ///< 指向输入连接器的图形项。
  NodeViewItemConnector *output_connector_; ///< 指向输出连接器的图形项。

  bool has_connectable_inputs_{}; ///< 标记此节点是否有可连接的输入端点。

  bool label_as_output_; ///< 标记此项是否应被特别标记为上下文的输出（例如，组节点的输出伪节点）。

 private slots:
  /**
   * @brief 当关联节点的显示属性（如颜色、名称）发生改变时调用的槽函数。
   */
  void NodeAppearanceChanged();

  /**
   * @brief 重新填充（创建或更新）代表节点输入参数的子项。
   */
  void RepopulateInputs();

  /**
   * @brief 当节点的某个数组类型的输入参数大小发生改变时调用的槽函数。
   * @param input 发生大小改变的数组参数的名称。
   */
  void InputArraySizeChanged(const QString &input);
};

}  // namespace olive

#endif  // NODEVIEWITEM_H
