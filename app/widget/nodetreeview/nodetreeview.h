#ifndef NODETREEVIEW_H
#define NODETREEVIEW_H

#include <QTreeWidget>

#include "node/node.h"

namespace olive {

class NodeTreeView : public QTreeWidget {
  Q_OBJECT
 public:
  explicit NodeTreeView(QWidget* parent = nullptr);

  bool IsNodeEnabled(Node* n) const;

  [[nodiscard]] bool IsInputEnabled(const NodeKeyframeTrackReference& ref) const;

  void SetCheckBoxesEnabled(bool e) { checkboxes_enabled_ = e; }

  void SetKeyframeTrackColor(const NodeKeyframeTrackReference& ref, const QColor& color);

  void SetOnlyShowKeyframable(bool e) { only_show_keyframable_ = e; }

  void SetShowKeyframeTracksAsRows(bool e) { show_keyframe_tracks_as_rows_ = e; }

 public slots:
  void SetNodes(const QVector<Node*>& nodes);

 signals:
  void NodeEnableChanged(Node* n, bool e);

  void InputEnableChanged(const NodeKeyframeTrackReference& ref, bool e);

  void InputSelectionChanged(const NodeKeyframeTrackReference& ref);

  void InputDoubleClicked(const NodeKeyframeTrackReference& ref);

 protected:
  void changeEvent(QEvent* e) override;

  void mouseDoubleClickEvent(QMouseEvent* e) override;

 private:
  void Retranslate();

  NodeKeyframeTrackReference GetSelectedInput();

  QTreeWidgetItem* CreateItem(QTreeWidgetItem* parent, const NodeKeyframeTrackReference& ref);

  void CreateItemsForTracks(QTreeWidgetItem* parent, const NodeInput& input, int track_count);

  static bool UseRGBAOverXYZW(const NodeKeyframeTrackReference& ref);

  enum ItemType { kItemTypeNode, kItemTypeInput };

  static const int kItemType = Qt::UserRole;
  static const int kItemInputReference = Qt::UserRole + 1;
  static const int kItemNodePointer = Qt::UserRole + 1;

  QVector<Node*> nodes_;

  QVector<Node*> disabled_nodes_;

  QVector<NodeKeyframeTrackReference> disabled_inputs_;

  QHash<NodeKeyframeTrackReference, QTreeWidgetItem*> item_map_;

  bool only_show_keyframable_;

  bool show_keyframe_tracks_as_rows_;

  QHash<NodeKeyframeTrackReference, QColor> keyframe_colors_;

  bool checkboxes_enabled_;

 private slots:
  void ItemCheckStateChanged(QTreeWidgetItem* item, int column);

  void SelectionChanged();
};

}  // namespace olive

#endif  // NODETREEVIEW_H
