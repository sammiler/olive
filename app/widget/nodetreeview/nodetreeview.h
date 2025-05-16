#ifndef NODETREEVIEW_H
#define NODETREEVIEW_H

#include <QTreeWidget> // Qt 树形控件基类
#include <QVector>     // Qt 动态数组容器
#include <QHash>       // Qt 哈希表容器
#include <QColor>      // Qt 颜色类

#include "node/node.h"  // 节点基类定义
#include "node/param.h" // 节点参数相关定义 (包含 NodeKeyframeTrackReference, NodeInput)

// 前向声明 Qt 类 (根据用户要求，不添加)
// class QWidget;
// class QEvent;
// class QMouseEvent;
// class QTreeWidgetItem;
// class QString;

namespace olive {

/**
 * @brief NodeTreeView 类是一个用于显示节点及其可动画参数（关键帧轨道）的树形视图控件。
 *
 * 它继承自 QTreeWidget，允许用户查看节点的层级结构以及每个节点内部可设置关键帧的参数。
 * 用户可以通过此视图启用/禁用节点或参数的动画，选择参数以在其他视图（如曲线编辑器）中编辑，
 * 以及自定义某些显示属性（如轨道颜色）。
 */
class NodeTreeView : public QTreeWidget {
  Q_OBJECT // Qt 元对象系统宏

 public:
  /**
   * @brief 构造函数。
   * @param parent 父控件指针，默认为 nullptr。
   */
  explicit NodeTreeView(QWidget* parent = nullptr);

  /**
   * @brief 检查指定的节点当前是否处于启用状态。
   * @param n 要检查的 Node 指针。
   * @return 如果节点已启用，则返回 true；否则返回 false。
   */
  bool IsNodeEnabled(Node* n) const;

  /**
   * @brief 检查指定的输入参数（关键帧轨道）当前是否处于启用状态。
   * @param ref 对 NodeKeyframeTrackReference 的 const 引用，指定了要检查的轨道。
   * @return 如果输入参数已启用，则返回 true；否则返回 false。
   * @note [[nodiscard]] 属性提示调用者应使用此函数的返回值。
   */
  [[nodiscard]] bool IsInputEnabled(const NodeKeyframeTrackReference& ref) const;

  /**
   * @brief 设置是否启用树视图中各项的复选框。
   * @param e 如果为 true，则启用复选框；否则禁用。
   */
  void SetCheckBoxesEnabled(bool e) { checkboxes_enabled_ = e; }

  /**
   * @brief 为指定的关键帧轨道设置显示颜色。
   * @param ref 要设置颜色的轨道的引用 (NodeKeyframeTrackReference)。
   * @param color 要设置的 QColor。
   */
  void SetKeyframeTrackColor(const NodeKeyframeTrackReference& ref, const QColor& color);

  /**
   * @brief 设置是否仅显示可设置关键帧的参数。
   * @param e 如果为 true，则只显示可动画的参数；否则显示所有参数。
   */
  void SetOnlyShowKeyframable(bool e) { only_show_keyframable_ = e; }

  /**
   * @brief 设置是否将关键帧轨道（参数的各个分量）显示为单独的行。
   * @param e 如果为 true，则每个分量显示为一行；否则参数可能只显示为一项。
   */
  void SetShowKeyframeTracksAsRows(bool e) { show_keyframe_tracks_as_rows_ = e; }

 public slots:
  /**
   * @brief 设置要在树视图中显示的节点列表。
   *
   * 此函数会清除现有内容并根据提供的节点列表重新构建树。
   * @param nodes 包含要显示的 Node 指针的 QVector。
   */
  void SetNodes(const QVector<Node*>& nodes);

 signals:
  /**
   * @brief 当节点的启用状态（通过复选框）发生改变时发出此信号。
   * @param n 发生状态改变的 Node 指针。
   * @param e 节点新的启用状态 (true 表示启用)。
   */
  void NodeEnableChanged(Node* n, bool e);

  /**
   * @brief 当输入参数（关键帧轨道）的启用状态（通过复选框）发生改变时发出此信号。
   * @param ref 发生状态改变的轨道的引用 (NodeKeyframeTrackReference)。
   * @param e 输入参数新的启用状态 (true 表示启用)。
   */
  void InputEnableChanged(const NodeKeyframeTrackReference& ref, bool e);

  /**
   * @brief 当树视图中的当前选中项（通常代表一个输入参数轨道）发生改变时发出此信号。
   * @param ref 新选中的轨道的引用 (NodeKeyframeTrackReference)。如果没有选中项，可能是无效引用。
   */
  void InputSelectionChanged(const NodeKeyframeTrackReference& ref);

  /**
   * @brief 当树视图中的某个输入参数项被双击时发出此信号。
   * @param ref 被双击的轨道的引用 (NodeKeyframeTrackReference)。
   */
  void InputDoubleClicked(const NodeKeyframeTrackReference& ref);

 protected:
  /**
   * @brief 处理 Qt 的 changeEvent 事件。
   *
   * 主要用于响应 QEvent::LanguageChange 事件，以便在应用程序语言设置更改时
   * 调用 Retranslate() 方法来更新树中各项的文本。
   * @param e 指向 QEvent 对象的指针。
   */
  void changeEvent(QEvent* e) override;

  /**
   * @brief 鼠标双击事件处理函数。
   *
   * 当用户双击树中的某一项时，如果该项代表一个输入参数，则发出 InputDoubleClicked 信号。
   * @param e 鼠标事件指针。
   */
  void mouseDoubleClickEvent(QMouseEvent* e) override;

 private:
  /**
   * @brief 重新翻译树视图中所有项的文本。
   *
   * 此方法会在构造时以及语言更改时被调用。
   */
  void Retranslate();

  /**
   * @brief 获取当前选中的输入参数轨道的引用。
   * @return 如果有选中的输入轨道项，则返回其 NodeKeyframeTrackReference；否则可能返回无效引用。
   */
  NodeKeyframeTrackReference GetSelectedInput();

  /**
   * @brief 为指定的关键帧轨道引用创建一个新的 QTreeWidgetItem。
   * @param parent 新项的父 QTreeWidgetItem 指针 (如果是顶层节点项，则为 nullptr)。
   * @param ref 要为其创建项的轨道的引用 (NodeKeyframeTrackReference)。
   * @return 返回创建的 QTreeWidgetItem 指针。
   */
  QTreeWidgetItem* CreateItem(QTreeWidgetItem* parent, const NodeKeyframeTrackReference& ref);

  /**
   * @brief 为指定输入参数的多个轨道（例如向量的各个分量）创建树形项。
   * @param parent 这些轨道项的父 QTreeWidgetItem 指针（通常是代表输入参数本身的项）。
   * @param input 相关的 NodeInput 对象。
   * @param track_count 此输入参数包含的轨道数量。
   */
  void CreateItemsForTracks(QTreeWidgetItem* parent, const NodeInput& input, int track_count);

  /**
   * @brief 判断对于给定的轨道引用，是否应优先使用 RGBA (而非 XYZW) 作为分量标签。
   * @param ref 轨道的引用 (NodeKeyframeTrackReference)。
   * @return 如果应使用 RGBA，则返回 true；否则返回 false。
   */
  static bool UseRGBAOverXYZW(const NodeKeyframeTrackReference& ref);

  /**
   * @brief 定义树形项的类型，用于区分是节点项还是输入参数项。
   */
  enum ItemType {
    kItemTypeNode,  ///< 项代表一个节点。
    kItemTypeInput  ///< 项代表一个输入参数或其分量。
  };

  // 自定义 QTreeWidgetItem 数据角色的常量定义
  static const int kItemType = Qt::UserRole; ///< 用于存储项类型 (ItemType) 的角色。
  static const int kItemInputReference = Qt::UserRole + 1; ///< 用于存储 NodeKeyframeTrackReference (如果项是输入参数) 的角色。
  static const int kItemNodePointer = Qt::UserRole + 1;    ///< 用于存储 Node* (如果项是节点) 的角色 (与 kItemInputReference 值相同，使用时需根据 kItemType 区分)。

  QVector<Node*> nodes_; ///< 当前在视图中显示或管理的节点列表。

  QVector<Node*> disabled_nodes_; ///< 存储被用户禁用的节点列表。
  QVector<NodeKeyframeTrackReference> disabled_inputs_; ///< 存储被用户禁用的输入参数轨道列表。

  QHash<NodeKeyframeTrackReference, QTreeWidgetItem*> item_map_; ///< 存储从 NodeKeyframeTrackReference 到对应 QTreeWidgetItem 的映射，用于快速查找。

  bool only_show_keyframable_; ///< 标记是否仅显示可设置关键帧的参数。
  bool show_keyframe_tracks_as_rows_; ///< 标记是否将参数的各个轨道（分量）显示为单独的行。

  QHash<NodeKeyframeTrackReference, QColor> keyframe_colors_; ///< 存储每个轨道的自定义显示颜色。

  bool checkboxes_enabled_; ///< 标记是否启用项旁边的复选框。

 private slots:
  /**
   * @brief 当树形项的复选框状态发生改变时调用的槽函数。
   *
   * 此函数会根据改变的项是节点还是输入参数，发出相应的 NodeEnableChanged 或 InputEnableChanged 信号。
   * @param item 状态发生改变的 QTreeWidgetItem 指针。
   * @param column 复选框所在的列索引 (通常为0)。
   */
  void ItemCheckStateChanged(QTreeWidgetItem* item, int column);

  /**
   * @brief 当树视图中的选择项发生改变时调用的槽函数。
   *
   * 此函数会获取当前选中的输入参数轨道，并发出 InputSelectionChanged 信号。
   */
  void SelectionChanged();
};

}  // namespace olive

#endif  // NODETREEVIEW_H
