#include "projectviewmodel.h"

#include <QDebug>
#include <QMimeData>
#include <QUrl>

#include "common/qtutils.h"
#include "core.h"
#include "node/nodeundo.h"

namespace olive {

ProjectViewModel::ProjectViewModel(QObject *parent) : QAbstractItemModel(parent), project_(nullptr) {}

Project *ProjectViewModel::project() const { return project_; }

void ProjectViewModel::set_project(Project *p) {
  beginResetModel();

  if (project_) {
    DisconnectItem(project_->root());
  }

  project_ = p;

  if (project_) {
    ConnectItem(project_->root());
  }

  endResetModel();
}

QModelIndex ProjectViewModel::index(int row, int column, const QModelIndex &parent) const {
  // I'm actually not 100% sure what this does, but it seems logical and was in the earlier code
  if (!hasIndex(row, column, parent)) {
    return {};
  }

  // Get the parent object, we assume it's a folder since only folders can have children
  auto *item_parent = dynamic_cast<Folder *>(GetItemObjectFromIndex(parent));

  // Return an index to this object
  return createIndex(row, column, item_parent->item_child(row));
}

QModelIndex ProjectViewModel::parent(const QModelIndex &child) const {
  // Get the Item object from the index
  Node *item = GetItemObjectFromIndex(child);

  // Get Item's parent object
  Folder *par = item->folder();

  // If the parent is the root, return an empty index
  if (par == project_->root()) {
    return {};
  }

  // Otherwise return a true index to its parent
  int parent_index = IndexOfChild(par);

  // Make sure the index is valid (there's no reason it shouldn't be)
  Q_ASSERT(parent_index > -1);

  // Return an index to the parent
  return createIndex(parent_index, 0, par);
}

int ProjectViewModel::rowCount(const QModelIndex &parent) const {
  // If there's no project, there are obviously no items to show
  if (project_ == nullptr) {
    return 0;
  }

  // If the index is the root, return the root child count
  if (parent == QModelIndex()) {
    return project_->root()->item_child_count();
  }

  // Otherwise, the index must contain a valid pointer, so we just return its child count
  return dynamic_cast<Folder *>(GetItemObjectFromIndex(parent))->item_child_count();
}

int ProjectViewModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent)

  // Not strictly necessary, but a decent visual cue that there's no project currently active
  if (project_ == nullptr) {
    return 0;
  }

  return kColumnCount;
}

QVariant ProjectViewModel::data(const QModelIndex &index, int role) const {
  Node *internal_item = GetItemObjectFromIndex(index);

  auto column_type = static_cast<ColumnType>(index.column());

  switch (role) {
    case Qt::DisplayRole:
    case kInnerTextRole: {
      // Standard text role

      switch (column_type) {
        case kName:
          return internal_item->GetLabel();
        case kDuration:
          return internal_item->data(Node::DURATION);
        case kRate:
          return internal_item->data(Node::FREQUENCY_RATE);
        case kLastModified:
        case kCreatedTime: {
          qint64 using_time = (column_type == kLastModified) ? internal_item->data(Node::MODIFIED_TIME).toLongLong()
                                                             : internal_item->data(Node::CREATED_TIME).toLongLong();

          if (using_time == 0) {
            // 0 is the null value, return nothing
            break;
          }

          QVariant ret;

          if (role == kInnerTextRole) {
            // Use time value directly for correct sorting
            ret = using_time;
          } else {
            // Display role, format to a human readable string
            ret = QtUtils::GetFormattedDateTime(QDateTime::fromSecsSinceEpoch(using_time));
          }

          return ret;
        }
        case kColumnCount:
          break;
      }
    } break;
    case Qt::EditRole:
      if (column_type == kName) {
        return internal_item->GetLabel();
      }
      break;
    case Qt::DecorationRole:
      // If this is the first column, return the Item's icon
      if (column_type == kName) {
        return internal_item->data(Node::ICON);
      }
      break;
    case Qt::ToolTipRole:
      return internal_item->data(Node::TOOLTIP);
  }

  return {};
}

QVariant ProjectViewModel::headerData(int section, Qt::Orientation orientation, int role) const {
  // Check if we need text data (DisplayRole) and orientation is horizontal
  // FIXME I'm not 100% sure what happens if the orientation is vertical/if that check is necessary
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    auto column_type = static_cast<ColumnType>(section);

    // Return the name based on the column's current type
    switch (column_type) {
      case kName:
        return tr("Name");
      case kDuration:
        return tr("Duration");
      case kRate:
        return tr("Rate");
      case kLastModified:
        return tr("Modified");
      case kCreatedTime:
        return tr("Created");
      case kColumnCount:
        break;
    }
  }

  return QAbstractItemModel::headerData(section, orientation, role);
}

bool ProjectViewModel::hasChildren(const QModelIndex &parent) const {
  // If it's a folder, we always return TRUE in order to always show the "expand triangle" icon,
  // even when there are no "physical" children
  Node *item = GetItemObjectFromIndex(parent);

  return dynamic_cast<Folder *>(item);
}

bool ProjectViewModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  // The name is editable
  if (index.isValid() && index.column() == kName && role == Qt::EditRole) {
    Node *item = GetItemObjectFromIndex(index);

    QString new_name = value.toString();

    if (!new_name.isEmpty()) {
      auto *nrc = new NodeRenameCommand();

      nrc->AddNode(item, value.toString());

      Core::instance()->undo_stack()->push(nrc, tr(R"(Renamed Item "%1" to "%2")").arg(item->GetLabel(), new_name));

      return true;
    }
  }

  return false;
}

bool ProjectViewModel::canFetchMore(const QModelIndex &parent) const {
  // Use the same hack that always returns true with folders so the expand triangle is always visible
  return hasChildren(parent);
}

Qt::ItemFlags ProjectViewModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) {
    // Allow dropping files from external sources
    return Qt::ItemIsDropEnabled;
  }

  Qt::ItemFlags f = Qt::ItemIsDragEnabled | QAbstractItemModel::flags(index);

  if (dynamic_cast<Folder *>(GetItemObjectFromIndex(index))) {
    f |= Qt::ItemIsDropEnabled;
  }

  // If the column is the kName column, that means it's editable
  if (index.column() == kName) {
    f |= Qt::ItemIsEditable;
  }

  return f;
}

QStringList ProjectViewModel::mimeTypes() const {
  // Allow data from this model and a file list from external sources
  return {Project::kItemMimeType, QStringLiteral("text/uri-list")};
}

QMimeData *ProjectViewModel::mimeData(const QModelIndexList &indexes) const {
  // Compliance with Qt standard
  if (indexes.isEmpty()) {
    return nullptr;
  }

  // Encode mime data for the rows/items that were dragged
  auto *data = new QMimeData();

  // Use QDataStream to stream the item data into a byte array
  QByteArray encoded_data;
  QDataStream stream(&encoded_data, QIODevice::WriteOnly);

  // The indexes list includes indexes for each column which we don't use. To make sure each row only gets sent *once*,
  // we keep a list of dragged items
  QVector<void *> dragged_items;

  foreach (QModelIndex index, indexes) {
    if (index.isValid()) {
      // Check if we've dragged this item before
      if (!dragged_items.contains(index.internalPointer())) {
        // If not, add it to the stream (and also keep track of it in the vector)
        Node *item = static_cast<Node *>(index.internalPointer());
        QVector<Track::Reference> streams;

        if (auto *footage = dynamic_cast<ViewerOutput *>(item)) {
          streams = footage->GetEnabledStreamsAsReferences();
        }

        stream << streams << reinterpret_cast<quintptr>(item);

        dragged_items.append(item);
      }
    }
  }

  // Set byte array as the mime data and return the mime data
  data->setData(Project::kItemMimeType, encoded_data);

  return data;
}

bool ProjectViewModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                    const QModelIndex &drop) {
  // Default recommended checks from
  // https://doc.qt.io/qt-5/model-view-programming.html#using-drag-and-drop-with-item-views
  if (!canDropMimeData(data, action, row, column, drop)) {
    return false;
  }

  if (action == Qt::IgnoreAction) {
    return true;
  }

  // Probe mime data for its format
  QStringList mime_formats = data->formats();

  if (mime_formats.contains(Project::kItemMimeType)) {
    // Data is drag/drop data from this model
    QByteArray model_data = data->data(Project::kItemMimeType);

    // Use QDataStream to deserialize the data
    QDataStream stream(&model_data, QIODevice::ReadOnly);

    // Get the Item object that the items were dropped on
    auto *drop_location = dynamic_cast<Folder *>(GetItemObjectFromIndex(drop));

    // If this is not a folder, we cannot drop these items here
    if (!drop_location) {
      return false;
    }

    // Variables to deserialize into
    quintptr item_ptr;
    QList<Track::Reference> streams;

    // Loop through all data
    auto *move_command = new MultiUndoCommand();

    int count = 0;

    while (!stream.atEnd()) {
      stream >> streams >> item_ptr;

      Node *item = reinterpret_cast<Node *>(item_ptr);

      // Check if Item is already the drop location or if its parent is the drop location, in which case this is a
      // no-op

      if (item != drop_location && item->folder() != drop_location &&
          (!dynamic_cast<Folder *>(item) || !ItemIsParentOfChild(dynamic_cast<Folder *>(item), drop_location))) {
        move_command->add_child(new NodeEdgeRemoveCommand(
            item, NodeInput(item->folder(), Folder::kChildInput, item->folder()->index_of_child_in_array(item))));
        move_command->add_child(new FolderAddChild(drop_location, item));
        count++;
      }
    }

    Core::instance()->undo_stack()->push(move_command, tr("Move %1 Item(s)").arg(count));

    return true;

  } else if (mime_formats.contains(QStringLiteral("text/uri-list"))) {
    // We received a list of files
    QByteArray file_data = data->data(QStringLiteral("text/uri-list"));

    // Use text stream to parse (just an easy way of sifting through line breaks
    QTextStream stream(&file_data);

    // Convert QByteArray to QStringList (which Core takes for importing)
    QStringList urls;
    while (!stream.atEnd()) {
      QUrl url = stream.readLine();

      if (!url.isEmpty()) {
        urls.append(url.toLocalFile());
      }
    }

    // Get folder dropped onto
    Node *drop_item = GetItemObjectFromIndex(drop);

    // If we didn't drop onto an item, find the nearest parent folder (should eventually terminate at root either way)
    if (!dynamic_cast<Folder *>(drop_item)) {
      drop_item = drop_item->folder();

      if (!drop_item) {
        // Failed to find folder to place this in
        return false;
      }
    }

    // Trigger an import
    Core::instance()->ImportFiles(urls, dynamic_cast<Folder *>(drop_item));

    return true;
  }

  return false;
}

int ProjectViewModel::IndexOfChild(Node *item) {
  // Find parent's index within its own parent
  Folder *parent = item->folder();

  if (parent) {
    return parent->index_of_child(item);
  }

  return -1;
}

Node *ProjectViewModel::GetItemObjectFromIndex(const QModelIndex &index) const {
  if (index.isValid()) {
    return static_cast<Node *>(index.internalPointer());
  }

  return project_ ? project_->root() : nullptr;
}

bool ProjectViewModel::ItemIsParentOfChild(Folder *parent, Node *child) {
  // Loop through parent hierarchy checking if `parent` is one of its parents
  do {
    child = child->folder();

    if (parent == child) {
      return true;
    }
  } while (child != nullptr);

  return false;
}

void ProjectViewModel::ConnectItem(Node *n) {
  connect(n, &Node::LabelChanged, this, &ProjectViewModel::ItemRenamed);

  auto *f = dynamic_cast<Folder *>(n);
  if (f) {
    connect(f, &Folder::BeginInsertItem, this, &ProjectViewModel::FolderBeginInsertItem);
    connect(f, &Folder::EndInsertItem, this, &ProjectViewModel::FolderEndInsertItem);
    connect(f, &Folder::BeginRemoveItem, this, &ProjectViewModel::FolderBeginRemoveItem);
    connect(f, &Folder::EndRemoveItem, this, &ProjectViewModel::FolderEndRemoveItem);

    foreach (Node *c, f->children()) {
      ConnectItem(c);
    }
  }
}

void ProjectViewModel::DisconnectItem(Node *n) {
  disconnect(n, &Node::LabelChanged, this, &ProjectViewModel::ItemRenamed);

  auto *f = dynamic_cast<Folder *>(n);
  if (f) {
    disconnect(f, &Folder::BeginInsertItem, this, &ProjectViewModel::FolderBeginInsertItem);
    disconnect(f, &Folder::EndInsertItem, this, &ProjectViewModel::FolderEndInsertItem);
    disconnect(f, &Folder::BeginRemoveItem, this, &ProjectViewModel::FolderBeginRemoveItem);
    disconnect(f, &Folder::EndRemoveItem, this, &ProjectViewModel::FolderEndRemoveItem);

    foreach (Node *c, f->children()) {
      DisconnectItem(c);
    }
  }
}

void ProjectViewModel::FolderBeginInsertItem(Node *n, int insert_index) {
  auto *folder = dynamic_cast<Folder *>(sender());

  ConnectItem(n);

  QModelIndex index;

  if (folder != project_->root()) {
    index = CreateIndexFromItem(folder);
  }

  beginInsertRows(index, insert_index, insert_index);
}

void ProjectViewModel::FolderEndInsertItem() { endInsertRows(); }

void ProjectViewModel::FolderBeginRemoveItem(Node *n, int child_index) {
  auto *folder = dynamic_cast<Folder *>(sender());

  DisconnectItem(n);

  QModelIndex index;

  if (folder != project_->root()) {
    index = CreateIndexFromItem(folder);
  }

  beginRemoveRows(index, child_index, child_index);
}

void ProjectViewModel::FolderEndRemoveItem() { endRemoveRows(); }

void ProjectViewModel::ItemRenamed() {
  Node *item = dynamic_cast<Node *>(sender());

  QModelIndex index = CreateIndexFromItem(item);

  emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
}

QModelIndex ProjectViewModel::CreateIndexFromItem(Node *item, int column) {
  return createIndex(IndexOfChild(item), column, item);
}

}  // namespace olive
